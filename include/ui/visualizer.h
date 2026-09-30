/**
 * @file visualizer.h
 * @brief ANSI Terminal UI, Matrix Tables, and ASCII Graph Visualizer.
 */

#ifndef DD_UI_VISUALIZER_H
#define DD_UI_VISUALIZER_H

#include "core/resource_mgr.h"
#include "core/graph.h"
#include <stdbool.h>

#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_DIM     "\x1b[2m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[37m"
#define ANSI_BG_RED  "\x1b[41m"
#define ANSI_BG_GRN  "\x1b[42m"

void visualizer_print_matrices(const ResourceManager *rm);
void visualizer_print_graphs(const ResourceManager *rm);
void visualizer_print_deadlock_alert(const ResourceManager *rm, const DeadlockReport *report);
void visualizer_print_safety_report(const ResourceManager *rm, const SafeSequence *seq);
void visualizer_print_recovery_report(const RecoveryReport *report);
void visualizer_print_banner(void);

#endif /* DD_UI_VISUALIZER_H */
