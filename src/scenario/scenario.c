/**
 * @file scenario.c
 * @brief Scenario Parser and Replay Simulation Runner.
 */

#include "scenario/scenario.h"
#include "avoidance/banker.h"
#include "detection/detector.h"
#include "recovery/recovery.h"
#include "ui/visualizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

static void trim(char *s) {
    char *p = s;
    while (isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
}

bool scenario_parse_file(const char *filename, Scenario *out_scenario) {
    if (!filename || !out_scenario) return false;

    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "[ERROR] Failed to open scenario file: %s\n", filename);
        return false;
    }

    memset(out_scenario, 0, sizeof(Scenario));
    strncpy(out_scenario->name, filename, DD_MAX_NAME_LEN - 1);

    char line[512];
    int line_num = 0;

    while (fgets(line, sizeof(line), f)) {
        line_num++;
        trim(line);
        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') continue;
        if (strncmp(line, "//", 2) == 0) continue;

        if (out_scenario->num_events >= MAX_SCENARIO_EVENTS) {
            fprintf(stderr, "[WARNING] Reached maximum event limit (%d)\n", MAX_SCENARIO_EVENTS);
            break;
        }

        ScenarioEvent *ev = &out_scenario->events[out_scenario->num_events];
        ev->step_num = out_scenario->num_events + 1;

        char token[64];
        if (sscanf(line, "%63s", token) != 1) continue;

        if (strcasecmp(token, "MODE") == 0) {
            ev->type = EVENT_SET_MODE;
            sscanf(line, "%*s %31s", ev->extra_arg);
            out_scenario->num_events++;
        } else if (strcasecmp(token, "PREVENTION") == 0) {
            ev->type = EVENT_SET_PREVENTION;
            sscanf(line, "%*s %31s", ev->extra_arg);
            out_scenario->num_events++;
        } else if (strcasecmp(token, "VICTIM_POLICY") == 0) {
            ev->type = EVENT_SET_VICTIM_POLICY;
            sscanf(line, "%*s %31s", ev->extra_arg);
            out_scenario->num_events++;
        } else if (strcasecmp(token, "RESOURCE") == 0) {
            ev->type = EVENT_DEF_RESOURCE;
            int total = 1;
            int rank = -1;
            sscanf(line, "%*s %31s %d %d", ev->resource_name, &total, &rank);
            ev->units = (total > 0) ? total : 1;
            ev->priority = (rank >= 0) ? rank : 0;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "PROCESS") == 0) {
            ev->type = EVENT_DEF_PROCESS;
            int prio = 1;
            double cost = 1.0;
            sscanf(line, "%*s %31s %d %lf", ev->target_name, &prio, &cost);
            ev->priority = prio;
            ev->cost_weight = cost;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "CLAIM") == 0) {
            ev->type = EVENT_CLAIM;
            char *ptr = line;
            char dummy[64];
            sscanf(ptr, "%63s %31s", dummy, ev->target_name);
            ptr = strstr(ptr, ev->target_name);
            if (ptr) ptr += strlen(ev->target_name);

            ev->vector_count = 0;
            while (ptr && *ptr != '\0') {
                while (isspace((unsigned char)*ptr)) ptr++;
                if (*ptr == '\0') break;

                int val = 0;
                char res_tag[64];
                if (isalpha((unsigned char)*ptr)) {
                    if (sscanf(ptr, "%63s %d", res_tag, &val) == 2) {
                        ev->vector_data[ev->vector_count++] = val;
                        ptr = strstr(ptr, res_tag);
                        if (ptr) ptr += strlen(res_tag);
                        while (*ptr && !isdigit((unsigned char)*ptr)) ptr++;
                        while (*ptr && isdigit((unsigned char)*ptr)) ptr++;
                    } else {
                        break;
                    }
                } else if (isdigit((unsigned char)*ptr) || *ptr == '-') {
                    if (sscanf(ptr, "%d", &val) == 1) {
                        ev->vector_data[ev->vector_count++] = val;
                        while (*ptr && (isdigit((unsigned char)*ptr) || *ptr == '-')) ptr++;
                    } else {
                        break;
                    }
                } else {
                    ptr++;
                }
            }
            out_scenario->num_events++;
        } else if (strcasecmp(token, "REQUEST") == 0) {
            ev->type = EVENT_REQUEST;
            char *ptr = line;
            char dummy[64];
            sscanf(ptr, "%63s %31s", dummy, ev->target_name);
            ptr = strstr(ptr, ev->target_name);
            if (ptr) ptr += strlen(ev->target_name);

            ev->vector_count = 0;
            while (ptr && *ptr != '\0') {
                while (isspace((unsigned char)*ptr)) ptr++;
                if (*ptr == '\0') break;

                int val = 0;
                char res_tag[64];
                if (isalpha((unsigned char)*ptr)) {
                    if (sscanf(ptr, "%63s %d", res_tag, &val) == 2) {
                        ev->vector_data[ev->vector_count++] = val;
                        ptr = strstr(ptr, res_tag);
                        if (ptr) ptr += strlen(res_tag);
                        while (*ptr && !isdigit((unsigned char)*ptr)) ptr++;
                        while (*ptr && isdigit((unsigned char)*ptr)) ptr++;
                    } else {
                        break;
                    }
                } else if (isdigit((unsigned char)*ptr) || *ptr == '-') {
                    if (sscanf(ptr, "%d", &val) == 1) {
                        ev->vector_data[ev->vector_count++] = val;
                        while (*ptr && (isdigit((unsigned char)*ptr) || *ptr == '-')) ptr++;
                    } else {
                        break;
                    }
                } else {
                    ptr++;
                }
            }
            out_scenario->num_events++;
        } else if (strcasecmp(token, "RELEASE") == 0) {
            ev->type = EVENT_RELEASE;
            char *ptr = line;
            char dummy[64];
            sscanf(ptr, "%63s %31s", dummy, ev->target_name);
            ptr = strstr(ptr, ev->target_name);
            if (ptr) ptr += strlen(ev->target_name);

            ev->vector_count = 0;
            while (ptr && *ptr != '\0') {
                while (isspace((unsigned char)*ptr)) ptr++;
                if (*ptr == '\0') break;

                int val = 0;
                char res_tag[64];
                if (isalpha((unsigned char)*ptr)) {
                    if (sscanf(ptr, "%63s %d", res_tag, &val) == 2) {
                        ev->vector_data[ev->vector_count++] = val;
                        ptr = strstr(ptr, res_tag);
                        if (ptr) ptr += strlen(res_tag);
                        while (*ptr && !isdigit((unsigned char)*ptr)) ptr++;
                        while (*ptr && isdigit((unsigned char)*ptr)) ptr++;
                    } else {
                        break;
                    }
                } else if (isdigit((unsigned char)*ptr) || *ptr == '-') {
                    if (sscanf(ptr, "%d", &val) == 1) {
                        ev->vector_data[ev->vector_count++] = val;
                        while (*ptr && (isdigit((unsigned char)*ptr) || *ptr == '-')) ptr++;
                    } else {
                        break;
                    }
                } else {
                    ptr++;
                }
            }
            out_scenario->num_events++;
        } else if (strcasecmp(token, "TERMINATE") == 0) {
            ev->type = EVENT_TERMINATE;
            sscanf(line, "%*s %31s", ev->target_name);
            out_scenario->num_events++;
        } else if (strcasecmp(token, "CHECK_DEADLOCK") == 0) {
            ev->type = EVENT_CHECK_DEADLOCK;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "CHECK_SAFETY") == 0) {
            ev->type = EVENT_CHECK_SAFETY;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "RECOVER") == 0) {
            ev->type = EVENT_RECOVER;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "PRINT_STATE") == 0) {
            ev->type = EVENT_PRINT_STATE;
            out_scenario->num_events++;
        } else if (strcasecmp(token, "SLEEP") == 0) {
            ev->type = EVENT_SLEEP_MS;
            sscanf(line, "%*s %d", &ev->units);
            out_scenario->num_events++;
        }
    }

    fclose(f);
    return true;
}

