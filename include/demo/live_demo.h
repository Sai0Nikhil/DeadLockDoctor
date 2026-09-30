/**
 * @file live_demo.h
 * @brief Real Multi-threaded Deadlock Demonstration using POSIX Threads and Mutexes.
 */

#ifndef DD_DEMO_LIVE_DEMO_H
#define DD_DEMO_LIVE_DEMO_H

#include "core/resource_mgr.h"
#include <stdbool.h>

typedef struct {
    int num_threads;
    int num_mutexes;
    bool enable_recovery;
    VictimPolicy recovery_policy;
    int check_interval_ms;
    int timeout_limit_ms;
    bool verbose;
} LiveDemoConfig;

void live_demo_default_config(LiveDemoConfig *config);
int live_demo_run(const LiveDemoConfig *config);

#endif /* DD_DEMO_LIVE_DEMO_H */
