/**
 * @file test_scenario.c
 * @brief Unit tests for Scenario parser and simulation runner.
 */

#include "scenario/scenario.h"
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

void run_scenario_tests(int *passed, int *total) {
    printf("\n\x1b[1m\x1b[34m[TEST SUITE: Scenario Parser & Runner]\x1b[0m\n");

    Scenario sc;
    bool parsed = scenario_parse_file("scenarios/01_bankers_safe.dd", &sc);
    TEST_ASSERT(parsed == true, "Successfully parsed 01_bankers_safe.dd");
    TEST_ASSERT(sc.num_events > 0, "Scenario contains parsed event sequence");

    ResourceManager rm;
    rm_init(&rm, MODE_AVOIDANCE);
    bool run_ok = scenario_run(&sc, &rm, false, false);
    TEST_ASSERT(run_ok == true, "Scenario simulation executes through completion");
    TEST_ASSERT(rm.num_processes == 5, "5 processes registered during scenario replay");
    TEST_ASSERT(rm.num_resources == 3, "3 resources registered during scenario replay");

    rm_destroy(&rm);
}
