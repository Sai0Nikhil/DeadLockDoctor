/**
 * @file visualizer.c
 * @brief ANSI Terminal UI, Matrix Tables, and ASCII Graph Visualizer.
 */

#include "ui/visualizer.h"
#include "detection/detector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void visualizer_print_banner(void) {
    printf("\n" ANSI_CYAN ANSI_BOLD);
    printf("  ██████╗ ███████╗ █████╗ ██████╗ ██╗      ██████╗  ██████╗████████╗ ██████╗ ██████╗ \n");
    printf("  ██╔══██╗██╔════╝██╔══██╗██╔══██╗██║     ██╔═══██╗██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗\n");
    printf("  ██║  ██║█████╗  ███████║██║  ██║██║     ██║   ██║██║        ██║   ██║   ██║██████╔╝\n");
    printf("  ██║  ██║██╔══╝  ██╔══██║██║  ██║██║     ██║   ██║██║        ██║   ██║   ██║██╔══██╗\n");
    printf("  ██████╔╝███████╗██║  ██║██████╔╝███████╗╚██████╔╝╚██████╗   ██║   ╚██████╔╝██║  ██║\n");
    printf("  ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝ ╚═════╝  ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝\n");
    printf(ANSI_RESET);
    printf(ANSI_BOLD "  [ OS Capstone Project: High-Performance Resource Allocation & Deadlock Engine ]\n" ANSI_RESET);
    printf(ANSI_DIM  "  Version 2.4-Pro | C11 Standard | 3-Color DFS | Banker's Engine | Dual CLI/GUI\n\n" ANSI_RESET);
}

void visualizer_print_matrices(const ResourceManager *rm) {
    if (!rm) return;

    printf("\n" ANSI_BOLD ANSI_CYAN "============================== CURRENT SYSTEM MATRICES ==============================" ANSI_RESET "\n");

    // Print Available Vector
    printf(ANSI_BOLD " Available Resources Vector (Available): " ANSI_RESET "[ ");
    for (int j = 0; j < rm->num_resources; j++) {
        printf(ANSI_GREEN "%s:%d" ANSI_RESET "%s", rm->resources[j].name, rm->available[j], (j < rm->num_resources - 1) ? ", " : "");
    }
    printf(" ] (Total: [ ");
    for (int j = 0; j < rm->num_resources; j++) {
        printf("%s:%d%s", rm->resources[j].name, rm->total[j], (j < rm->num_resources - 1) ? ", " : "");
    }
    printf(" ])\n\n");

    // Table Header
    printf(ANSI_BOLD " %-10s | %-16s | %-16s | %-16s | %-12s | %-10s\n" ANSI_RESET,
           "Process", "Allocation", "Request / Need", "Max Claim", "Prio / Cost", "Status");
    printf(" -----------+------------------+------------------+------------------+--------------+------------\n");

    for (int i = 0; i < rm->num_processes; i++) {
        if (rm->processes[i].state == PROC_UNUSED) continue;

        char alloc_buf[64] = "";
        char req_buf[64] = "";
        char max_buf[64] = "";

        for (int j = 0; j < rm->num_resources; j++) {
            char tmp[16];
            snprintf(tmp, sizeof(tmp), "%d ", rm->allocation[i][j]);
            strcat(alloc_buf, tmp);

            snprintf(tmp, sizeof(tmp), "%d ", (rm->mode == MODE_AVOIDANCE) ? rm->need[i][j] : rm->request[i][j]);
            strcat(req_buf, tmp);

            snprintf(tmp, sizeof(tmp), "%d ", rm->max_claim[i][j]);
            strcat(max_buf, tmp);
        }

        const char *st_color = ANSI_GREEN;
        const char *st_str = "RUNNING";
        if (rm->processes[i].state == PROC_BLOCKED) {
            st_color = ANSI_YELLOW;
            st_str = "BLOCKED";
        } else if (rm->processes[i].state == PROC_TERMINATED) {
            st_color = ANSI_DIM;
            st_str = "TERMINATED";
        } else if (rm->processes[i].state == PROC_ABORTED) {
            st_color = ANSI_RED;
            st_str = "ABORTED";
        }

        printf(" %-10s | %-16s | %-16s | %-16s | P:%-2d C:%.1f    | %s%-10s" ANSI_RESET "\n",
               rm->processes[i].name, alloc_buf, req_buf, max_buf,
               rm->processes[i].priority, rm->processes[i].cost_weight, st_color, st_str);
    }
    printf("======================================================================================\n");
}

