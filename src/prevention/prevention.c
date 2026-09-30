/**
 * @file prevention.c
 * @brief Deadlock Prevention Strategies and Rule Enforcers.
 */

#include "prevention/prevention.h"
#include <stdio.h>
#include <string.h>

bool prevention_check_resource_ordering(const ResourceManager *rm, int pid, int rid) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources) {
        return false;
    }

    int target_rank = rm->resources[rid].hierarchy_rank;

    for (int r = 0; r < rm->num_resources; ++r) {
        if (rm->allocation[pid][r] > 0) {
            int held_rank = rm->resources[r].hierarchy_rank;
            if (target_rank <= held_rank) {
                return false;
            }
        }
    }

    return true;
}

bool prevention_check_all_or_nothing(const ResourceManager *rm, int pid, int rid) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources) {
        return false;
    }

    for (int r = 0; r < rm->num_resources; ++r) {
        if (rm->allocation[pid][r] > 0) {
            return false;
        }
    }

    return true;
}

bool prevention_validate_request(const ResourceManager *rm, int pid, int rid, char *out_reason, size_t reason_len) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources) {
        if (out_reason && reason_len > 0) {
            snprintf(out_reason, reason_len, "Invalid process or resource index");
        }
        return false;
    }

    PreventionStrategy strat = rm->prevention_strategy;
    if (rm->mode == MODE_PREVENTION && strat == PREVENT_NONE) {
        strat = PREVENT_RESOURCE_ORDERING;
    }

    switch (strat) {
        case PREVENT_RESOURCE_ORDERING:
            if (!prevention_check_resource_ordering(rm, pid, rid)) {
                if (out_reason && reason_len > 0) {
                    snprintf(out_reason, reason_len,
                             "Violates Havender's Resource Ordering (requested rank %d <= held rank)",
                             rm->resources[rid].hierarchy_rank);
                }
                return false;
            }
            break;

        case PREVENT_ALL_OR_NOTHING:
            if (!prevention_check_all_or_nothing(rm, pid, rid)) {
                if (out_reason && reason_len > 0) {
                    snprintf(out_reason, reason_len,
                             "Violates Hold-and-Wait Prevention (must request all resources at start or release held first)");
                }
                return false;
            }
            break;

        case PREVENT_NO_PREEMPTION:
        case PREVENT_NONE:
        default:
            break;
    }

    return true;
}
