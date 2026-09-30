/**
 * @file resource_mgr.c
 * @brief Core Resource Manager Implementation.
 */

#include "core/resource_mgr.h"
#include "avoidance/banker.h"
#include "detection/detector.h"
#include "prevention/prevention.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void rm_init(ResourceManager *rm, OperatingMode mode) {
    if (!rm) return;
    memset(rm, 0, sizeof(ResourceManager));
    pthread_mutex_init(&rm->lock, NULL);
    rm->mode = mode;
    rm->prevention_strategy = PREVENT_NONE;
    rm->recovery_policy = RECOVERY_CHEAPEST;
    rm->verbose_logging = true;
}

void rm_destroy(ResourceManager *rm) {
    if (!rm) return;
    pthread_mutex_destroy(&rm->lock);
}

void rm_reset(ResourceManager *rm) {
    if (!rm) return;
    pthread_mutex_lock(&rm->lock);
    OperatingMode old_mode = rm->mode;
    PreventionStrategy old_prev = rm->prevention_strategy;
    VictimPolicy old_rec = rm->recovery_policy;
    bool old_v = rm->verbose_logging;

    memset(rm, 0, sizeof(ResourceManager));
    pthread_mutex_init(&rm->lock, NULL);
    rm->mode = old_mode;
    rm->prevention_strategy = old_prev;
    rm->recovery_policy = old_rec;
    rm->verbose_logging = old_v;
    pthread_mutex_unlock(&rm->lock);
}

void rm_set_mode(ResourceManager *rm, OperatingMode mode) {
    if (!rm) return;
    pthread_mutex_lock(&rm->lock);
    rm->mode = mode;
    pthread_mutex_unlock(&rm->lock);
}

void rm_set_prevention_strategy(ResourceManager *rm, PreventionStrategy strategy) {
    if (!rm) return;
    pthread_mutex_lock(&rm->lock);
    rm->prevention_strategy = strategy;
    pthread_mutex_unlock(&rm->lock);
}

void rm_set_victim_policy(ResourceManager *rm, VictimPolicy policy) {
    if (!rm) return;
    pthread_mutex_lock(&rm->lock);
    rm->recovery_policy = policy;
    pthread_mutex_unlock(&rm->lock);
}

void rm_set_verbose(ResourceManager *rm, bool verbose) {
    if (!rm) return;
    pthread_mutex_lock(&rm->lock);
    rm->verbose_logging = verbose;
    pthread_mutex_unlock(&rm->lock);
}

int rm_register_resource(ResourceManager *rm, const char *name, int total_instances) {
    if (!rm || !name || total_instances <= 0) return -1;
    pthread_mutex_lock(&rm->lock);

    if (rm->num_resources >= DD_MAX_RESOURCES) {
        pthread_mutex_unlock(&rm->lock);
        return -1;
    }

    for (int i = 0; i < rm->num_resources; ++i) {
        if (strcmp(rm->resources[i].name, name) == 0) {
            pthread_mutex_unlock(&rm->lock);
            return i;
        }
    }

    int rid = rm->num_resources++;
    Resource *res = &rm->resources[rid];
    res->rid = rid;
    strncpy(res->name, name, DD_MAX_NAME_LEN - 1);
    res->name[DD_MAX_NAME_LEN - 1] = '\0';
    res->total_instances = total_instances;
    res->available_instances = total_instances;
    res->type = (total_instances == 1) ? RES_SINGLE_INSTANCE : RES_MULTI_INSTANCE;
    res->hierarchy_rank = rid;

    rm->total[rid] = total_instances;
    rm->available[rid] = total_instances;

    pthread_mutex_unlock(&rm->lock);
    return rid;
}

