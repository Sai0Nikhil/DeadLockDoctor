/**
 * @file test_recovery.c
 * @brief Unit tests for cost-based victim selection and system recovery.
 */

#include "recovery/recovery.h"
#include "detection/detector.h"
#include "core/resource_mgr.h"
#include <stdio.h>
#include <assert.h>

#define TEST_ASSERT(expr, desc) do { \
    (*total)++; \
    if (expr) { \
        (*passed)++; \
        printf("  " "\x1b[32m" "[PASS]" "\x1b[0m" " %s\n", desc); \
    } else { \
        printf("  " "\x1b[31m" "[FAIL]" "\x1b[0m" " %s\n", desc); \
    } \
} while(0)

void run_recovery_tests(int *passed, int *total) {
    printf("\n\x1b[1m\x1b[34m[TEST SUITE: Cost-Based Deadlock Recovery]\x1b[0m\n");

    ResourceManager rm;
    rm_init(&rm, MODE_DETECTION);
    rm_register_resource(&rm, "L1", 1);
    rm_register_resource(&rm, "L2", 1);

    int p_expensive = rm_register_process(&rm, "P_Expensive", 10, 100.0);
    int p_cheap = rm_register_process(&rm, "P_Cheap", 1, 1.0);

    // Circular wait deadlock
    rm.allocation[p_expensive][0] = 1;
    rm.allocation[p_cheap][1] = 1;
    rm.request[p_expensive][1] = 1;
    rm.request[p_cheap][0] = 1;
    rm.available[0] = 0;
    rm.available[1] = 0;
    rm.processes[p_expensive].state = PROC_BLOCKED;
    rm.processes[p_cheap].state = PROC_BLOCKED;

    DeadlockReport rep;
    detector_detect_deadlock(&rm, &rep);
    TEST_ASSERT(rep.has_deadlock == true, "Deadlock state established");

    // Test 1: Cheapest victim selection
    int victim = recovery_select_victim(&rm, &rep, RECOVERY_CHEAPEST);
    TEST_ASSERT(victim == p_cheap, "Cheapest victim policy selects P_Cheap (lowest penalty)");

    // Test 2: Full recovery execution
    RecoveryReport rec;
    bool recovered = recovery_recover_system(&rm, &rep, &rec);
    TEST_ASSERT(recovered == true, "Recovery execution successfully resolves deadlock");
    TEST_ASSERT(rec.victim_pid == p_cheap, "Recovery report confirms P_Cheap aborted");
    TEST_ASSERT(rm.processes[p_cheap].state == PROC_ABORTED, "Victim state marked PROC_ABORTED");

    rm_destroy(&rm);
}
