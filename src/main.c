/**
 * @file main.c
 * @brief DeadlockDoctor Main Application Entrypoint & Shell.
 */

#include "deadlockdoctor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

static void setup_classic_silberschatz_demo(ResourceManager *rm);
static void run_guided_walkthrough(ResourceManager *rm);
static void run_interactive_shell(ResourceManager *rm);
static void print_shell_help(void);

static void trim_str(char *str) {
    if (!str) return;
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || isspace((unsigned char)str[len - 1]))) {
        str[len - 1] = '\0';
        len--;
    }
}

static void setup_classic_silberschatz_demo(ResourceManager *rm) {
    rm_init(rm, MODE_AVOIDANCE);

    // Resources: A(10), B(5), C(7)
    rm_register_resource(rm, "A", 10);
    rm_register_resource(rm, "B", 5);
    rm_register_resource(rm, "C", 7);

    // Processes: P0 to P4
    int p0 = rm_register_process(rm, "P0", 1, 1.0);
    int p1 = rm_register_process(rm, "P1", 2, 2.0);
    int p2 = rm_register_process(rm, "P2", 3, 1.5);
    int p3 = rm_register_process(rm, "P3", 4, 3.0);
    int p4 = rm_register_process(rm, "P4", 5, 2.5);

    // Max Claims
    int max_p0[3] = {7, 5, 3};
    int max_p1[3] = {3, 2, 2};
    int max_p2[3] = {9, 0, 2};
    int max_p3[3] = {2, 2, 2};
    int max_p4[3] = {4, 3, 3};

    rm_set_process_max(rm, p0, max_p0);
    rm_set_process_max(rm, p1, max_p1);
    rm_set_process_max(rm, p2, max_p2);
    rm_set_process_max(rm, p3, max_p3);
    rm_set_process_max(rm, p4, max_p4);

    // Initial allocations:
    // P0: 0,1,0 | P1: 2,0,0 | P2: 3,0,2 | P3: 2,1,1 | P4: 0,0,2
    int alloc_p0[3] = {0, 1, 0};
    int alloc_p1[3] = {2, 0, 0};
    int alloc_p2[3] = {3, 0, 2};
    int alloc_p3[3] = {2, 1, 1};
    int alloc_p4[3] = {0, 0, 2};

    rm_request_vector(rm, p0, alloc_p0);
    rm_request_vector(rm, p1, alloc_p1);
    rm_request_vector(rm, p2, alloc_p2);
    rm_request_vector(rm, p3, alloc_p3);
    rm_request_vector(rm, p4, alloc_p4);
}

int main(int argc, char **argv) {
    visualizer_print_banner();

    ResourceManager rm;
    setup_classic_silberschatz_demo(&rm);

    // Command line argument flags
    if (argc > 1) {
        if (strcmp(argv[1], "--shell") == 0 || strcmp(argv[1], "-s") == 0) {
            run_interactive_shell(&rm);
            rm_destroy(&rm);
            return 0;
        } else if (strcmp(argv[1], "--demo") == 0 || strcmp(argv[1], "-d") == 0) {
            LiveDemoConfig cfg;
            live_demo_default_config(&cfg);
            live_demo_run(&cfg);
            rm_destroy(&rm);
            return 0;
        } else if (strcmp(argv[1], "--walkthrough") == 0 || strcmp(argv[1], "-w") == 0) {
            run_guided_walkthrough(&rm);
            rm_destroy(&rm);
            return 0;
        } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage: ./deadlockdoctor [OPTIONS] [SCENARIO_FILE]\n\n");
            printf("Options:\n");
            printf("  --shell, -s         Launch directly into the interactive DeadlockDoctor Shell\n");
            printf("  --demo, -d          Run the real POSIX Multithreading Deadlock Race Demo\n");
            printf("  --walkthrough, -w   Launch step-by-step educational walkthrough\n");
            printf("  --help, -h          Show this help screen\n\n");
            rm_destroy(&rm);
            return 0;
        } else {
            // Assume scenario file argument
            printf("[*] Loading scenario file: %s\n", argv[1]);
            Scenario sc;
            if (scenario_parse_file(argv[1], &sc)) {
                ResourceManager sc_rm;
                rm_init(&sc_rm, MODE_AVOIDANCE);
                scenario_run(&sc, &sc_rm, false, true);
                rm_destroy(&sc_rm);
            }
            rm_destroy(&rm);
            return 0;
        }
    }

    // Interactive Mode Selector
    printf(ANSI_BOLD " Select DeadlockDoctor Operational Mode:\n" ANSI_RESET);
    printf("  " ANSI_GREEN "[1]" ANSI_RESET " Interactive Guided Walkthrough & Visual Simulations\n");
    printf("  " ANSI_CYAN  "[2]" ANSI_RESET " DeadlockDoctor REPL Shell (CLI + OS Commands: pwd, ls, etc.)\n");
    printf("  " ANSI_YELLOW"[3]" ANSI_RESET " Real POSIX Threads Lock Race & Live Watchdog Demo\n");
    printf("  " ANSI_MAGENTA"[4]" ANSI_RESET " Load a Pre-configured Scenario (.dd file)\n");
    printf("  " ANSI_RED   "[5]" ANSI_RESET " Exit\n\n");
    printf(ANSI_BOLD " Enter choice [1-5]: " ANSI_RESET);

    char choice_buf[32];
    if (fgets(choice_buf, sizeof(choice_buf), stdin)) {
        trim_str(choice_buf);
        if (strcmp(choice_buf, "1") == 0) {
            run_guided_walkthrough(&rm);
        } else if (strcmp(choice_buf, "2") == 0) {
            run_interactive_shell(&rm);
        } else if (strcmp(choice_buf, "3") == 0) {
            LiveDemoConfig cfg;
            live_demo_default_config(&cfg);
            live_demo_run(&cfg);
        } else if (strcmp(choice_buf, "4") == 0) {
            printf("\n Enter scenario file path (e.g., scenarios/01_bankers_safe.dd): ");
            char sc_path[256];
            if (fgets(sc_path, sizeof(sc_path), stdin)) {
                trim_str(sc_path);
                Scenario sc;
                if (scenario_parse_file(sc_path, &sc)) {
                    ResourceManager sc_rm;
                    rm_init(&sc_rm, MODE_AVOIDANCE);
                    scenario_run(&sc, &sc_rm, false, true);
                    rm_destroy(&sc_rm);
                }
            }
        } else {
            printf(" Exiting DeadlockDoctor. Goodbye!\n");
        }
    }

    rm_destroy(&rm);
    return 0;
}

