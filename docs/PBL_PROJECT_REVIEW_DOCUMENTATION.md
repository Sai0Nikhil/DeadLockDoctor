# DEADLOCKDOCTOR: OS Capstone Project Documentation
## Use Case #11: Resource Allocation & Deadlock Management System
### Operating Systems Capstone Project • Review - 1

---

### Team Information (3 Students)
- **[Student 1 Name]** (`[Student 1 Roll No]`) — Core Algorithmic Architecture, Banker's Avoidance, 3-Color DFS Cycle Detection, AddressSanitizer Verification.
- **[Student 2 Name]** (`[Student 2 Roll No]`) — POSIX Multithreading Mutex Race, Watchdog Engine, Cost-Function Recovery Formulation, Prevention Guard.
- **[Student 3 Name]** (`[Student 3 Roll No]`) — Dual CLI Shell (Host OS Passthrough), Interactive Web GUI Dashboard (SVG/Canvas Topology), Scenario Parser (.dd DSL).

---

## 1. Project Identification (5 Marks)

### 1.1 Topic & Domain
- **Approved Domain:** Operating System Resource Subsystems & Concurrency Management
- **Approved Topic:** Use Case #11 — Resource Allocation & Deadlock Management System
- **Project Title:** DeadlockDoctor (Version 2.4-Pro)

### 1.2 Problem Statement
In modern concurrent operating systems, databases, and high-performance server runtimes, asynchronous processes contend for shared hardware and software resources. When resource allocation orders interleave unfavorably, systems enter **deadlock** satisfying all four Coffman conditions:
1. **Mutual Exclusion:** Exclusive unshareable resource hold.
2. **Hold and Wait:** Retaining allocated resources while requesting additional units.
3. **No Preemption:** Inability of the OS to forcibly revoke locks without process cooperation.
4. **Circular Wait:** Closed chain of dependencies $P_0 \to P_1 \to \dots \to P_n \to P_0$.

Existing operating systems lack real-time transparent diagnostics, relying on brute-force timeouts or unguided thread terminations that cause massive computational waste and process starvation. DeadlockDoctor resolves this crisis by providing a unified engine for avoidance, cycle detection, cost-optimal recovery, prevention, and live visual monitoring.

### 1.3 Objectives & Scope
- **Avoidance:** Full Dijkstra's Banker's Algorithm computing safe sequence vectors in $O(P^2 \times R)$ time.
- **Detection:** Linear $O(V + E)$ 3-color DFS graph cycle detection on Wait-For Graphs (WFG) and matrix reduction on multi-instance resources.
- **Recovery:** Starvation-preventing penalty formulation factoring priority, resources held, CPU execution time, and cycle impact.
- **Live Concurrency:** Real POSIX pthread mutex lock race harness with background watchdog detection in $<100\text{ms}$.
- **Dual Presentation:** Integrated CLI REPL (with host OS passthrough) and interactive SVG/Canvas web GUI.

---

## 2. Requirements Analysis (10 Marks)

### 2.1 Functional Requirements (FR)
| ID | Requirement | Description |
|---|---|---|
| **FR-01** | Multi-Instance Resource Manager | Tracks Allocation, Need, Max Claim, Request matrices and Available vectors up to 64 processes and 32 resources. |
| **FR-02** | Banker's Safety Engine | Evaluates tentative allocations and constructs complete safe sequences $\langle P_{i_1}, P_{i_2}, \dots, P_{i_n} \rangle$. |
| **FR-03** | 3-Color DFS Cycle Detector | Traverses Wait-For Graphs using WHITE/GRAY/BLACK states to identify circular back-edges in $O(V+E)$ time. |
| **FR-04** | Multi-Instance Matrix Reducer | Vector reduction algorithm detecting multi-instance resource deadlocks. |
| **FR-05** | Cost-Based Recovery Engine | Selects optimal victim processes across Lowest Cost, Lowest Priority, Max Resources, and Max Cycles Broken policies. |
| **FR-06** | Prevention Rule Enforcer | Enforces Havender's Resource Ordering and Hold-and-Wait all-or-nothing constraints. |
| **FR-07** | Live POSIX Concurrency Harness | Real pthread mutex contention monitored by an asynchronous watchdog thread. |
| **FR-08** | Scriptable Scenario Runner | Parses and executes `.dd` domain-specific scenario scripts. |