int rm_find_resource(const ResourceManager *rm, const char *name) {
    if (!rm || !name) return -1;
    for (int i = 0; i < rm->num_resources; ++i) {
        if (strcmp(rm->resources[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int rm_get_resource_available(const ResourceManager *rm, int rid) {
    if (!rm || rid < 0 || rid >= rm->num_resources) return -1;
    return rm->available[rid];
}

int rm_register_process(ResourceManager *rm, const char *name, int priority, double cost_weight) {
    if (!rm || !name) return -1;
    pthread_mutex_lock(&rm->lock);

    if (rm->num_processes >= DD_MAX_PROCESSES) {
        pthread_mutex_unlock(&rm->lock);
        return -1;
    }

    for (int i = 0; i < rm->num_processes; ++i) {
        if (strcmp(rm->processes[i].name, name) == 0) {
            pthread_mutex_unlock(&rm->lock);
            return i;
        }
    }

    int pid = rm->num_processes++;
    Process *proc = &rm->processes[pid];
    proc->pid = pid;
    strncpy(proc->name, name, DD_MAX_NAME_LEN - 1);
    proc->name[DD_MAX_NAME_LEN - 1] = '\0';
    proc->state = PROC_READY;
    proc->priority = (priority > 0) ? priority : 1;
    proc->cost_weight = (cost_weight > 0.0) ? cost_weight : 1.0;
    proc->start_time_ms = 0;
    proc->cpu_time_ms = 0;
    proc->last_blocked_res = -1;

    pthread_mutex_unlock(&rm->lock);
    return pid;
}

int rm_find_process(const ResourceManager *rm, const char *name) {
    if (!rm || !name) return -1;
    for (int i = 0; i < rm->num_processes; ++i) {
        if (strcmp(rm->processes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

bool rm_set_process_max(ResourceManager *rm, int pid, const int max_claims[]) {
    if (!rm || !max_claims || pid < 0 || pid >= rm->num_processes) return false;
    pthread_mutex_lock(&rm->lock);

    for (int r = 0; r < rm->num_resources; ++r) {
        if (max_claims[r] < 0 || max_claims[r] > rm->total[r]) {
            pthread_mutex_unlock(&rm->lock);
            return false;
        }
    }

    for (int r = 0; r < rm->num_resources; ++r) {
        rm->max_claim[pid][r] = max_claims[r];
        rm->need[pid][r] = max_claims[r] - rm->allocation[pid][r];
    }

    pthread_mutex_unlock(&rm->lock);
    return true;
}

bool rm_set_process_max_single(ResourceManager *rm, int pid, int rid, int max_claim) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources) return false;
    if (max_claim < 0 || max_claim > rm->total[rid]) return false;

    pthread_mutex_lock(&rm->lock);
    rm->max_claim[pid][rid] = max_claim;
    rm->need[pid][rid] = max_claim - rm->allocation[pid][rid];
    pthread_mutex_unlock(&rm->lock);
    return true;
}

RequestResult rm_request_vector(ResourceManager *rm, int pid, const int req_vector[]) {
    if (!rm || !req_vector || pid < 0 || pid >= rm->num_processes) {
        return REQ_ERROR_INVALID;
    }

    pthread_mutex_lock(&rm->lock);

    Process *proc = &rm->processes[pid];
    if (proc->state == PROC_TERMINATED || proc->state == PROC_ABORTED) {
        pthread_mutex_unlock(&rm->lock);
        return REQ_ERROR_INVALID;
    }

    /* 1. Prevention checks */
    if (rm->mode == MODE_PREVENTION || rm->prevention_strategy != PREVENT_NONE) {
        for (int r = 0; r < rm->num_resources; ++r) {
            if (req_vector[r] > 0) {
                char reason[128];
                if (!prevention_validate_request(rm, pid, r, reason, sizeof(reason))) {
                    if (rm->verbose_logging) {
                        printf("[PREVENTION] Request by %s for %s REJECTED: %s\n",
                               proc->name, rm->resources[r].name, reason);
                    }
                    rm->total_requests_rejected++;
                    pthread_mutex_unlock(&rm->lock);
                    return REQ_REJECTED_PREVENTION_RULE;
                }
            }
        }
    }

    /* 2. Check if request <= Need (if Max claims defined) */
    bool max_defined = false;
    for (int r = 0; r < rm->num_resources; ++r) {
        if (rm->max_claim[pid][r] > 0) max_defined = true;
    }

    if (max_defined) {
        for (int r = 0; r < rm->num_resources; ++r) {
            if (req_vector[r] > rm->need[pid][r]) {
                if (rm->verbose_logging) {
                    printf("[ERROR] %s requested %d of %s, which exceeds its declared Need (%d)\n",
                           proc->name, req_vector[r], rm->resources[r].name, rm->need[pid][r]);
                }
                rm->total_requests_rejected++;
                pthread_mutex_unlock(&rm->lock);
                return REQ_REJECTED_EXCEEDS_MAX;
            }
        }
    }

    /* 3. Check if request <= Available */
    for (int r = 0; r < rm->num_resources; ++r) {
        if (req_vector[r] > rm->available[r]) {
            for (int j = 0; j < rm->num_resources; ++j) {
                rm->request[pid][j] = req_vector[j];
            }
            proc->state = PROC_BLOCKED;
            proc->last_blocked_res = r;
            rm->total_requests_blocked++;
            if (rm->verbose_logging) {
                printf("[BLOCK] %s requested %d units of %s (only %d available) -> BLOCKED\n",
                       proc->name, req_vector[r], rm->resources[r].name, rm->available[r]);
            }
            pthread_mutex_unlock(&rm->lock);
            return REQ_BLOCKED_UNAVAILABLE;
        }
    }

    /* 4. Avoidance Mode: Banker's safety check */
    if (rm->mode == MODE_AVOIDANCE) {
        SafeSequence seq;
        if (!banker_evaluate_request(rm, pid, req_vector, &seq)) {
            for (int j = 0; j < rm->num_resources; ++j) {
                rm->request[pid][j] = req_vector[j];
            }
            proc->state = PROC_BLOCKED;
            rm->total_requests_blocked++;
            if (rm->verbose_logging) {
                printf("[BANKER] Request by %s would lead to UNSAFE state -> BLOCKED (Avoidance)\n",
                       proc->name);
            }
            pthread_mutex_unlock(&rm->lock);
            return REQ_BLOCKED_UNSAFE;
        }
    }

    /* 5. Safe to grant */
    for (int r = 0; r < rm->num_resources; ++r) {
        rm->available[r] -= req_vector[r];
        rm->resources[r].available_instances = rm->available[r];
        rm->allocation[pid][r] += req_vector[r];
        rm->need[pid][r] -= req_vector[r];
        rm->request[pid][r] = 0;
    }
    proc->state = PROC_RUNNING;
    proc->last_blocked_res = -1;
    rm->total_requests_granted++;

    if (rm->verbose_logging) {
        printf("[GRANT] Request granted to %s\n", proc->name);
    }

    pthread_mutex_unlock(&rm->lock);
    return REQ_GRANTED;
}

RequestResult rm_request(ResourceManager *rm, int pid, int rid, int units) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources || units <= 0) {
        return REQ_ERROR_INVALID;
    }
    int req_vector[DD_MAX_RESOURCES] = {0};
    req_vector[rid] = units;
    return rm_request_vector(rm, pid, req_vector);
}

bool rm_release_vector(ResourceManager *rm, int pid, const int rel_vector[]) {
    if (!rm || !rel_vector || pid < 0 || pid >= rm->num_processes) {
        return false;
    }

    pthread_mutex_lock(&rm->lock);

    for (int r = 0; r < rm->num_resources; ++r) {
        if (rel_vector[r] < 0 || rel_vector[r] > rm->allocation[pid][r]) {
            pthread_mutex_unlock(&rm->lock);
            return false;
        }
    }

    for (int r = 0; r < rm->num_resources; ++r) {
        rm->allocation[pid][r] -= rel_vector[r];
        rm->need[pid][r] += rel_vector[r];
        rm->available[r] += rel_vector[r];
        rm->resources[r].available_instances = rm->available[r];
    }

    if (rm->verbose_logging) {
        printf("[RELEASE] Process %s released resources\n", rm->processes[pid].name);
    }

    pthread_mutex_unlock(&rm->lock);

    rm_process_pending_requests(rm);
    return true;
}

bool rm_release(ResourceManager *rm, int pid, int rid, int units) {
    if (!rm || pid < 0 || pid >= rm->num_processes || rid < 0 || rid >= rm->num_resources || units <= 0) {
        return false;
    }
    int rel_vector[DD_MAX_RESOURCES] = {0};
    rel_vector[rid] = units;
    return rm_release_vector(rm, pid, rel_vector);
}

bool rm_release_all(ResourceManager *rm, int pid) {
    if (!rm || pid < 0 || pid >= rm->num_processes) return false;
    int rel_vector[DD_MAX_RESOURCES];
    for (int r = 0; r < rm->num_resources; ++r) {
        rel_vector[r] = rm->allocation[pid][r];
    }
    return rm_release_vector(rm, pid, rel_vector);
}

bool rm_terminate_process(ResourceManager *rm, int pid) {
    if (!rm || pid < 0 || pid >= rm->num_processes) return false;
    pthread_mutex_lock(&rm->lock);

    Process *proc = &rm->processes[pid];
    for (int r = 0; r < rm->num_resources; ++r) {
        rm->available[r] += rm->allocation[pid][r];
        rm->resources[r].available_instances = rm->available[r];
        rm->allocation[pid][r] = 0;
        rm->need[pid][r] = 0;
        rm->request[pid][r] = 0;
    }
    proc->state = PROC_TERMINATED;
    proc->last_blocked_res = -1;

    if (rm->verbose_logging) {
        printf("[TERMINATE] Process %s terminated, all resources released\n", proc->name);
    }

    pthread_mutex_unlock(&rm->lock);

    rm_process_pending_requests(rm);
    return true;
}

int rm_process_pending_requests(ResourceManager *rm) {
    if (!rm) return 0;
    int unblocked_count = 0;
    bool progress = true;

    while (progress) {
        progress = false;
        for (int p = 0; p < rm->num_processes; ++p) {
            if (rm->processes[p].state != PROC_BLOCKED) continue;

            bool has_active_request = false;
            int req_vec[DD_MAX_RESOURCES] = {0};
            for (int r = 0; r < rm->num_resources; ++r) {
                req_vec[r] = rm->request[p][r];
                if (req_vec[r] > 0) has_active_request = true;
            }

            if (!has_active_request) continue;

            bool can_avail = true;
            for (int r = 0; r < rm->num_resources; ++r) {
                if (req_vec[r] > rm->available[r]) {
                    can_avail = false;
                    break;
                }
            }
            if (!can_avail) continue;

            if (rm->mode == MODE_AVOIDANCE) {
                SafeSequence seq;
                if (!banker_evaluate_request(rm, p, req_vec, &seq)) {
                    continue;
                }
            }

            pthread_mutex_lock(&rm->lock);
            for (int r = 0; r < rm->num_resources; ++r) {
                rm->available[r] -= req_vec[r];
                rm->resources[r].available_instances = rm->available[r];
                rm->allocation[p][r] += req_vec[r];
                rm->need[p][r] -= req_vec[r];
                rm->request[p][r] = 0;
            }
            rm->processes[p].state = PROC_RUNNING;
            rm->processes[p].last_blocked_res = -1;
            rm->total_requests_granted++;
            pthread_mutex_unlock(&rm->lock);

            if (rm->verbose_logging) {
                printf("[UNBLOCK] Blocked process %s was granted its pending request\n",
                       rm->processes[p].name);
            }

            unblocked_count++;
            progress = true;
            break;
        }
    }

    return unblocked_count;
}

bool rm_is_safe(const ResourceManager *rm, SafeSequence *out_seq) {
    if (!rm) return false;
    return banker_check_safety(rm, out_seq);
}

bool rm_detect_deadlock(const ResourceManager *rm, DeadlockReport *out_report) {
    if (!rm) return false;
    return detector_detect_deadlock(rm, out_report);
}