static void run_guided_walkthrough(ResourceManager *rm) {
    printf("\n" ANSI_BOLD ANSI_GREEN "=== STEP 1: INITIAL SYSTEM STATE & MATRICES ===" ANSI_RESET "\n");
    visualizer_print_matrices(rm);
    visualizer_print_graphs(rm);

    printf("\n" ANSI_BOLD ANSI_CYAN "=== STEP 2: RUNNING BANKER'S SAFETY ALGORITHM ===" ANSI_RESET "\n");
    SafeSequence seq;
    banker_check_safety(rm, &seq);
    visualizer_print_safety_report(rm, &seq);

    printf("\n" ANSI_BOLD ANSI_YELLOW "=== STEP 3: SIMULATING DANGEROUS RESOURCE REQUEST ===" ANSI_RESET "\n");
    printf(" Process P1 attempts to request [A:1, B:0, C:2]...\n");
    int req[3] = {1, 0, 2};
    SafeSequence req_seq;
    bool approved = banker_evaluate_request(rm, 1, req, &req_seq);
    if (approved) {
        printf(ANSI_GREEN " [✓] Banker's Engine: Request APPROVED safely without violating invariants.\n" ANSI_RESET);
    } else {
        printf(ANSI_RED " [X] Banker's Engine: Request REJECTED! State would transition to unsafe.\n" ANSI_RESET);
    }

    printf("\n" ANSI_BOLD ANSI_RED "=== STEP 4: INTENTIONALLY INJECTING CIRCULAR WAIT DEADLOCK ===" ANSI_RESET "\n");
    // Switch to detection mode and inject circular wait
    rm_set_mode(rm, MODE_DETECTION);
    rm->allocation[0][0] = 1;
    rm->allocation[1][1] = 1;
    rm->request[0][1] = 1;
    rm->request[1][0] = 1;
    rm->available[0] = 0;
    rm->available[1] = 0;
    rm->processes[0].state = PROC_BLOCKED;
    rm->processes[1].state = PROC_BLOCKED;

    visualizer_print_matrices(rm);
    visualizer_print_graphs(rm);

    DeadlockReport d_rep;
    detector_detect_deadlock(rm, &d_rep);
    visualizer_print_deadlock_alert(rm, &d_rep);

    printf("\n" ANSI_BOLD ANSI_MAGENTA "=== STEP 5: AUTOMATED COST-BASED RECOVERY ===" ANSI_RESET "\n");
    RecoveryReport rec;
    recovery_recover_system(rm, &d_rep, &rec);
    visualizer_print_recovery_report(&rec);

    visualizer_print_matrices(rm);
    printf("\n" ANSI_GREEN "[+] Guided Walkthrough Complete!" ANSI_RESET "\n\n");
}

