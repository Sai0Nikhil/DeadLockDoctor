/**
 * @file graph.c
 * @brief Graph representations (RAG & WFG) and cycle detection for DeadlockDoctor.
 */

#include "core/graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void wfg_init(WaitForGraph *wfg) {
    if (!wfg) return;
    memset(wfg, 0, sizeof(WaitForGraph));
    for (int i = 0; i < DD_MAX_PROCESSES; ++i) {
        for (int j = 0; j < DD_MAX_PROCESSES; ++j) {
            wfg->waiting_on_res[i][j] = -1;
        }
    }
}

void wfg_add_edge(WaitForGraph *wfg, int waiting_pid, int holding_pid, int rid) {
    if (!wfg || waiting_pid < 0 || waiting_pid >= DD_MAX_PROCESSES ||
        holding_pid < 0 || holding_pid >= DD_MAX_PROCESSES) {
        return;
    }
    wfg->adj[waiting_pid][holding_pid] = true;
    wfg->waiting_on_res[waiting_pid][holding_pid] = rid;
}

bool wfg_has_edge(const WaitForGraph *wfg, int waiting_pid, int holding_pid) {
    if (!wfg || waiting_pid < 0 || waiting_pid >= DD_MAX_PROCESSES ||
        holding_pid < 0 || holding_pid >= DD_MAX_PROCESSES) {
        return false;
    }
    return wfg->adj[waiting_pid][holding_pid];
}

void rag_init(ResourceAllocationGraph *rag) {
    if (!rag) return;
    memset(rag, 0, sizeof(ResourceAllocationGraph));
}

void rag_add_request_edge(ResourceAllocationGraph *rag, int pid, int rid, int units) {
    if (!rag || pid < 0 || pid >= DD_MAX_PROCESSES || rid < 0 || rid >= DD_MAX_RESOURCES) return;
    rag->request_edges[pid][rid] += units;
}

void rag_add_assignment_edge(ResourceAllocationGraph *rag, int rid, int pid, int units) {
    if (!rag || pid < 0 || pid >= DD_MAX_PROCESSES || rid < 0 || rid >= DD_MAX_RESOURCES) return;
    rag->assignment_edges[rid][pid] += units;
}

enum Color { WHITE = 0, GRAY = 1, BLACK = 2 };

static void dfs_visit(
    const WaitForGraph *wfg,
    int u,
    int color[],
    int parent[],
    int path[],
    int path_len,
    DeadlockReport *report,
    bool in_deadlock[]
) {
    color[u] = GRAY;
    path[path_len] = u;

    for (int v = 0; v < wfg->num_nodes; ++v) {
        if (!wfg->adj[u][v]) continue;

        if (color[v] == GRAY) {
            report->has_deadlock = true;
            if (report->num_cycles < DD_MAX_CYCLES) {
                DeadlockCycle *cycle = &report->cycles[report->num_cycles++];
                cycle->length = 0;
                
                int start_idx = 0;
                for (int i = 0; i <= path_len; ++i) {
                    if (path[i] == v) {
                        start_idx = i;
                        break;
                    }
                }
                for (int i = start_idx; i <= path_len; ++i) {
                    cycle->pids[cycle->length++] = path[i];
                    in_deadlock[path[i]] = true;
                }
            }
        } else if (color[v] == WHITE) {
            parent[v] = u;
            dfs_visit(wfg, v, color, parent, path, path_len + 1, report, in_deadlock);
        }
    }

    color[u] = BLACK;
}

bool wfg_find_cycles(const WaitForGraph *wfg, DeadlockReport *report) {
    if (!wfg || !report) return false;

    memset(report, 0, sizeof(DeadlockReport));
    int color[DD_MAX_PROCESSES] = {0};
    int parent[DD_MAX_PROCESSES];
    int path[DD_MAX_PROCESSES];
    bool in_deadlock[DD_MAX_PROCESSES] = {0};

    for (int i = 0; i < DD_MAX_PROCESSES; ++i) {
        parent[i] = -1;
    }

    for (int u = 0; u < wfg->num_nodes; ++u) {
        if (color[u] == WHITE) {
            dfs_visit(wfg, u, color, parent, path, 0, report, in_deadlock);
        }
    }

    report->num_deadlocked_processes = 0;
    for (int i = 0; i < wfg->num_nodes; ++i) {
        if (in_deadlock[i]) {
            report->deadlocked_pids[report->num_deadlocked_processes++] = i;
        }
    }

    return report->has_deadlock;
}

bool rag_find_cycles(const ResourceAllocationGraph *rag, DeadlockReport *report) {
    if (!rag || !report) return false;

    WaitForGraph wfg;
    wfg_init(&wfg);
    wfg.num_nodes = rag->num_proc_nodes;

    for (int p1 = 0; p1 < rag->num_proc_nodes; ++p1) {
        for (int r = 0; r < rag->num_res_nodes; ++r) {
            if (rag->request_edges[p1][r] > 0) {
                for (int p2 = 0; p2 < rag->num_proc_nodes; ++p2) {
                    if (p1 != p2 && rag->assignment_edges[r][p2] > 0) {
                        wfg_add_edge(&wfg, p1, p2, r);
                    }
                }
            }
        }
    }

    return wfg_find_cycles(&wfg, report);
}