void visualizer_print_graphs(const ResourceManager *rm) {
    if (!rm) return;

    printf("\n" ANSI_BOLD ANSI_CYAN "======================== TOPOLOGY: RAG & WAIT-FOR GRAPH ========================" ANSI_RESET "\n");

    // Resource Allocation Graph (RAG) Dependencies
    printf(ANSI_BOLD ANSI_YELLOW " Resource Allocation Graph (RAG) Edges:\n" ANSI_RESET);
    bool has_rag = false;
    for (int i = 0; i < rm->num_processes; i++) {
        if (rm->processes[i].state == PROC_UNUSED) continue;
        for (int j = 0; j < rm->num_resources; j++) {
            if (rm->allocation[i][j] > 0) {
                printf("   [Resource] (%s) ──── (held: %d) ───► [Process] (%s)\n",
                       rm->resources[j].name, rm->allocation[i][j], rm->processes[i].name);
                has_rag = true;
            }
            int req_val = (rm->mode == MODE_AVOIDANCE) ? rm->need[i][j] : rm->request[i][j];
            if (req_val > 0) {
                printf("   [Process]  (%s) ──── (req:  %d) ───► [Resource] (%s)\n",
                       rm->processes[i].name, req_val, rm->resources[j].name);
                has_rag = true;
            }
        }
    }
    if (!has_rag) {
        printf("   (No active allocations or pending requests)\n");
    }

    // Wait-For Graph (WFG) direct Process->Process Edges
    printf("\n" ANSI_BOLD ANSI_MAGENTA " Process Wait-For Graph (WFG) Direct Dependencies:\n" ANSI_RESET);
    WaitForGraph wfg;
    detector_build_wfg(rm, &wfg);

    bool has_wfg = false;
    for (int i = 0; i < rm->num_processes; i++) {
        for (int j = 0; j < rm->num_processes; j++) {
            if (wfg.adj[i][j]) {
                int rid = wfg.waiting_on_res[i][j];
                const char *rname = (rid >= 0 && rid < rm->num_resources) ? rm->resources[rid].name : "?";
                printf("   " ANSI_RED "• %s" ANSI_RESET " ──[WAITS ON %s HELD BY]──► " ANSI_GREEN "%s" ANSI_RESET "\n",
                       rm->processes[i].name, rname, rm->processes[j].name);
                has_wfg = true;
            }
        }
    }
    if (!has_wfg) {
        printf("   (Wait-For Graph is empty. No circular waiting dependencies exist.)\n");
    }
    printf("=================================================================================\n");
}