static void print_shell_help(void) {
    printf("\n" ANSI_BOLD ANSI_CYAN "DeadlockDoctor Shell - Command Reference:" ANSI_RESET "\n");
    printf("  " ANSI_YELLOW "Core Simulation & Matrix Operations:" ANSI_RESET "\n");
    printf("    reset                   - Reset resource manager to clean slate\n");
    printf("    add_res <name> <inst>   - Register a new resource type with total instances\n");
    printf("    add_proc <name> [prio]  - Register a new process (default prio 1, cost 1.0)\n");
    printf("    set_max <p> <r> <k>     - Set max claim of process p on resource r\n");
    printf("    req <p> <r> <k>         - Process p requests k units of resource r\n");
    printf("    rel <p> <r> <k>         - Process p releases k units of resource r\n");
    printf("    show | matrices         - Display Allocation, Request/Need, Max, and Available matrices\n");
    printf("    graph                   - Display Resource Allocation Graph (RAG) and Wait-For Graph (WFG)\n");
    printf("\n  " ANSI_YELLOW "Algorithms & Analysis:" ANSI_RESET "\n");
    printf("    banker                  - Run Banker's Safety Algorithm to check state and safe sequence\n");
    printf("    detect                  - Execute 3-Color DFS Wait-For Cycle & Multi-Instance Detection\n");
    printf("    recover                 - Execute automated cost-function victim recovery\n");
    printf("    mode <avoid|detect|prev>- Switch operational mode\n");
    printf("    prevent <ord|all|none>  - Set deadlock prevention strategy\n");
    printf("    demo                    - Run live POSIX Multithreading Mutex Race demonstration\n");
    printf("    load <path>             - Load and execute scenario script (.dd file)\n");
    printf("\n  " ANSI_YELLOW "Integrated Host OS Shell Commands:" ANSI_RESET "\n");
    printf("    pwd, ls, cd, clear, cat, whoami, etc. (Executed directly via OS subshell)\n");
    printf("    exit | quit             - Exit DeadlockDoctor Shell\n\n");
}

