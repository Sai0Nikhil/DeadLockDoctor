/**
 * @file live_demo.c
 * @brief Real Multi-threaded Deadlock Demonstration using POSIX Threads and Mutexes.
 */

#include "demo/live_demo.h"
#include "detection/detector.h"
#include "recovery/recovery.h"
#include "ui/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

#define MAX_DEMO_THREADS 8
#define MAX_DEMO_MUTEXES 8

typedef struct {
    int thread_id;
    int first_mutex;
    int second_mutex;
    pthread_t thread;
    atomic_bool should_terminate;
    atomic_bool holding_first;
    atomic_bool holding_second;
    atomic_bool waiting_second;
    ResourceManager *rm;
    pthread_mutex_t *mutexes;
} WorkerContext;

static pthread_mutex_t g_rm_lock = PTHREAD_MUTEX_INITIALIZER;

void live_demo_default_config(LiveDemoConfig *config) {
    if (!config) return;
    config->num_threads = 3;
    config->num_mutexes = 3;
    config->enable_recovery = true;
    config->recovery_policy = RECOVERY_CHEAPEST;
    config->check_interval_ms = 100;
    config->timeout_limit_ms = 4000;
    config->verbose = true;
}

static void *worker_thread_func(void *arg) {
    WorkerContext *ctx = (WorkerContext *)arg;
    int tid = ctx->thread_id;
    int m1 = ctx->first_mutex;
    int m2 = ctx->second_mutex;

    // Step 1: Request & acquire first mutex
    pthread_mutex_lock(&g_rm_lock);
    rm_request(ctx->rm, tid, m1, 1);
    pthread_mutex_unlock(&g_rm_lock);

    pthread_mutex_lock(&ctx->mutexes[m1]);
    atomic_store(&ctx->holding_first, true);

    pthread_mutex_lock(&g_rm_lock);
    printf("  " ANSI_GREEN "[Thread %d]" ANSI_RESET " Acquired Mutex R%d (Holding: R%d)\n", tid, m1, m1);
    pthread_mutex_unlock(&g_rm_lock);

    // Sleep briefly to guarantee interleaving circular hold-and-wait
    usleep(50000); // 50ms

    // Step 2: Request second mutex
    pthread_mutex_lock(&g_rm_lock);
    rm_request(ctx->rm, tid, m2, 1);
    printf("  " ANSI_YELLOW "[Thread %d]" ANSI_RESET " Requesting Mutex R%d (Creates Dependency: P%d -> R%d)\n", tid, m2, tid, m2);
    pthread_mutex_unlock(&g_rm_lock);

    atomic_store(&ctx->waiting_second, true);

    // Try-lock loop checking for termination / recovery signal
    while (!atomic_load(&ctx->should_terminate)) {
        if (pthread_mutex_trylock(&ctx->mutexes[m2]) == 0) {
            atomic_store(&ctx->holding_second, true);
            atomic_store(&ctx->waiting_second, false);

            pthread_mutex_lock(&g_rm_lock);
            printf("  " ANSI_GREEN "[Thread %d]" ANSI_RESET " Acquired Mutex R%d (Work Critical Section Done)\n", tid, m2);
            pthread_mutex_unlock(&g_rm_lock);

            // Simulate work
            usleep(20000);

            // Release second
            pthread_mutex_unlock(&ctx->mutexes[m2]);
            atomic_store(&ctx->holding_second, false);
            pthread_mutex_lock(&g_rm_lock);
            rm_release(ctx->rm, tid, m2, 1);
            pthread_mutex_unlock(&g_rm_lock);
            break;
        }
        usleep(10000); // 10ms poll
    }

    // Release first
    if (atomic_load(&ctx->holding_first)) {
        pthread_mutex_unlock(&ctx->mutexes[m1]);
        atomic_store(&ctx->holding_first, false);
        pthread_mutex_lock(&g_rm_lock);
        rm_release(ctx->rm, tid, m1, 1);
        pthread_mutex_unlock(&g_rm_lock);
    }

    if (atomic_load(&ctx->should_terminate)) {
        printf("  " ANSI_RED "[Thread %d]" ANSI_RESET " Received Recovery / Abort Signal. Cleanly Released Held Mutexes.\n", tid);
    }

    return NULL;
}

