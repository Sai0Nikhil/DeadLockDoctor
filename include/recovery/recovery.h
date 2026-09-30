/**
 * @file recovery.h
 * @brief Deadlock Recovery via Cost-Function-Based Victim Selection & Rollback.
 */

#ifndef DD_RECOVERY_RECOVERY_H
#define DD_RECOVERY_RECOVERY_H

#include "core/resource_mgr.h"
#include "core/types.h"
#include <stdbool.h>

int recovery_select_victim(const ResourceManager *rm, const DeadlockReport *report, VictimPolicy policy);
double recovery_calculate_cost(const ResourceManager *rm, int pid, const DeadlockReport *report, VictimPolicy policy);
bool recovery_recover_system(ResourceManager *rm, const DeadlockReport *report, RecoveryReport *out_rec_report);

#endif /* DD_RECOVERY_RECOVERY_H */