static void run_interactive_shell(ResourceManager *rm) {
    char line[512];
    printf("\n" ANSI_BOLD ANSI_GREEN "Entering DeadlockDoctor Shell. Type 'help' for command manual." ANSI_RESET "\n");

    while (1) {
        printf(ANSI_BOLD ANSI_CYAN "DeadlockDoctor> " ANSI_RESET);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        trim_str(line);
        if (strlen(line) == 0) continue;

        // Tokenize command
        char cmd[64] = "";
        char arg1[128] = "";
        char arg2[128] = "";
        char arg3[128] = "";
        char arg4[128] = "";
        int parsed = sscanf(line, "%63s %127s %127s %127s %127s", cmd, arg1, arg2, arg3, arg4);

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            printf("Exiting DeadlockDoctor Shell. Goodbye!\n");
            break;
        } else if (strcmp(cmd, "help") == 0) {
            print_shell_help();
        } else if (strcmp(cmd, "show") == 0 || strcmp(cmd, "matrices") == 0) {
            visualizer_print_matrices(rm);
        } else if (strcmp(cmd, "graph") == 0 || strcmp(cmd, "graphs") == 0) {
            visualizer_print_graphs(rm);
        } else if (strcmp(cmd, "banker") == 0) {
            SafeSequence seq;
            banker_check_safety(rm, &seq);
            visualizer_print_safety_report(rm, &seq);
        } else if (strcmp(cmd, "detect") == 0) {
            DeadlockReport rep;
            detector_detect_deadlock(rm, &rep);
            visualizer_print_deadlock_alert(rm, &rep);
        } else if (strcmp(cmd, "recover") == 0) {
            DeadlockReport rep;
            detector_detect_deadlock(rm, &rep);
            if (!rep.has_deadlock) {
                printf(ANSI_GREEN "[✓] System is safe; no recovery needed.\n" ANSI_RESET);
            } else {
                RecoveryReport rec_rep;
                recovery_recover_system(rm, &rep, &rec_rep);
                visualizer_print_recovery_report(&rec_rep);
            }
        } else if (strcmp(cmd, "reset") == 0) {
            rm_reset(rm);
            printf(ANSI_GREEN "[✓] System reset to empty state.\n" ANSI_RESET);
        } else if (strcmp(cmd, "add_res") == 0 && parsed >= 3) {
            int inst = atoi(arg2);
            int rid = rm_register_resource(rm, arg1, inst);
            if (rid >= 0) {
                printf(ANSI_GREEN "[✓] Registered resource '%s' (RID: %d, Instances: %d)\n" ANSI_RESET, arg1, rid, inst);
            } else {
                printf(ANSI_RED "[!] Failed to register resource.\n" ANSI_RESET);
            }
        } else if (strcmp(cmd, "add_proc") == 0 && parsed >= 2) {
            int prio = (parsed >= 3) ? atoi(arg2) : 1;
            int pid = rm_register_process(rm, arg1, prio, 1.0);
            if (pid >= 0) {
                printf(ANSI_GREEN "[✓] Registered process '%s' (PID: %d, Priority: %d)\n" ANSI_RESET, arg1, pid, prio);
            } else {
                printf(ANSI_RED "[!] Failed to register process.\n" ANSI_RESET);
            }
        } else if (strcmp(cmd, "set_max") == 0 && parsed >= 4) {
            int p = atoi(arg1);
            int r = atoi(arg2);
            int k = atoi(arg3);
            if (rm_set_process_max_single(rm, p, r, k)) {
                printf(ANSI_GREEN "[✓] Set Max Claim for Process %d on Resource %d = %d\n" ANSI_RESET, p, r, k);
            } else {
                printf(ANSI_RED "[!] Failed to set max claim.\n" ANSI_RESET);
            }
        } else if (strcmp(cmd, "req") == 0 && parsed >= 4) {
            int p = atoi(arg1);
            int r = atoi(arg2);
            int k = atoi(arg3);
            RequestResult res = rm_request(rm, p, r, k);
            if (res == REQ_GRANTED) {
                printf(ANSI_GREEN "[✓] Request GRANTED: Process %d acquired %d units of Resource %d.\n" ANSI_RESET, p, k, r);
            } else if (res == REQ_BLOCKED_UNAVAILABLE) {
                printf(ANSI_YELLOW "[!] Request BLOCKED: Insufficient available units. Process %d waiting.\n" ANSI_RESET, p);
            } else if (res == REQ_BLOCKED_UNSAFE) {
                printf(ANSI_RED "[!] Request BLOCKED (Banker's Avoidance): Granting would enter UNSAFE state.\n" ANSI_RESET);
            } else {
                printf(ANSI_RED "[!] Request REJECTED (Error code %d).\n" ANSI_RESET, res);
            }
        } else if (strcmp(cmd, "rel") == 0 && parsed >= 4) {
            int p = atoi(arg1);
            int r = atoi(arg2);
            int k = atoi(arg3);
            if (rm_release(rm, p, r, k)) {
                printf(ANSI_GREEN "[✓] Process %d released %d units of Resource %d.\n" ANSI_RESET, p, k, r);
            } else {
                printf(ANSI_RED "[!] Failed to release resources.\n" ANSI_RESET);
            }
        } else if (strcmp(cmd, "mode") == 0 && parsed >= 2) {
            if (strcasecmp(arg1, "avoid") == 0) {
                rm_set_mode(rm, MODE_AVOIDANCE);
                printf(ANSI_GREEN "[✓] Mode set to AVOIDANCE (Banker's Algorithm)\n" ANSI_RESET);
            } else if (strcasecmp(arg1, "detect") == 0) {
                rm_set_mode(rm, MODE_DETECTION);
                printf(ANSI_GREEN "[✓] Mode set to DETECTION (Immediate grant, periodic detection)\n" ANSI_RESET);
            } else if (strcasecmp(arg1, "prev") == 0) {
                rm_set_mode(rm, MODE_PREVENTION);
                printf(ANSI_GREEN "[✓] Mode set to PREVENTION\n" ANSI_RESET);
            }
        } else if (strcmp(cmd, "demo") == 0) {
            LiveDemoConfig cfg;
            live_demo_default_config(&cfg);
            live_demo_run(&cfg);
        } else if (strcmp(cmd, "load") == 0 && parsed >= 2) {
            Scenario sc;
            if (scenario_parse_file(arg1, &sc)) {
                scenario_run(&sc, rm, false, true);
            }
        } else if (strcmp(cmd, "cd") == 0) {
            if (parsed >= 2) {
                if (chdir(arg1) != 0) {
                    perror("cd failed");
                }
            } else {
                const char *home = getenv("HOME");
                if (chdir(home ? home : ".") != 0) {
                    /* ignore */
                }
            }
        } else if (strcmp(cmd, "clear") == 0) {
            #ifdef _WIN32
            int res = system("cls");
            #else
            int res = system("clear");
            #endif
            (void)res;
        } else {
            // Passthrough arbitrary commands to host OS subshell (e.g. pwd, ls, echo, git status)
            int sys_res = system(line);
            (void)sys_res;
        }
    }
}
