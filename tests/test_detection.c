/**
 * @file test_detection.c
 * @brief Unit tests for 3-Color DFS cycle detection and Multi-instance reduction.
 */

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

void run_detection_tests(int *passed, int *total) {
    printf("\n\x1b[1m\x1b[34m[TEST SUITE: Deadlock Detection & Graph Cycles]\x1b[0m\n");

    // Test 1: Single-instance cycle detection in WFG
    WaitForGraph wfg;
    wfg_init(&wfg);
    wfg.num_nodes = 3;
    wfg_add_edge(&wfg, 0, 1, 0); // P0 waits for P1
    wfg_add_edge(&wfg, 1, 2, 1); // P1 waits for P2
    wfg_add_edge(&wfg, 2, 0, 2); // P2 waits for P0 (Cycle: 0->1->2->0)

    DeadlockReport report;
    bool has_cycle = wfg_find_cycles(&wfg, &report);
    TEST_ASSERT(has_cycle == true, "3-Color DFS detects 3-node cycle");
    TEST_ASSERT(report.num_deadlocked_processes == 3, "All 3 processes identified as deadlocked");
    TEST_ASSERT(report.num_cycles >= 1, "Cycle recorded in report");

    // Test 2: DAG with no cycle
    WaitForGraph dag;
    wfg_init(&dag);
    dag.num_nodes = 3;
    wfg_add_edge(&dag, 0, 1, 0);
    wfg_add_edge(&dag, 1, 2, 1);

    DeadlockReport no_cycle_rep;
    bool has_dag_cycle = wfg_find_cycles(&dag, &no_cycle_rep);
    TEST_ASSERT(has_dag_cycle == false, "3-Color DFS returns false for acyclic DAG");

    // Test 3: Multi-instance detection matrix algorithm
    ResourceManager rm;
    rm_init(&rm, MODE_DETECTION);
    rm_register_resource(&rm, "R0", 2);
    rm_register_resource(&rm, "R1", 1);

    int p0 = rm_register_process(&rm, "P0", 1, 1.0);
    int p1 = rm_register_process(&rm, "P1", 2, 1.0);

    // P0 holds 1 of R0, requests 1 of R1
    // P1 holds 1 of R1, requests 2 of R0
    // Total R0=2, R1=1 -> Free R0=1, Free R1=0
    rm.allocation[p0][0] = 1;
    rm.allocation[p1][1] = 1;
    rm.request[p0][1] = 1;
    rm.request[p1][0] = 2; // P1 needs 2, but only 1 free -> neither can finish
    rm.available[0] = 1;
    rm.available[1] = 0;
    rm.processes[p0].state = PROC_BLOCKED;
    rm.processes[p1].state = PROC_BLOCKED;

    DeadlockReport multi_rep;
    bool multi_dead = detector_detect_multi_instance(&rm, &multi_rep);
    TEST_ASSERT(multi_dead == true, "Multi-instance matrix algorithm detects deadlock");
    TEST_ASSERT(multi_rep.num_deadlocked_processes == 2, "P0 and P1 flagged in multi-instance deadlock");

    rm_destroy(&rm);
}
