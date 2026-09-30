/**
 * @file graph.h
 * @brief Graph representations (RAG & WFG) and cycle detection for DeadlockDoctor.
 */

#ifndef DD_CORE_GRAPH_H
#define DD_CORE_GRAPH_H

#include "core/types.h"
#include <stdbool.h>

#define MAX_GRAPH_NODES (DD_MAX_PROCESSES + DD_MAX_RESOURCES)

typedef enum {
    NODE_PROCESS = 1,
    NODE_RESOURCE
} GraphNodeType;

typedef struct {
    int id;                 /* PID or RID */
    GraphNodeType type;
    char label[DD_MAX_NAME_LEN];
} GraphNode;

/* Wait-For Graph (WFG): Direct process-to-process wait dependencies */
typedef struct {
    int num_nodes;          /* Number of processes */
    int pids[DD_MAX_PROCESSES];
    bool adj[DD_MAX_PROCESSES][DD_MAX_PROCESSES];  /* adj[i][j] == true means process i waits for process j */
    int waiting_on_res[DD_MAX_PROCESSES][DD_MAX_PROCESSES]; /* RID causing the wait */
} WaitForGraph;

/* Resource-Allocation Graph (RAG): Bipartite graph of processes and resources */
typedef struct {
    int num_proc_nodes;
    int num_res_nodes;
    int proc_ids[DD_MAX_PROCESSES];
    int res_ids[DD_MAX_RESOURCES];
    
    /* Request edges: Process -> Resource (proc_requests_res[P][R] = units requested) */
    int request_edges[DD_MAX_PROCESSES][DD_MAX_RESOURCES];

    /* Assignment edges: Resource -> Process (res_assigned_to_proc[R][P] = units assigned) */
    int assignment_edges[DD_MAX_RESOURCES][DD_MAX_PROCESSES];
} ResourceAllocationGraph;

/* Graph Construction Functions */
void wfg_init(WaitForGraph *wfg);
void wfg_add_edge(WaitForGraph *wfg, int waiting_pid, int holding_pid, int rid);
bool wfg_has_edge(const WaitForGraph *wfg, int waiting_pid, int holding_pid);

void rag_init(ResourceAllocationGraph *rag);
void rag_add_request_edge(ResourceAllocationGraph *rag, int pid, int rid, int units);
void rag_add_assignment_edge(ResourceAllocationGraph *rag, int rid, int pid, int units);

/* Cycle Detection Algorithms */
bool wfg_find_cycles(const WaitForGraph *wfg, DeadlockReport *report);
bool rag_find_cycles(const ResourceAllocationGraph *rag, DeadlockReport *report);

#endif /* DD_CORE_GRAPH_H */
