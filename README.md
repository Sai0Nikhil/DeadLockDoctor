# DeadLockDoctor 🩺⚡
> **High-Performance Resource Allocation & Deadlock Management System**  
> *Operating Systems Capstone Project • Review - 1 (CO-1 to CO-5 Coverage)*

[![C11 Standard](https://img.shields.io/badge/Language-C11-00599C?logo=c)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Build Status](https://img.shields.io/badge/Build-Passing-10B981?logo=gcc)](Makefile)
[![Unit Tests](https://img.shields.io/badge/Tests-24%2F24%20PASS-2563EB)](tests/)
[![Memory Safety](https://img.shields.io/badge/ASan-0%20Leaks-059669)](Makefile)
[![Web Dashboard](https://img.shields.io/badge/UI-CLI%20%2B%20Web%20GUI-7C3AED)](gui/)

---

## 📌 Executive Summary
**DeadlockDoctor** is a modular C11 kernel-style system designed to simulate, avoid, detect, and automatically resolve complex resource deadlocks in multithreaded and distributed OS environments.

```
 ┌──────────────────────┐   ┌──────────────────────┐   ┌──────────────────────┐
 │ 1. CLI REPL Shell    │   │ 2. .dd Script Parser │   │ 3. Web GUI Dashboard │
 └──────────┬───────────┘   └──────────┬───────────┘   └──────────┬───────────┘
            │ CLI Command              │ Replay Event             │ REST API
            ▼                          ▼                          ▼
 ┌────────────────────────────────────────────────────────────────────────────┐
 │                  Central Engine & State Repository                         │
 │   • Allocation, Need, Max, Available Matrices [Thread-Safe Mutex Guard]    │
 │   • Dynamic Mode Switcher (Avoidance | Detection | Prevention)             │
 └──────────────┬──────────────────────┬──────────────────────┬───────────────┘
                │ Mode: Avoid          │ Mode: Detect         │ Mode: Prev
                ▼                      ▼                      ▼
 ┌──────────────────────┐   ┌──────────────────────┐   ┌──────────────────────┐
 │  Banker's Avoidance  │   │   Cycle Detection    │   │  Prevention Engine   │
 │ • Speculative Grant  │   │ • WFG Builder        │   │ • Havender Ordering  │
 │ • Safe Sequence Test │   │ • 3-Color DFS Search │   │ • Strict Rank Check  │
 └──────────┬───────────┘   └──────────┬───────────┘   └──────────┬───────────┘
            │ Safe Path                │ Deadlock Detected!       │ Rank Validated
            │                          ▼                          │
            │               ┌──────────────────────┐              │
            │               │ Cost Recovery Engine │              │
            │               │ • Penalty Metric C(P)│              │
            │               │ • Abort Optimal Vic  │              │
            │               └──────────┬───────────┘              │
            ▼                          │ Victim Aborted           ▼
 ┌─────────────────────────────────────┴──────────────────────────────────────┐
 │                      State Commit & Process Unblocking                     │
 │   • Resources Committed / Released  • Blocked Queue Woken                  │
 └────────────────────────────────────────────────────────────────────────────┘
```

---

## 🚀 Quick Start (30 Seconds)

### 1. Build & Run Tests
```bash
# Compile core engine & test suite
make

# Run all 24 unit tests & memory sanitizers
make test
make memcheck
```

### 2. Launch Interactive CLI Shell
```bash
./bin/deadlockdoctor --shell
```

```text
DeadlockDoctor> add_res LockA 1
DeadlockDoctor> add_res LockB 1
DeadlockDoctor> add_proc Worker1 1
DeadlockDoctor> add_proc Worker2 2

# Create circular wait deadlock
DeadlockDoctor> req 0 0 1
DeadlockDoctor> req 1 1 1
DeadlockDoctor> req 0 1 1
DeadlockDoctor> req 1 0 1

# Detect & break deadlock
DeadlockDoctor> show
DeadlockDoctor> graph
DeadlockDoctor> detect
DeadlockDoctor> recover
```

### 3. Launch Web GUI Dashboard
```bash
python gui/app.py
```
*Access interactive graphical topology at `http://localhost:5000`.*

---

## 🌟 Core Engine Capabilities

| Engine | Algorithm & Mathematical Model | Complexity | Key Feature |
|---|---|---|---|
| **Avoidance** | Dijkstra's Banker's Algorithm | $\mathcal{O}(P^2 \times R)$ | Safe execution sequence calculation `<P1 -> P3 -> P0>` |
| **Detection** | 3-Color DFS on Wait-For Graph (WFG) | $\mathcal{O}(V + E)$ | Sub-millisecond cycle back-edge identification ($<15\,\mu\text{s}$) |
| **Recovery** | Multi-Attribute Cost Penalty $C(P_i)$ | $\mathcal{O}(P)$ | Starvation-aware optimal victim abortion & preemption |
| **Prevention** | Havender's Linear Hierarchy | $\mathcal{O}(1)$ | Enforces strict ascending resource rank ordering |
| **Live Concurrency** | POSIX `pthread_mutex_t` Harness | Real-time | Watchdog intercepts real thread lock race in $<100\,\text{ms}$ |

<details>
<summary><b>📐 Algorithmic Mathematical Formulations (Click to expand)</b></summary>

### 1. Dijkstra's Banker's Safety Algorithm
$$\text{Need}[i][j] = \text{Max}[i][j] - \text{Allocation}[i][j]$$
$$\text{If } \text{Finish}[i] == \text{false} \land \text{Need}[i] \le \text{Work} \implies \text{Work} \leftarrow \text{Work} + \text{Allocation}[i], \quad \text{Finish}[i] \leftarrow \text{true}$$

### 2. Starvation-Aware Recovery Penalty Metric
$$C(P_i) = w_p \cdot \text{Priority}(P_i) + w_h \cdot \text{HeldResources}(P_i) + w_w \cdot \text{WorkCompleted}(P_i) - w_s \cdot \text{Starvation}(P_i)$$
$$\text{Victim} = \arg\min_{P \in \text{Deadlocked}} C(P)$$
</details>

---

## 📂 Project Architecture

```text
DeadLockDoctor/
├── assets/                  # High-resolution flowcharts, RAG/WFG topologies & benchmarks
├── bin/                     # Compiled binaries (deadlockdoctor, test_suite, test_asan)
├── docs/                    # Complete manuals, references, reports & presentation decks
│   ├── reports/             # Master Review-1 PPTX presentation & Word DOCX deliverables
│   ├── reference/           # Project specifications, syllabus & templates
│   ├── CLI_COMMANDS_REFERENCE.md
│   └── PBL_PROJECT_REVIEW_DOCUMENTATION.md
├── gui/                     # Web Dashboard (app.js, app.py, index.html, style.css)
├── include/                 # Modular C11 header interfaces (core, avoidance, detection, etc.)
├── scenarios/               # 6 Benchmark .dd scenario test scripts
├── scripts/                 # Python build utilities & diagram renderers
├── src/                     # Core C11 engine source code
├── tests/                   # 24/24 Unit test suite
├── .gitignore               # Clean repository tracking
├── Makefile                 # Build automation & AddressSanitizer audit
└── README.md                # Project showcase & documentation
```

---

## 🎓 Course Outcome (CO) Mapping

| Course Outcome | OS Syllabus Domain | Implemented Component & File |
|---|---|---|
| **CO-1** | OS Services & Shell Interface | Custom REPL shell with native OS command passthrough (`src/main.c`, `src/ui/visualizer.c`). |
| **CO-2** | Process Management & Scheduling | Process lifecycle states, priority queues, cost preemption (`src/core/resource_mgr.c`, `src/recovery/recovery.c`). |
| **CO-3** | Concurrency & Synchronization | Mutex lock race harness and watchdog timer (`src/demo/live_demo.c`). |
| **CO-4** | Memory Safety & Runtimes | Zero leak dynamic matrices verified under ASan (`Makefile`, `make memcheck`). |
| **CO-5** | File Subsystems & Scripting | `.dd` scenario lexer/parser and replay engine (`src/scenario/scenario.c`). |

---

## 📄 Deliverables & Reports

All formal documentation and presentation materials are neatly organized in `docs/reports/`:
- 📊 **[Master Presentation Deck (PPTX)](docs/reports/DeadlockDoctor_Review_Presentation.pptx)**
- 📝 **[PBL Review-1 Documentation (DOCX)](docs/reports/DeadlockDoctor_PBL_Review_Documentation.docx)**
- 📋 **[CLI Commands Cheat Sheet (DOCX)](docs/reports/DeadlockDoctor_Commands_CheatSheet.docx)**

---

## 📄 License
Academic PBL Capstone Project. Open for educational and research exploration.
