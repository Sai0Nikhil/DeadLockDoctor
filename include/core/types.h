/**
 * @file types.h
 * @brief Core types, enums, and data definitions for DeadlockDoctor.
 */

#ifndef DD_CORE_TYPES_H
#define DD_CORE_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define DD_MAX_PROCESSES 64
#define DD_MAX_RESOURCES 32
#define DD_MAX_NAME_LEN 32
#define DD_MAX_CYCLES 32
#define DD_MAX_PATH_LEN 128

typedef enum {
    PROC_UNUSED = 0,
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED,
    PROC_TERMINATED,
    PROC_ABORTED,
    PROC_ROLLEDBACK
} ProcessState;

typedef enum {
    RES_SINGLE_INSTANCE = 1,
    RES_MULTI_INSTANCE
} ResourceType;

typedef enum {
    MODE_AVOIDANCE = 0,  /* Banker's safety check on every request */
    MODE_DETECTION,      /* Grant immediately if available, run periodic/on-demand detection */
    MODE_PREVENTION      /* Enforce strict resource ordering or all-or-nothing */
} OperatingMode;

typedef enum {
    PREVENT_NONE = 0,
    PREVENT_RESOURCE_ORDERING,  /* Havender's hierarchical resource acquisition */
    PREVENT_ALL_OR_NOTHING,     /* Hold-and-wait prevention: request all at start */
    PREVENT_NO_PREEMPTION       /* Preempt resources if request cannot be fully satisfied */
} PreventionStrategy;

typedef enum {
    RECOVERY_CHEAPEST = 0,
    RECOVERY_FEWEST_RESOURCES,
    RECOVERY_MOST_RESOURCES,
    RECOVERY_LOWEST_PRIORITY,
    RECOVERY_MAX_CYCLES_BROKEN
} VictimPolicy;

typedef struct {
    int pid;
    char name[DD_MAX_NAME_LEN];
    ProcessState state;
    int priority;           /* Higher number = higher priority */
    double cost_weight;     /* Custom cost multiplier */
    uint64_t start_time_ms;
    uint64_t cpu_time_ms;
    int last_blocked_res;   /* Resource ID this process is waiting on (-1 if none) */
} Process;

typedef struct {
    int rid;
    char name[DD_MAX_NAME_LEN];
    ResourceType type;
    int total_instances;
    int available_instances;
    int hierarchy_rank;     /* For resource ordering prevention (0 < 1 < 2 ...) */
} Resource;

typedef struct {
    int count;
    int sequence[DD_MAX_PROCESSES];
    bool is_safe;
} SafeSequence;

typedef struct {
    int length;
    int pids[DD_MAX_PROCESSES];
} DeadlockCycle;

typedef struct {
    bool has_deadlock;
    int num_deadlocked_processes;
    int deadlocked_pids[DD_MAX_PROCESSES];
    int num_cycles;
    DeadlockCycle cycles[DD_MAX_CYCLES];
} DeadlockReport;

typedef struct {
    int victim_pid;
    char victim_name[DD_MAX_NAME_LEN];
    double calculated_cost;
    int resources_freed[DD_MAX_RESOURCES];
    int total_units_freed;
    bool recovery_successful;
} RecoveryReport;

#endif /* DD_CORE_TYPES_H */
