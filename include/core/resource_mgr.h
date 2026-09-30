/**
 * @file resource_mgr.h
 * @brief Resource Manager API and State definition for DeadlockDoctor.
 */

#ifndef DD_CORE_RESOURCE_MGR_H
#define DD_CORE_RESOURCE_MGR_H

#include "core/types.h"
#include <pthread.h>

typedef struct ResourceManager {
    pthread_mutex_t lock;

    OperatingMode mode;
    PreventionStrategy prevention_strategy;
    VictimPolicy recovery_policy;

    int num_processes;
    int num_resources;

    Process processes[DD_MAX_PROCESSES];
    Resource resources[DD_MAX_RESOURCES];

    /* Allocation Matrix: Allocation[P][R] = units of R held by P */
    int allocation[DD_MAX_PROCESSES][DD_MAX_RESOURCES];

    /* Max Claim Matrix: Max[P][R] = maximum units of R that P may request */
    int max_claim[DD_MAX_PROCESSES][DD_MAX_RESOURCES];

    /* Need Matrix: Need[P][R] = Max[P][R] - Allocation[P][R] */
    int need[DD_MAX_PROCESSES][DD_MAX_RESOURCES];

    /* Request / Wait Matrix: Request[P][R] = current outstanding request */
    int request[DD_MAX_PROCESSES][DD_MAX_RESOURCES];

    /* Available Vector: Available[R] = currently free units of R */
    int available[DD_MAX_RESOURCES];

    /* Total Vector: Total[R] = total system capacity of R */
    int total[DD_MAX_RESOURCES];

    /* Performance & Event counters */
    int total_requests_granted;
    int total_requests_blocked;
    int total_requests_rejected;
    int total_deadlocks_detected;
    int total_recoveries_performed;

    bool verbose_logging;
} ResourceManager;

/* Initialization & Cleanup */
void rm_init(ResourceManager *rm, OperatingMode mode);
void rm_destroy(ResourceManager *rm);
void rm_reset(ResourceManager *rm);

/* Configuration */
void rm_set_mode(ResourceManager *rm, OperatingMode mode);
void rm_set_prevention_strategy(ResourceManager *rm, PreventionStrategy strategy);
void rm_set_victim_policy(ResourceManager *rm, VictimPolicy policy);
void rm_set_verbose(ResourceManager *rm, bool verbose);

/* Resource Registration & Query */
int rm_register_resource(ResourceManager *rm, const char *name, int total_instances);
int rm_find_resource(const ResourceManager *rm, const char *name);
int rm_get_resource_available(const ResourceManager *rm, int rid);

/* Process Registration & Query */
int rm_register_process(ResourceManager *rm, const char *name, int priority, double cost_weight);
int rm_find_process(const ResourceManager *rm, const char *name);
bool rm_set_process_max(ResourceManager *rm, int pid, const int max_claims[]);
bool rm_set_process_max_single(ResourceManager *rm, int pid, int rid, int max_claim);

/* Core Operations */
typedef enum {
    REQ_GRANTED = 0,
    REQ_BLOCKED_UNAVAILABLE,
    REQ_BLOCKED_UNSAFE,
    REQ_REJECTED_EXCEEDS_MAX,
    REQ_REJECTED_PREVENTION_RULE,
    REQ_ERROR_INVALID
} RequestResult;

RequestResult rm_request(ResourceManager *rm, int pid, int rid, int units);
RequestResult rm_request_vector(ResourceManager *rm, int pid, const int req_vector[]);

bool rm_release(ResourceManager *rm, int pid, int rid, int units);
bool rm_release_vector(ResourceManager *rm, int pid, const int rel_vector[]);
bool rm_release_all(ResourceManager *rm, int pid);

bool rm_terminate_process(ResourceManager *rm, int pid);

/* Try to wake up and grant blocked requests after a release */
int rm_process_pending_requests(ResourceManager *rm);

/* Safety & Detection Wrappers */
bool rm_is_safe(const ResourceManager *rm, SafeSequence *out_seq);
bool rm_detect_deadlock(const ResourceManager *rm, DeadlockReport *out_report);

#endif /* DD_CORE_RESOURCE_MGR_H */