### 2.2 Non-Functional Requirements (NFR)
- **NFR-01 Performance:** Detection latency $< 1\text{ms}$ for $64 \times 32$ matrices.
- **NFR-02 Memory Hardening:** Zero memory leaks verified under AddressSanitizer and UndefinedBehaviorSanitizer.
- **NFR-03 Thread Safety:** Mutex-synchronized ResourceManager state.
- **NFR-04 Portability:** ISO/IEC 9899:2011 (C11) standard compliant across Linux and WSL.

---

## 3. System Design & Architecture (10 Marks)

### 3.1 Layered Architecture
```
+-------------------------------------------------------------------------+
|                  Layer 4: User Interface & Presentation                 |
|  - Interactive DeadlockDoctor CLI Shell (with Host OS Passthrough)     |
|  - Real-Time Web GUI Dashboard (HTML5 / SVG / Canvas Topology)         |
|  - ANSI Colorized Terminal Visualizer & ASCII Matrices                  |
+-------------------------------------------------------------------------+
                                    |
+-------------------------------------------------------------------------+
|                  Layer 3: Algorithmic & Diagnostic Engines              |
|  - Dijkstra's Banker's Avoidance Engine                                 |
|  - 3-Color DFS Cycle Detector (Wait-For Graph)                          |
|  - Silberschatz Multi-Instance Matrix Reduction Engine                  |
|  - Starvation-Aware Multi-Attribute Cost Recovery Engine                |
|  - Havender's Hierarchical Prevention Guard                             |
+-------------------------------------------------------------------------+
                                    |
+-------------------------------------------------------------------------+
|              Layer 2: Core Data Structure & Graph Engine                |
|  - Multi-Instance Allocation, Need, Max, Request Matrices & Vectors     |
|  - Bipartite Resource Allocation Graph (RAG)                            |
|  - Process-to-Process Wait-For Graph (WFG)                              |
|  - Scriptable Scenario Parser (.dd DSL)                                 |
+-------------------------------------------------------------------------+
                                    |
+-------------------------------------------------------------------------+
|               Layer 1: OS Kernel & Concurrency Subsystem                |
|  - POSIX Threads (pthread_create, pthread_join, pthread_mutex_trylock)  |
|  - Atomic Synchronization Flags (stdatomic.h)                           |
|  - AddressSanitizer & UndefinedBehaviorSanitizer Runtimes               |
+-------------------------------------------------------------------------+
```

### 3.2 Mathematical Formulations

#### 3.2.1 Dijkstra's Banker's Algorithm
$$\text{Need}[i][j] = \text{Max}[i][j] - \text{Allocation}[i][j]$$
Iteratively find process $P_i$ such that $\text{Finish}[i] = \text{false}$ and $\text{Need}[i] \le \text{Work}$. Set $\text{Work} = \text{Work} + \text{Allocation}[i]$ and $\text{Finish}[i] = \text{true}$. If all $\text{Finish}[i] = \text{true}$, system is **SAFE**.

#### 3.2.2 3-Color DFS Cycle Detection
Nodes are classified into:
- **WHITE (0):** Unvisited node.
- **GRAY (1):** Currently in recursion stack (active exploration path).
- **BLACK (2):** Fully explored node.

Encountering an edge $u \to v$ where $\text{Color}[v] = \text{GRAY}$ proves a directed back-edge cycle in $O(V + E)$ linear time.

#### 3.2.3 Starvation-Aware Cost Metric Formulation
$$\text{Cost}(P_i) = \left[ (\text{Prio}_i \cdot W_{\text{prio}}) + (\text{Held}_i \cdot W_{\text{held}}) + (\text{CPU\_time}_i \cdot W_{\text{cpu}}) - (\text{CyclesBroken}_i \cdot W_{\text{cycle}}) \right] \cdot \text{CostWeight}_i$$

---

## 4. Working Environment & Toolchain (10 Marks)

- **Operating System:** Ubuntu 24.04 LTS on Windows Subsystem for Linux (WSL2)
- **Compiler:** GCC 15.2.0 (`-std=c11 -Wall -Wextra -Wpedantic -O2 -pthread`)
- **Sanitizers:** AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (`-fsanitize=address,undefined`)
- **Build System:** GNU Make 4.4.1
- **Web UI & Presentation Engine:** Python 3.11 with `docx`, `pptx`, `http.server`

