/**
 * @file scenario.h
 * @brief Scenario Parser, Event Representation, and Simulation Runner.
 */

#ifndef DD_SCENARIO_SCENARIO_H
#define DD_SCENARIO_SCENARIO_H

#include "core/resource_mgr.h"
#include <stdbool.h>

#define MAX_SCENARIO_EVENTS 256

typedef enum {
    EVENT_DEF_RESOURCE = 1,
    EVENT_DEF_PROCESS,
    EVENT_CLAIM,
    EVENT_REQUEST,
    EVENT_RELEASE,
    EVENT_TERMINATE,
    EVENT_CHECK_DEADLOCK,
    EVENT_CHECK_SAFETY,
    EVENT_RECOVER,
    EVENT_SET_MODE,
    EVENT_SET_PREVENTION,
    EVENT_SET_VICTIM_POLICY,
    EVENT_PRINT_STATE,
    EVENT_SLEEP_MS
} EventType;

typedef struct {
    EventType type;
    int step_num;
    char target_name[DD_MAX_NAME_LEN];
    char resource_name[DD_MAX_NAME_LEN];
    int units;
    int priority;
    double cost_weight;
    int vector_data[DD_MAX_RESOURCES];
    int vector_count;
    char extra_arg[DD_MAX_NAME_LEN];
} ScenarioEvent;

typedef struct {
    char name[DD_MAX_NAME_LEN];
    int num_events;
    ScenarioEvent events[MAX_SCENARIO_EVENTS];
} Scenario;

bool scenario_parse_file(const char *filename, Scenario *out_scenario);
bool scenario_run(const Scenario *scenario, ResourceManager *rm, bool interactive, bool visualize);

#endif /* DD_SCENARIO_SCENARIO_H */
