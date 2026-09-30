# DeadlockDoctor: CLI & Shell Command Manual
### Complete Command Reference, Syntax, Usage & Examples

This document serves as the complete operational guide for all command-line commands, flags, shell inputs, and scenario script directives in **DeadlockDoctor**.

---

## 1. Executable Launch Flags

When starting the binary from your Linux / WSL terminal (`./deadlockdoctor`), you can pass the following command-line arguments:

| Flag | Full Name | Description | Example |
|---|---|---|---|
| `-s` | `--shell` | Launches directly into the interactive DeadlockDoctor Shell (REPL). | `./deadlockdoctor --shell` |
| `-d` | `--demo` | Executes the real POSIX Multithreading Deadlock Race demonstration. | `./deadlockdoctor --demo` |
| `-w` | `--walkthrough` | Runs the step-by-step educational simulation walkthrough. | `./deadlockdoctor --walkthrough` |
| `-h` | `--help` | Displays the binary usage menu and options. | `./deadlockdoctor --help` |
| `<file>` | *Positional* | Loads and executes a `.dd` scenario script directly. | `./deadlockdoctor scenarios/01_bankers_safe.dd` |

---

## 2. Interactive Shell Commands (`DeadlockDoctor> `)

Once inside the interactive REPL shell (started via `./deadlockdoctor --shell`), you have access to internal simulation commands and native host OS commands.

### 2.1 System State & Inspection

#### `show` or `matrices`
* **Description:** Prints the current Allocation, Need (or Request), Max Claim, and Available resource vectors in formatted ANSI color tables.
* **Syntax:** `show`
* **Example Output:**
  ```text
   Process    | Allocation       | Request / Need   | Max Claim        | Prio / Cost  | Status    
   -----------+------------------+------------------+------------------+--------------+------------
   P0         | 0 1 0            | 7 4 3            | 7 5 3            | P:1  C:1.0    | RUNNING   
   P1         | 2 0 0            | 1 2 2            | 3 2 2            | P:2  C:2.0    | RUNNING   
  ```

#### `graph` or `graphs`
* **Description:** Renders the ASCII Resource Allocation Graph (RAG) and Wait-For Graph (WFG) showing process-resource and process-process dependency edges.
* **Syntax:** `graph`

#### `reset`
* **Description:** Resets the entire resource manager to an empty clean slate (0 processes, 0 resources).
* **Syntax:** `reset`

---

### 2.2 Resource & Process Configuration

#### `add_res <name> <instances>`
* **Description:** Registers a new resource type with a given name and total capacity.
* **Syntax:** `add_res <name_string> <total_units_int>`
* **Example:**
  ```text
  DeadlockDoctor> add_res GPU 4
  [✓] Registered resource 'GPU' (RID: 0, Instances: 4)
  ```

#### `add_proc <name> [priority]`
* **Description:** Registers a new process with an optional integer priority (higher number = higher priority).
* **Syntax:** `add_proc <name_string> [priority_int]`
* **Example:**
  ```text
  DeadlockDoctor> add_proc DB_Worker 5
  [✓] Registered process 'DB_Worker' (PID: 0, Priority: 5)
  ```

#### `set_max <p> <r> <k>`
* **Description:** Sets the maximum claim of Process `<p>` on Resource `<r>` to `<k>` units.
* **Syntax:** `set_max <process_id> <resource_id> <units>`
* **Example:**
  ```text
  DeadlockDoctor> set_max 0 0 3
  [✓] Set Max Claim for Process 0 on Resource 0 = 3
  ```

---

### 2.3 Resource Operations (Request & Release)

#### `req <p> <r> <k>`
* **Description:** Process `<p>` requests `<k>` units of Resource `<r>`.
  - In **Avoidance Mode**: Evaluates Banker's safety. Grants if safe; blocks if unsafe.
  - In **Detection Mode**: Grants immediately if available; blocks if unavailable.
