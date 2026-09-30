/**
 * @file recovery.c
 * @brief Deadlock Recovery via Cost-Function-Based Victim Selection & Rollback.
 */

#include "recovery/recovery.h"
#include "detection/detector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

static int get_total_held_resources(const ResourceManager *rm, int pid) {
    int sum = 0;
    for (int r = 0; r < rm->num_resources; ++r) {
        sum += rm->allocation[pid][r];
    }
    return sum;
}

static int get_cycles_count_for_process(const DeadlockReport *report, int pid) {
    int count = 0;
    for (int c = 0; c < report->num_cycles; ++c) {
        for (int i = 0; i < report->cycles[c].length; ++i) {
            if (report->cycles[c].pids[i] == pid) {
                count++;
                break;
            }
        }
    }
    return count;
}

double recovery_calculate_cost(const ResourceManager *rm, int pid, const DeadlockReport *report, VictimPolicy policy) {
    if (!rm || pid < 0 || pid >= rm->num_processes) return DBL_MAX;

    const Process *proc = &rm->processes[pid];
    int held = get_total_held_resources(rm, pid);
    int cycles_broken = get_cycles_count_for_process(report, pid);
    int prio = (proc->priority > 0) ? proc->priority : 1;

    switch (policy) {
        case RECOVERY_FEWEST_RESOURCES:
            return (double)held;

        case RECOVERY_MOST_RESOURCES:
            return -(double)held;

        case RECOVERY_LOWEST_PRIORITY:
            return (double)prio;

        case RECOVERY_MAX_CYCLES_BROKEN:
            return -(double)cycles_broken;

        case RECOVERY_CHEAPEST:
        default: {
            double priority_cost = (double)prio * 100.0;
            double held_cost = (double)held * 10.0;
            double cycle_benefit = (double)cycles_broken * 50.0;
            double cpu_cost = (double)proc->cpu_time_ms * 0.1;

            double base_cost = priority_cost + held_cost + cpu_cost - cycle_benefit;
            if (base_cost < 1.0) base_cost = 1.0;
            return base_cost * proc->cost_weight;
        }
    }
}

int recovery_select_victim(const ResourceManager *rm, const DeadlockReport *report, VictimPolicy policy) {
    if (!rm || !report || !report->has_deadlock || report->num_deadlocked_processes == 0) {
        return -1;
    }

    int best_victim = -1;
    double min_cost = DBL_MAX;

    for (int i = 0; i < report->num_deadlocked_processes; ++i) {
        int pid = report->deadlocked_pids[i];
        double cost = recovery_calculate_cost(rm, pid, report, policy);

        if (cost < min_cost) {
            min_cost = cost;
            best_victim = pid;
        }
    }

    return best_victim;
}

bool recovery_recover_system(ResourceManager *rm, const DeadlockReport *report, RecoveryReport *out_rec_report) {
    if (!rm || !report || !report->has_deadlock) return false;

    if (out_rec_report) {
        memset(out_rec_report, 0, sizeof(RecoveryReport));
    }

    int victim_pid = recovery_select_victim(rm, report, rm->recovery_policy);
    if (victim_pid < 0) return false;

    Process *victim = &rm->processes[victim_pid];

    if (out_rec_report) {
        out_rec_report->victim_pid = victim_pid;
        strncpy(out_rec_report->victim_name, victim->name, DD_MAX_NAME_LEN - 1);
        out_rec_report->calculated_cost = recovery_calculate_cost(rm, victim_pid, report, rm->recovery_policy);
    }

    int total_freed = 0;
    for (int r = 0; r < rm->num_resources; ++r) {
        int units = rm->allocation[victim_pid][r];
        if (out_rec_report) {
            out_rec_report->resources_freed[r] = units;
        }
        total_freed += units;
    }
    if (out_rec_report) {
        out_rec_report->total_units_freed = total_freed;
    }

    pthread_mutex_lock(&rm->lock);

    for (int r = 0; r < rm->num_resources; ++r) {
        rm->available[r] += rm->allocation[victim_pid][r];
        rm->resources[r].available_instances = rm->available[r];
        rm->allocation[victim_pid][r] = 0;
        rm->need[victim_pid][r] = 0;
        rm->request[victim_pid][r] = 0;
    }
    victim->state = PROC_ABORTED;
    victim->last_blocked_res = -1;
    rm->total_recoveries_performed++;

    if (rm->verbose_logging) {
        printf("[RECOVERY] Aborted victim process %s (PID %d), freed %d resource units\n",
               victim->name, victim_pid, total_freed);
    }

    pthread_mutex_unlock(&rm->lock);

    rm_process_pending_requests(rm);

    DeadlockReport post_report;
    bool still_deadlocked = detector_detect_deadlock(rm, &post_report);

    if (out_rec_report) {
        out_rec_report->recovery_successful = !still_deadlocked;
    }

    return !still_deadlocked;
}
