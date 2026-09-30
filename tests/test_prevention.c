/**
 * @file test_prevention.c
 * @brief Unit tests for Havender's Resource Ordering & Hold-and-Wait prevention rules.
 */

#include "prevention/prevention.h"
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

void run_prevention_tests(int *passed, int *total) {
    printf("\n\x1b[1m\x1b[34m[TEST SUITE: Deadlock Prevention Rules]\x1b[0m\n");

    ResourceManager rm;
    rm_init(&rm, MODE_PREVENTION);
    rm_set_prevention_strategy(&rm, PREVENT_RESOURCE_ORDERING);

    int r0 = rm_register_resource(&rm, "TapeDrive", 1);
    int r1 = rm_register_resource(&rm, "DiskDrive", 1);
    int r2 = rm_register_resource(&rm, "Printer", 1);

    rm.resources[r0].hierarchy_rank = 0;
    rm.resources[r1].hierarchy_rank = 1;
    rm.resources[r2].hierarchy_rank = 2;

    int p0 = rm_register_process(&rm, "JobA", 1, 1.0);

    // Test 1: Requesting higher ranked resource when holding lower rank -> Allowed
    rm.allocation[p0][r0] = 1; // Holding Rank 0
    bool higher_ok = prevention_check_resource_ordering(&rm, p0, r1); // Requesting Rank 1
    TEST_ASSERT(higher_ok == true, "Resource ordering permits requesting higher rank (1 > 0)");

    // Test 2: Requesting lower/equal ranked resource when holding higher rank -> Denied
    rm.allocation[p0][r2] = 1; // Now also holding Rank 2
    bool lower_ok = prevention_check_resource_ordering(&rm, p0, r1); // Requesting Rank 1 < Rank 2
    TEST_ASSERT(lower_ok == false, "Resource ordering denies requesting lower rank (1 <= 2)");

    // Test 3: All-or-nothing (Hold-and-Wait prevention)
    rm_set_prevention_strategy(&rm, PREVENT_ALL_OR_NOTHING);
    bool hold_wait_ok = prevention_check_all_or_nothing(&rm, p0, r1);
    TEST_ASSERT(hold_wait_ok == false, "All-or-Nothing denies request while holding existing resources");

    rm_destroy(&rm);
}