---

## 5. Working Prototype & Verification (10 Marks)

### 5.1 Test Suite Summary (24 / 24 Tests Passing)
```
============================================================
   DEADLOCKDOCTOR C11 UNIT & INTEGRATION TEST SUITE        
============================================================
[TEST SUITE: Banker's Algorithm]
  [PASS] Silberschatz initial state is recognized as SAFE
  [PASS] Safe sequence contains all 5 processes
  [PASS] P1 request (1,0,2) is evaluated as SAFE
  [PASS] P0 request (0,2,0) after P1 allocation is rejected as UNSAFE
  [PASS] Request exceeding claim is rejected

[TEST SUITE: Deadlock Detection & Graph Cycles]
  [PASS] 3-Color DFS detects 3-node cycle
  [PASS] All 3 processes identified as deadlocked
  [PASS] Cycle recorded in report
  [PASS] 3-Color DFS returns false for acyclic DAG
  [PASS] Multi-instance matrix algorithm detects deadlock
  [PASS] P0 and P1 flagged in multi-instance deadlock

[TEST SUITE: Cost-Based Deadlock Recovery]
  [PASS] Deadlock state established
  [PASS] Cheapest victim policy selects P_Cheap (lowest penalty)
  [PASS] Recovery execution successfully resolves deadlock
  [PASS] Recovery report confirms P_Cheap aborted
  [PASS] Victim state marked PROC_ABORTED

[TEST SUITE: Deadlock Prevention Rules]
  [PASS] Resource ordering permits requesting higher rank (1 > 0)
  [PASS] Resource ordering denies requesting lower rank (1 <= 2)
  [PASS] All-or-Nothing denies request while holding existing resources

[TEST SUITE: Scenario Parser & Runner]
  [PASS] Successfully parsed 01_bankers_safe.dd
  [PASS] Scenario contains parsed event sequence
  [PASS] Scenario simulation executes through completion
  [PASS] 5 processes registered during scenario replay
  [PASS] 3 resources registered during scenario replay

============================================================
 [✓] ALL 24 / 24 TESTS PASSED SUCCESSFULLY! (0 LEAKS)
============================================================
```

### 5.2 Live Multithreading Demo Execution Log
```
================================================================
   DEADLOCKDOCTOR: LIVE POSIX THREADS MULTITHREADING DEMO       
================================================================
 Spawning 3 POSIX worker threads competing for 3 Mutex Locks...
 Dependency Topology: Circular Chain (P0->R0,req R1; P1->R1,req R2; ... Pn-1->Rn-1,req R0)

[GRANT] Request granted to P0 (Thread 0 Acquired Mutex R0)
[GRANT] Request granted to P1 (Thread 1 Acquired Mutex R1)
[GRANT] Request granted to P2 (Thread 2 Acquired Mutex R2)
[BLOCK] P1 requested 1 units of R2 -> BLOCKED
[BLOCK] P0 requested 1 units of R1 -> BLOCKED
[BLOCK] P2 requested 1 units of R0 -> BLOCKED

 [!] DEADLOCK DETECTED BY WATCHDOG AT T = +100 ms 
 [!] Number of Deadlocked Processes: 3 / 3
 [!] Critical Cycle: [ P0 ──► P1 ──► P2 ──► P0 ] (CIRCULAR WAIT DEADLOCK)

>>> EXECUTING AUTOMATED RECOVERY ENGINE <<<
[RECOVERY] Aborted victim process P0 (PID 0), freed 1 resource units
[UNBLOCK] Blocked process P2 was granted its pending request
[UNBLOCK] Blocked process P1 was granted its pending request

[+] Live Multithreading Demo Completed Cleanly (All Locks Released, 0 Leaks).
```

---

## 6. Review-2 Project Roadmap
- **Phase 2:** Priority Ceiling & Priority Inheritance Protocol (PIP) integration.
- **Phase 3:** Linux eBPF kernel futex tracing for native external application monitoring.
- **Phase 4:** Distributed cluster deadlock management with Raft consensus.