* **Syntax:** `req <process_id> <resource_id> <units>`
* **Example:**
  ```text
  DeadlockDoctor> req 0 0 2
  [✓] Request GRANTED: Process 0 acquired 2 units of Resource 0.
  ```

#### `rel <p> <r> <k>`
* **Description:** Process `<p>` yields/releases `<k>` units of Resource `<r>` back to the available pool.
* **Syntax:** `rel <process_id> <resource_id> <units>`
* **Example:**
  ```text
  DeadlockDoctor> rel 0 0 2
  [✓] Process 0 released 2 units of Resource 0.
  ```

---

### 2.4 Diagnostic & Recovery Engines

#### `banker`
* **Description:** Runs Dijkstra's Banker's Algorithm to check if the current state is SAFE and derives a valid execution sequence.
* **Syntax:** `banker`
* **Example Output:**
  ```text
  [✓] STATE IS SAFE. Banker's algorithm found valid execution sequence:
      SAFE SEQUENCE:  < P1 ──► P3 ──► P0 ──► P2 ──► P4 >
  ```

#### `detect`
* **Description:** Executes 3-Color DFS cycle detection on the Wait-For Graph and matrix reduction to identify trapped processes and critical cycles.
* **Syntax:** `detect`

#### `recover`
* **Description:** Executes automated cost-based recovery. Evaluates the multi-attribute penalty metric, aborts the optimal victim, reclaims held resources, and unblocks waiting dependents.
* **Syntax:** `recover`

#### `mode <avoid|detect|prev>`
* **Description:** Switches the engine operating mode between Avoidance (Banker's), Detection & Recovery, or Prevention.
* **Syntax:** `mode avoid` / `mode detect` / `mode prev`

#### `demo`
* **Description:** Launches the real-time POSIX multithreading lock race within the shell session.
* **Syntax:** `demo`

#### `load <filepath>`
* **Description:** Loads and replays an external `.dd` scenario script.
* **Syntax:** `load scenarios/01_bankers_safe.dd`

---

### 2.5 Native Host OS Shell Passthrough

The DeadlockDoctor shell natively executes standard operating system commands directly in your subshell without requiring you to exit:

| OS Command | Description | Example |
|---|---|---|
| `pwd` | Print current working directory | `DeadlockDoctor> pwd` |
| `ls` or `dir` | List files in current directory | `DeadlockDoctor> ls -la` |
| `clear` or `cls` | Clears the terminal screen | `DeadlockDoctor> clear` |
| `cd <path>` | Changes working directory | `DeadlockDoctor> cd ..` |
| `cat <file>` | Prints file contents | `DeadlockDoctor> cat scenarios/01_bankers_safe.dd` |
| `whoami` | Shows current OS user | `DeadlockDoctor> whoami` |
| `exit` / `quit` | Exits the DeadlockDoctor Shell | `DeadlockDoctor> exit` |

---

## 3. Makefile Build Targets

From your terminal, you can execute the following `make` targets:

| Make Target | Action Performed |
|---|---|
| `make all` | Compiles the engine, creates directories, and builds `./bin/deadlockdoctor`. |
| `make test` | Builds and runs the 24-assertion C11 test suite. |
| `make memcheck` | Compiles with AddressSanitizer (`-fsanitize=address,undefined`) and tests for memory leaks. |
| `make demo` | Builds and directly launches the live multithreaded demo (`./deadlockdoctor --demo`). |
| `make clean` | Removes all compiled binaries, object files, and build artifacts. |

---

## 4. Web GUI Dashboard Commands

To launch the web dashboard:
```bash
python gui/app.py
```
* **URL:** `http://localhost:5000`
* **Features:**
  - **`🌓 Theme` Button:** Toggles between Dark Studio and Cream Studio theme.
  - **`⚡ Live Mutex Race` Button:** Triggers an animated 3-thread circular lock contention.
  - **`🔍 Banker's Check` Button:** Animates the safe sequence derivation.
  - **`💊 Cost Recovery` Button:** Executes instant victim selection and unblocks waiting nodes.
