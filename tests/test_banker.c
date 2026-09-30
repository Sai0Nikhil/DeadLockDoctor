/**
 * @file test_banker.c
 * @brief Unit tests for Banker's Algorithm avoidance engine.
 */

#include "avoidance/banker.h"
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

void run_banker_tests(int *passed, int *total) {
    printf("\n\x1b[1m\x1b[34m[TEST SUITE: Banker's Algorithm]\x1b[0m\n");

    ResourceManager rm;
    rm_init(&rm, MODE_AVOIDANCE);

    // Setup 3 resources: A(10), B(5), C(7)
    rm_register_resource(&rm, "A", 10);
    rm_register_resource(&rm, "B", 5);
    rm_register_resource(&rm, "C", 7);

    // Setup 5 processes: P0..P4
    for (int i = 0; i < 5; i++) {
        char name[8];
        snprintf(name, sizeof(name), "P%d", i);
        rm_register_process(&rm, name, i + 1, 1.0);
    }

    int max_claims[5][3] = {
        {7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}
    };
    int allocations[5][3] = {
        {0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}
    };

    for (int i = 0; i < 5; i++) {
        rm_set_process_max(&rm, i, max_claims[i]);
        rm_request_vector(&rm, i, allocations[i]);
    }

    // Test 1: Initial state must be safe
    SafeSequence seq;
    bool is_safe = banker_check_safety(&rm, &seq);
    TEST_ASSERT(is_safe == true, "Silberschatz initial state is recognized as SAFE");
    TEST_ASSERT(seq.count == 5, "Safe sequence contains all 5 processes");

    // Test 2: Safe request evaluation: P1 requests (1, 0, 2)
    int valid_req[3] = {1, 0, 2};
    SafeSequence req_seq;
    bool req_ok = banker_evaluate_request(&rm, 1, valid_req, &req_seq);
    TEST_ASSERT(req_ok == true, "P1 request (1,0,2) is evaluated as SAFE");

    // Grant P1 request so available becomes (2, 3, 0)
    rm_request_vector(&rm, 1, valid_req);

    // Test 3: Unsafe request evaluation: P0 requests (0, 2, 0) when available is (2, 3, 0)
    int unsafe_req[3] = {0, 2, 0};
    bool unsafe_ok = banker_evaluate_request(&rm, 0, unsafe_req, &req_seq);
    TEST_ASSERT(unsafe_ok == false, "P0 request (0,2,0) after P1 allocation is rejected as UNSAFE");

    // Test 4: Request exceeding maximum claim must fail
    int over_req[3] = {10, 10, 10};
    bool over_ok = banker_evaluate_request(&rm, 0, over_req, &req_seq);
    TEST_ASSERT(over_ok == false, "Request exceeding claim is rejected");

    rm_destroy(&rm);
}
