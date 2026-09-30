/**
 * @file detector.c
 * @brief Deadlock Detection Algorithms (Single-instance WFG & Multi-instance Matrix).
 */

#include "detection/detector.h"
#include <stdio.h>
#include <string.h>

void detector_build_wfg(const ResourceManager *rm, WaitForGraph *wfg) {
    if (!rm || !wfg) return;

    wfg_init(wfg);
    wfg->num_nodes = rm->num_processes;
    for (int i = 0; i < rm->num_processes; ++i) {
        wfg->pids[i] = i;
    }

    for (int i = 0; i < rm->num_processes; ++i) {
        if (rm->processes[i].state != PROC_BLOCKED) continue;

        for (int r = 0; r < rm->num_resources; ++r) {
            if (rm->request[i][r] > 0) {
                for (int j = 0; j < rm->num_processes; ++j) {
                    if (i != j && rm->allocation[j][r] > 0) {
                        wfg_add_edge(wfg, i, j, r);
                    }
                }
            }
        }
    }
}

void detector_build_rag(const ResourceManager *rm, ResourceAllocationGraph *rag) {
    if (!rm || !rag) return;

    rag_init(rag);
    rag->num_proc_nodes = rm->num_processes;
    rag->num_res_nodes = rm->num_resources;

    for (int i = 0; i < rm->num_processes; ++i) {
        rag->proc_ids[i] = i;
    }
    for (int r = 0; r < rm->num_resources; ++r) {
        rag->res_ids[r] = r;
    }

    for (int p = 0; p < rm->num_processes; ++p) {
        for (int r = 0; r < rm->num_resources; ++r) {
            if (rm->request[p][r] > 0) {
                rag_add_request_edge(rag, p, r, rm->request[p][r]);
            }
            if (rm->allocation[p][r] > 0) {
                rag_add_assignment_edge(rag, r, p, rm->allocation[p][r]);
            }
        }
    }
}

bool detector_detect_wfg_cycle(const ResourceManager *rm, DeadlockReport *out_report) {
    if (!rm || !out_report) return false;

    WaitForGraph wfg;
    detector_build_wfg(rm, &wfg);
    return wfg_find_cycles(&wfg, out_report);
}

bool detector_detect_multi_instance(const ResourceManager *rm, DeadlockReport *out_report) {
    if (!rm || !out_report) return false;

    memset(out_report, 0, sizeof(DeadlockReport));

    int work[DD_MAX_RESOURCES];
    bool finish[DD_MAX_PROCESSES];

    for (int r = 0; r < rm->num_resources; ++r) {
        work[r] = rm->available[r];
    }

    for (int p = 0; p < rm->num_processes; ++p) {
        if (rm->processes[p].state == PROC_UNUSED ||
            rm->processes[p].state == PROC_TERMINATED ||
            rm->processes[p].state == PROC_ABORTED) {
            finish[p] = true;
            continue;
        }

        bool has_allocation = false;
        for (int r = 0; r < rm->num_resources; ++r) {
            if (rm->allocation[p][r] > 0) {
                has_allocation = true;
                break;
            }
        }

        finish[p] = !has_allocation;
    }

    bool progress = true;
    while (progress) {
        progress = false;

        for (int p = 0; p < rm->num_processes; ++p) {
            if (finish[p]) continue;

            bool can_satisfy = true;
            for (int r = 0; r < rm->num_resources; ++r) {
                if (rm->request[p][r] > work[r]) {
                    can_satisfy = false;
                    break;
                }
            }

            if (can_satisfy) {
                for (int r = 0; r < rm->num_resources; ++r) {
                    work[r] += rm->allocation[p][r];
                }
                finish[p] = true;
                progress = true;
                break;
            }
        }
    }

    out_report->has_deadlock = false;
    out_report->num_deadlocked_processes = 0;

    for (int p = 0; p < rm->num_processes; ++p) {
        if (!finish[p] &&
            rm->processes[p].state != PROC_UNUSED &&
            rm->processes[p].state != PROC_TERMINATED &&
            rm->processes[p].state != PROC_ABORTED) {
            out_report->has_deadlock = true;
            out_report->deadlocked_pids[out_report->num_deadlocked_processes++] = p;
        }
    }

    WaitForGraph wfg;
    detector_build_wfg(rm, &wfg);
    DeadlockReport cycle_report;
    if (wfg_find_cycles(&wfg, &cycle_report)) {
        out_report->num_cycles = cycle_report.num_cycles;
        for (int c = 0; c < cycle_report.num_cycles; ++c) {
            out_report->cycles[c] = cycle_report.cycles[c];
        }
    }

    return out_report->has_deadlock;
}

bool detector_detect_deadlock(const ResourceManager *rm, DeadlockReport *out_report) {
    if (!rm || !out_report) return false;
    return detector_detect_multi_instance(rm, out_report);
}