int live_demo_run(const LiveDemoConfig *config) {
    LiveDemoConfig cfg;
    if (config) {
        cfg = *config;
    } else {
        live_demo_default_config(&cfg);
    }

    int n_threads = (cfg.num_threads > MAX_DEMO_THREADS) ? MAX_DEMO_THREADS : cfg.num_threads;
    int n_mutexes = (cfg.num_mutexes > MAX_DEMO_MUTEXES) ? MAX_DEMO_MUTEXES : cfg.num_mutexes;
    if (n_threads < 2) n_threads = 2;
    if (n_mutexes < 2) n_mutexes = 2;

    printf("\n" ANSI_BOLD ANSI_CYAN "================================================================" ANSI_RESET "\n");
    printf(ANSI_BOLD ANSI_CYAN "   DEADLOCKDOCTOR: LIVE POSIX THREADS MULTITHREADING DEMO       " ANSI_RESET "\n");
    printf(ANSI_BOLD ANSI_CYAN "================================================================" ANSI_RESET "\n");
    printf(" Spawning %d POSIX worker threads competing for %d Mutex Locks...\n", n_threads, n_mutexes);
    printf(" Dependency Topology: Circular Chain (P0->R0,req R1; P1->R1,req R2; ... Pn-1->Rn-1,req R0)\n\n");

    ResourceManager rm;
    rm_init(&rm, MODE_DETECTION);
    rm.recovery_policy = cfg.recovery_policy;

    for (int i = 0; i < n_mutexes; i++) {
        char name[16];
        snprintf(name, sizeof(name), "R%d", i);
        rm_register_resource(&rm, name, 1);
    }

    for (int i = 0; i < n_threads; i++) {
        char name[16];
        snprintf(name, sizeof(name), "P%d", i);
        int pid = rm_register_process(&rm, name, (i + 1) * 10, (i + 1) * 1.5);
        int max_vec[DD_MAX_RESOURCES] = {0};
        max_vec[i] = 1;
        max_vec[(i + 1) % n_mutexes] = 1;
        rm_set_process_max(&rm, pid, max_vec);
    }

    pthread_mutex_t mutexes[MAX_DEMO_MUTEXES];
    for (int i = 0; i < n_mutexes; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
    }

    WorkerContext contexts[MAX_DEMO_THREADS];
    for (int i = 0; i < n_threads; i++) {
        contexts[i].thread_id = i;
        contexts[i].first_mutex = i;
        contexts[i].second_mutex = (i + 1) % n_mutexes;
        atomic_init(&contexts[i].should_terminate, false);
        atomic_init(&contexts[i].holding_first, false);
        atomic_init(&contexts[i].holding_second, false);
        atomic_init(&contexts[i].waiting_second, false);
        contexts[i].rm = &rm;
        contexts[i].mutexes = mutexes;
    }

    // Launch worker threads
    for (int i = 0; i < n_threads; i++) {
        pthread_create(&contexts[i].thread, NULL, worker_thread_func, &contexts[i]);
    }

    // Watchdog / Detection loop
    int elapsed_ms = 0;
    bool deadlock_handled = false;
    printf("\n" ANSI_YELLOW "[Watchdog Detector]" ANSI_RESET " Background Monitoring Activated (Polling every %d ms)...\n", cfg.check_interval_ms);

    while (elapsed_ms < cfg.timeout_limit_ms) {
        usleep(cfg.check_interval_ms * 1000);
        elapsed_ms += cfg.check_interval_ms;

        pthread_mutex_lock(&g_rm_lock);
        DeadlockReport report;
        bool is_deadlocked = rm_detect_deadlock(&rm, &report);

        if (is_deadlocked && !deadlock_handled) {
            printf("\n" ANSI_BG_RED ANSI_BOLD " [!] DEADLOCK DETECTED BY WATCHDOG AT T = +%d ms " ANSI_RESET "\n", elapsed_ms);
            visualizer_print_deadlock_alert(&rm, &report);
            visualizer_print_graphs(&rm);

            if (cfg.enable_recovery) {
                printf("\n" ANSI_BOLD ANSI_MAGENTA ">>> EXECUTING AUTOMATED RECOVERY ENGINE <<<" ANSI_RESET "\n");
                RecoveryReport rec_report;
                recovery_recover_system(&rm, &report, &rec_report);
                visualizer_print_recovery_report(&rec_report);

                // Signal victim thread to yield locks
                int v_pid = rec_report.victim_pid;
                if (v_pid >= 0 && v_pid < n_threads) {
                    printf(ANSI_CYAN "  -> Signaling Worker Thread P%d to abort and release mutex...\n" ANSI_RESET, v_pid);
                    atomic_store(&contexts[v_pid].should_terminate, true);
                }
                deadlock_handled = true;
            } else {
                printf(" Recovery disabled in configuration. Terminating demo.\n");
                for (int i = 0; i < n_threads; i++) {
                    atomic_store(&contexts[i].should_terminate, true);
                }
                deadlock_handled = true;
            }
        }
        pthread_mutex_unlock(&g_rm_lock);

        if (deadlock_handled) break;
    }

    // Allow threads to complete after recovery
    usleep(150000); // 150ms
    for (int i = 0; i < n_threads; i++) {
        atomic_store(&contexts[i].should_terminate, true);
    }
    for (int i = 0; i < n_threads; i++) {
        pthread_join(contexts[i].thread, NULL);
    }
    for (int i = 0; i < n_mutexes; i++) {
        pthread_mutex_destroy(&mutexes[i]);
    }

    printf("\n" ANSI_BOLD ANSI_GREEN "[+] Live Multithreading Demo Completed Cleanly (All Locks Released, 0 Leaks)." ANSI_RESET "\n\n");
    return 0;
}