bool scenario_run(const Scenario *scenario, ResourceManager *rm, bool interactive, bool visualize) {
    if (!scenario || !rm) return false;

    printf("\n" ANSI_BOLD "=== RUNNING SCENARIO: %s (%d events) ===" ANSI_RESET "\n",
           scenario->name, scenario->num_events);

    for (int i = 0; i < scenario->num_events; ++i) {
        const ScenarioEvent *ev = &scenario->events[i];
        printf("\n" ANSI_CYAN "[Step %02d]" ANSI_RESET " ", ev->step_num);

        switch (ev->type) {
            case EVENT_SET_MODE:
                if (strcasecmp(ev->extra_arg, "avoidance") == 0) {
                    rm_set_mode(rm, MODE_AVOIDANCE);
                    printf("Mode set to: " ANSI_YELLOW "AVOIDANCE (Banker's Algorithm)" ANSI_RESET "\n");
                } else if (strcasecmp(ev->extra_arg, "detection") == 0) {
                    rm_set_mode(rm, MODE_DETECTION);
                    printf("Mode set to: " ANSI_YELLOW "DETECTION ONLY" ANSI_RESET "\n");
                } else if (strcasecmp(ev->extra_arg, "prevention") == 0) {
                    rm_set_mode(rm, MODE_PREVENTION);
                    printf("Mode set to: " ANSI_YELLOW "PREVENTION" ANSI_RESET "\n");
                }
                break;

            case EVENT_SET_PREVENTION:
                if (strcasecmp(ev->extra_arg, "ordering") == 0) {
                    rm_set_prevention_strategy(rm, PREVENT_RESOURCE_ORDERING);
                    printf("Prevention rule: " ANSI_YELLOW "Havender's Resource Ordering" ANSI_RESET "\n");
                } else if (strcasecmp(ev->extra_arg, "all_or_nothing") == 0) {
                    rm_set_prevention_strategy(rm, PREVENT_ALL_OR_NOTHING);
                    printf("Prevention rule: " ANSI_YELLOW "Hold-and-Wait Prevention (All-or-Nothing)" ANSI_RESET "\n");
                } else {
                    rm_set_prevention_strategy(rm, PREVENT_NONE);
                    printf("Prevention rule: " ANSI_YELLOW "None" ANSI_RESET "\n");
                }
                break;

            case EVENT_SET_VICTIM_POLICY:
                if (strcasecmp(ev->extra_arg, "fewest") == 0) {
                    rm_set_victim_policy(rm, RECOVERY_FEWEST_RESOURCES);
                } else if (strcasecmp(ev->extra_arg, "most") == 0) {
                    rm_set_victim_policy(rm, RECOVERY_MOST_RESOURCES);
                } else if (strcasecmp(ev->extra_arg, "lowest_priority") == 0) {
                    rm_set_victim_policy(rm, RECOVERY_LOWEST_PRIORITY);
                } else if (strcasecmp(ev->extra_arg, "max_cycles") == 0) {
                    rm_set_victim_policy(rm, RECOVERY_MAX_CYCLES_BROKEN);
                } else {
                    rm_set_victim_policy(rm, RECOVERY_CHEAPEST);
                }
                printf("Victim policy set to: %s\n", ev->extra_arg);
                break;

            case EVENT_DEF_RESOURCE: {
                int rid = rm_register_resource(rm, ev->resource_name, ev->units);
                if (ev->priority > 0 && rid >= 0) {
                    rm->resources[rid].hierarchy_rank = ev->priority;
                }
                printf("Defined resource " ANSI_BOLD "%s" ANSI_RESET " (Capacity: %d, Rank: %d)\n",
                       ev->resource_name, ev->units, rm->resources[rid].hierarchy_rank);
                break;
            }

            case EVENT_DEF_PROCESS: {
                int pid = rm_register_process(rm, ev->target_name, ev->priority, ev->cost_weight);
                printf("Defined process " ANSI_BOLD "%s" ANSI_RESET " (PID: %d, Priority: %d, CostFactor: %.2f)\n",
                       ev->target_name, pid, ev->priority, ev->cost_weight);
                break;
            }

            case EVENT_CLAIM: {
                int pid = rm_find_process(rm, ev->target_name);
                if (pid < 0) {
                    printf(ANSI_RED "Unknown process '%s'" ANSI_RESET "\n", ev->target_name);
                    break;
                }
                int claim_vec[DD_MAX_RESOURCES] = {0};
                for (int r = 0; r < ev->vector_count && r < rm->num_resources; ++r) {
                    claim_vec[r] = ev->vector_data[r];
                }
                rm_set_process_max(rm, pid, claim_vec);
                printf("Set Max Claim for %s: [", ev->target_name);
                for (int r = 0; r < rm->num_resources; ++r) printf(" %d", rm->max_claim[pid][r]);
                printf(" ]\n");
                break;
            }

            case EVENT_REQUEST: {
                int pid = rm_find_process(rm, ev->target_name);
                if (pid < 0) {
                    printf(ANSI_RED "Unknown process '%s'" ANSI_RESET "\n", ev->target_name);
                    break;
                }
                int req_vec[DD_MAX_RESOURCES] = {0};
                if (ev->vector_count == 1 && rm->num_resources > 1) {
                    req_vec[0] = ev->vector_data[0];
                } else {
                    for (int r = 0; r < ev->vector_count && r < rm->num_resources; ++r) {
                        req_vec[r] = ev->vector_data[r];
                    }
                }
                printf("Process %s requests [", ev->target_name);
                for (int r = 0; r < rm->num_resources; ++r) printf(" %d", req_vec[r]);
                printf(" ] -> ");
                rm_request_vector(rm, pid, req_vec);
                break;
            }

            case EVENT_RELEASE: {
                int pid = rm_find_process(rm, ev->target_name);
                if (pid < 0) {
                    printf(ANSI_RED "Unknown process '%s'" ANSI_RESET "\n", ev->target_name);
                    break;
                }
                int rel_vec[DD_MAX_RESOURCES] = {0};
                for (int r = 0; r < ev->vector_count && r < rm->num_resources; ++r) {
                    rel_vec[r] = ev->vector_data[r];
                }
                printf("Process %s releases [", ev->target_name);
                for (int r = 0; r < rm->num_resources; ++r) printf(" %d", rel_vec[r]);
                printf(" ] -> ");
                rm_release_vector(rm, pid, rel_vec);
                break;
            }

            case EVENT_TERMINATE: {
                int pid = rm_find_process(rm, ev->target_name);
                if (pid < 0) {
                    printf(ANSI_RED "Unknown process '%s'" ANSI_RESET "\n", ev->target_name);
                    break;
                }
                rm_terminate_process(rm, pid);
                break;
            }

            case EVENT_CHECK_SAFETY: {
                SafeSequence seq;
                rm_is_safe(rm, &seq);
                visualizer_print_safety_report(rm, &seq);
                break;
            }

            case EVENT_CHECK_DEADLOCK: {
                DeadlockReport rep;
                bool deadlocked = rm_detect_deadlock(rm, &rep);
                if (deadlocked) {
                    visualizer_print_deadlock_alert(rm, &rep);
                } else {
                    printf(ANSI_GREEN "[NO DEADLOCK]" ANSI_RESET " System is deadlock-free.\n");
                }
                break;
            }

            case EVENT_RECOVER: {
                DeadlockReport rep;
                if (rm_detect_deadlock(rm, &rep)) {
                    RecoveryReport rec;
                    recovery_recover_system(rm, &rep, &rec);
                    visualizer_print_recovery_report(&rec);
                } else {
                    printf("[RECOVER] No deadlock present, recovery not needed.\n");
                }
                break;
            }

            case EVENT_PRINT_STATE:
                visualizer_print_matrices(rm);
                visualizer_print_graphs(rm);
                break;

            case EVENT_SLEEP_MS:
                usleep(ev->units * 1000);
                printf("Slept %d ms\n", ev->units);
                break;
        }

        if (visualize && ev->type != EVENT_PRINT_STATE) {
            visualizer_print_matrices(rm);
        }

        if (interactive) {
            printf(ANSI_DIM "\n[Press Enter to step...]" ANSI_RESET);
            getchar();
        }
    }

    printf("\n" ANSI_BOLD "=== SCENARIO EXECUTION COMPLETE ===" ANSI_RESET "\n");
    printf("Total Requests Granted : " ANSI_GREEN "%d" ANSI_RESET "\n", rm->total_requests_granted);
    printf("Total Requests Blocked : " ANSI_YELLOW "%d" ANSI_RESET "\n", rm->total_requests_blocked);
    printf("Total Requests Rejected: " ANSI_RED "%d" ANSI_RESET "\n", rm->total_requests_rejected);
    printf("Total Recoveries       : " ANSI_MAGENTA "%d" ANSI_RESET "\n\n", rm->total_recoveries_performed);

    return true;
}
