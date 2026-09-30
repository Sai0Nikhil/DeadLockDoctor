/**
 * @file test_main.c
 * @brief Comprehensive Unit and Integration Test Runner for DeadlockDoctor.
 */

#include "deadlockdoctor.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern void run_banker_tests(int *passed, int *total);
extern void run_detection_tests(int *passed, int *total);
extern void run_recovery_tests(int *passed, int *total);
extern void run_prevention_tests(int *passed, int *total);
extern void run_scenario_tests(int *passed, int *total);

int main(void) {
    printf(ANSI_BOLD ANSI_CYAN "\n============================================================\n");
    printf("   DEADLOCKDOCTOR C11 UNIT & INTEGRATION TEST SUITE        \n");
    printf("============================================================\n" ANSI_RESET);

    int total_passed = 0;
    int total_tests = 0;

    int p = 0, t = 0;
    run_banker_tests(&p, &t);
    total_passed += p; total_tests += t;

    p = 0; t = 0;
    run_detection_tests(&p, &t);
    total_passed += p; total_tests += t;

    p = 0; t = 0;
    run_recovery_tests(&p, &t);
    total_passed += p; total_tests += t;

    p = 0; t = 0;
    run_prevention_tests(&p, &t);
    total_passed += p; total_tests += t;

    p = 0; t = 0;
    run_scenario_tests(&p, &t);
    total_passed += p; total_tests += t;

    printf("\n" ANSI_BOLD ANSI_CYAN "============================================================\n" ANSI_RESET);
    if (total_passed == total_tests) {
        printf(ANSI_BOLD ANSI_GREEN " [✓] ALL %d / %d TESTS PASSED SUCCESSFULLY!\n" ANSI_RESET, total_passed, total_tests);
    } else {
        printf(ANSI_BOLD ANSI_RED " [X] %d / %d TESTS FAILED!\n" ANSI_RESET, total_tests - total_passed, total_tests);
    }
    printf(ANSI_BOLD ANSI_CYAN "============================================================\n\n" ANSI_RESET);

    return (total_passed == total_tests) ? 0 : 1;
}