void visualizer_print_deadlock_alert(const ResourceManager *rm, const DeadlockReport *report) {
    if (!report || !report->has_deadlock) {
        printf("\n" ANSI_BOLD ANSI_GREEN " [✓] SYSTEM HEALTH: SAFE - No Deadlocks Detected." ANSI_RESET "\n");
        return;
    }

    printf("\n" ANSI_BG_RED ANSI_BOLD " !!!!!!!!!!!!!!!!!!!!!!!! SYSTEM DEADLOCK DETECTED !!!!!!!!!!!!!!!!!!!!!!!!" ANSI_RESET "\n");
    printf(ANSI_BOLD ANSI_RED " [!] Number of Deadlocked Processes: " ANSI_RESET "%d / %d\n",
           report->num_deadlocked_processes, (rm ? rm->num_processes : 0));

    printf(ANSI_BOLD " [!] Deadlocked Processes: " ANSI_RESET);
    for (int i = 0; i < report->num_deadlocked_processes; i++) {
        int pid = report->deadlocked_pids[i];
        const char *pname = (rm && pid >= 0 && pid < rm->num_processes) ? rm->processes[pid].name : "Proc";
        printf(ANSI_BOLD ANSI_RED "%s (PID %d)" ANSI_RESET "%s",
               pname, pid, (i < report->num_deadlocked_processes - 1) ? ", " : "");
    }
    printf("\n");

    if (report->num_cycles > 0) {
        printf(ANSI_BOLD ANSI_YELLOW " [!] Critical Cycle(s) Formed: \n" ANSI_RESET);
        for (int c = 0; c < report->num_cycles; c++) {
            printf("     Cycle #%d: [ ", c + 1);
            for (int k = 0; k < report->cycles[c].length; k++) {
                int pid = report->cycles[c].pids[k];
                const char *pname = (rm && pid >= 0 && pid < rm->num_processes) ? rm->processes[pid].name : "Proc";
                printf(ANSI_BOLD ANSI_RED "%s" ANSI_RESET " ──► ", pname);
            }
            int first_pid = report->cycles[c].pids[0];
            const char *first_name = (rm && first_pid >= 0 && first_pid < rm->num_processes) ? rm->processes[first_pid].name : "Proc";
            printf(ANSI_BOLD ANSI_RED "%s" ANSI_RESET " ] (CIRCULAR WAIT DEADLOCK)\n", first_name);
        }
    }
    printf(ANSI_BG_RED "                                                                           " ANSI_RESET "\n");
}

void visualizer_print_safety_report(const ResourceManager *rm, const SafeSequence *seq) {
    if (!seq) return;

    printf("\n" ANSI_BOLD ANSI_CYAN "========================== BANKER'S SAFETY ANALYSIS ==========================" ANSI_RESET "\n");
    if (seq->is_safe) {
        printf(ANSI_BOLD ANSI_GREEN " [✓] STATE IS SAFE." ANSI_RESET " Banker's algorithm found valid execution sequence:\n\n");
        printf("     " ANSI_BOLD ANSI_GREEN "SAFE SEQUENCE:  < ");
        for (int i = 0; i < seq->count; i++) {
            int pid = seq->sequence[i];
            const char *pname = (rm && pid >= 0 && pid < rm->num_processes) ? rm->processes[pid].name : "Proc";
            printf("%s%s", pname, (i < seq->count - 1) ? " ──► " : "");
        }
        printf(" >" ANSI_RESET "\n\n");
        printf(" All processes can satisfy their maximum resource claims without causing deadlock.\n");
    } else {
        printf(ANSI_BOLD ANSI_RED " [X] STATE IS UNSAFE / BLOCKED!" ANSI_RESET "\n");
        printf(" No complete safe sequence could be constructed. Allocating this request risks deadlock.\n");
    }
    printf("==============================================================================\n");
}

void visualizer_print_recovery_report(const RecoveryReport *report) {
    if (!report) return;

    printf("\n" ANSI_BOLD ANSI_MAGENTA "=========================== RECOVERY ENGINE SUMMARY ===========================" ANSI_RESET "\n");
    printf(" Victim Process Aborted: " ANSI_BOLD ANSI_RED "%s (PID %d)" ANSI_RESET "\n", report->victim_name, report->victim_pid);
    printf(" Calculated Penalty Cost: " ANSI_BOLD "%.2f units" ANSI_RESET "\n", report->calculated_cost);
    printf(" Total Resources Freed:   " ANSI_BOLD "%d units" ANSI_RESET "\n", report->total_units_freed);
    printf(" Resulting Status:        %s\n\n",
           report->recovery_successful ? ANSI_GREEN ANSI_BOLD "[ RESOLVED - SYSTEM SAFE ]" ANSI_RESET : ANSI_YELLOW "[ PARTIALLY RESOLVED - ADDITIONAL STEP NEEDED ]" ANSI_RESET);

    printf(ANSI_BOLD " Reclaimed Resources Vector: [ " ANSI_RESET);
    for (int r = 0; r < DD_MAX_RESOURCES; r++) {
        if (report->resources_freed[r] > 0) {
            printf("R%d:%d ", r, report->resources_freed[r]);
        }
    }
    printf("]\n");
    printf("================================================================================\n");
}
