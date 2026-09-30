/**
 * @file banker.c
 * @brief Banker's Algorithm Implementation for Deadlock Avoidance.
 */

#include "avoidance/banker.h"
#include <stdio.h>
#include <string.h>

bool banker_check_safety(const ResourceManager *rm, SafeSequence *out_seq) {
    if (!rm) return false;

    int work[DD_MAX_RESOURCES];
    bool finish[DD_MAX_PROCESSES];
    int active_process_count = 0;

    if (out_seq) {
        memset(out_seq, 0, sizeof(SafeSequence));
    }

    for (int r = 0; r < rm->num_resources; ++r) {
        work[r] = rm->available[r];
    }

    for (int p = 0; p < rm->num_processes; ++p) {
        if (rm->processes[p].state == PROC_UNUSED ||
            rm->processes[p].state == PROC_TERMINATED ||
            rm->processes[p].state == PROC_ABORTED) {
            finish[p] = true;
        } else {
            finish[p] = false;
            active_process_count++;
        }
    }

    int completed_count = 0;
    while (completed_count < active_process_count) {
        bool found = false;

        for (int p = 0; p < rm->num_processes; ++p) {
            if (finish[p]) continue;

            bool can_allocate = true;
            for (int r = 0; r < rm->num_resources; ++r) {
                if (rm->need[p][r] > work[r]) {
                    can_allocate = false;
                    break;
                }
            }

            if (can_allocate) {
                for (int r = 0; r < rm->num_resources; ++r) {
                    work[r] += rm->allocation[p][r];
                }
                finish[p] = true;
                found = true;
                if (out_seq) {
                    out_seq->sequence[out_seq->count++] = p;
                }
                completed_count++;
                break;
            }
        }

        if (!found) {
            if (out_seq) {
                out_seq->is_safe = false;
            }
            return false;
        }
    }

    if (out_seq) {
        out_seq->is_safe = true;
    }
    return true;
}

bool banker_evaluate_request(const ResourceManager *rm, int pid, const int req_vector[], SafeSequence *out_seq) {
    if (!rm || !req_vector || pid < 0 || pid >= rm->num_processes) return false;

    for (int r = 0; r < rm->num_resources; ++r) {
        if (req_vector[r] > rm->need[pid][r]) {
            return false;
        }
    }

    for (int r = 0; r < rm->num_resources; ++r) {
        if (req_vector[r] > rm->available[r]) {
            return false;
        }
    }

    ResourceManager temp_rm = *rm;

    for (int r = 0; r < temp_rm.num_resources; ++r) {
        temp_rm.available[r] -= req_vector[r];
        temp_rm.resources[r].available_instances = temp_rm.available[r];
        temp_rm.allocation[pid][r] += req_vector[r];
        temp_rm.need[pid][r] -= req_vector[r];
    }

    return banker_check_safety(&temp_rm, out_seq);
}
