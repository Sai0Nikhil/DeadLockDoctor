#!/usr/bin/env python3
"""
DeadlockDoctor - Master Presentation Deck Generator (Review-1 Edition)
Builds an exceptional, high-impact 8-slide presentation with embedded flow diagrams,
algorithmic charts, rubric mapping, and benchmark graphics.
"""

import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.enum.shapes import MSO_SHAPE

def build_presentation():
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)

    # Professional Modern Editorial Palette
    CREAM_BG = RGBColor(248, 246, 240)       # #F8F6F0 Warm Pale Cream
    WHITE_CARD = RGBColor(255, 255, 255)     # #FFFFFF Pure Card
    BORDER_COLOR = RGBColor(226, 221, 212)   # #E2DDD4 Subtle Warm Border
    BORDER_DARK = RGBColor(203, 213, 225)    # #CBD5E1 Slate Border
    TEXT_MAIN = RGBColor(15, 23, 42)         # #0F172A Slate 900
    TEXT_MUTED = RGBColor(71, 85, 105)       # #475569 Slate 600
    
    # Accent Highlights
    TERRACOTTA = RGBColor(194, 76, 44)       # #C24C2C Terracotta Accent
    NAVY = RGBColor(20, 45, 78)              # #142D4E Rich Navy
    BLUE = RGBColor(29, 78, 216)             # #1D4ED8 Blue
    GREEN = RGBColor(21, 128, 61)            # #15803D Green
    AMBER = RGBColor(180, 83, 9)             # #B45309 Amber
    PURPLE = RGBColor(109, 40, 217)          # #6D28D9 Purple
    RED = RGBColor(185, 28, 28)              # #B91C1C Red

    blank_layout = prs.slide_layouts[6]

    def add_bg(slide):
        bg = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0), Inches(0), Inches(13.333), Inches(7.5))
        bg.fill.solid()
        bg.fill.fore_color.rgb = CREAM_BG
        bg.line.fill.background()
        return bg

    def add_header(slide, subtitle, title, badge_color=TERRACOTTA):
        # Category / Rubric Badge
        sub_box = slide.shapes.add_textbox(Inches(0.85), Inches(0.4), Inches(11.6), Inches(0.32))
        tf_s = sub_box.text_frame
        tf_s.word_wrap = True
        tf_s.margin_left = tf_s.margin_top = tf_s.margin_right = tf_s.margin_bottom = 0
        p_s = tf_s.paragraphs[0]
        p_s.text = subtitle.upper()
        p_s.font.size = Pt(10)
        p_s.font.bold = True
        p_s.font.color.rgb = badge_color

        # Main Title
        t_box = slide.shapes.add_textbox(Inches(0.85), Inches(0.72), Inches(11.6), Inches(0.6))
        tf_t = t_box.text_frame
        tf_t.word_wrap = True
        tf_t.margin_left = tf_t.margin_top = tf_t.margin_right = tf_t.margin_bottom = 0
        p_t = tf_t.paragraphs[0]
        p_t.text = title
        p_t.font.size = Pt(21)
        p_t.font.bold = True
        p_t.font.color.rgb = TEXT_MAIN

    def add_card(slide, left, top, width, height, title="", title_color=NAVY, bg_color=WHITE_CARD, border_color=BORDER_COLOR, border_width=1):
        card = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, left, top, width, height)
        card.fill.solid()
        card.fill.fore_color.rgb = bg_color
        card.line.color.rgb = border_color
        card.line.width = Pt(border_width)

        if title:
            tb = slide.shapes.add_textbox(left + Inches(0.22), top + Inches(0.18), width - Inches(0.44), Inches(0.38))
            tf = tb.text_frame
            tf.word_wrap = True
            tf.margin_left = tf.margin_top = tf.margin_right = tf.margin_bottom = 0
            p = tf.paragraphs[0]
            p.text = title
            p.font.size = Pt(12.5)
            p.font.bold = True
            p.font.color.rgb = title_color
        return card

    # ========================================================
    # SLIDE 1: TITLE SLIDE (MINIMAL EDITORIAL 2-COLUMN)
    # ========================================================
    s1 = prs.slides.add_slide(blank_layout)
    add_bg(s1)

    # Master Frame
    add_card(s1, Inches(0.8), Inches(0.75), Inches(11.733), Inches(6.0), border_color=BORDER_DARK)

    # Left Column: Typography & Team Details
    tb1 = s1.shapes.add_textbox(Inches(1.2), Inches(1.1), Inches(5.8), Inches(5.3))
    tf1 = tb1.text_frame
    tf1.word_wrap = True

    p = tf1.paragraphs[0]
    p.text = "OPERATING SYSTEMS CAPSTONE • REVIEW - 1 (CO-1 TO CO-5)"
    p.font.size = Pt(10)
    p.font.bold = True
    p.font.color.rgb = TERRACOTTA

    p = tf1.add_paragraph()
    p.text = "DeadlockDoctor"
    p.font.size = Pt(36)
    p.font.bold = True
    p.font.color.rgb = NAVY

    p = tf1.add_paragraph()
    p.text = "High-Performance Resource Allocation & Deadlock Management System"
    p.font.size = Pt(13.5)
    p.font.bold = True
    p.font.color.rgb = TEXT_MAIN

    p = tf1.add_paragraph()
    p.text = "Use Case #11: Banker's Avoidance, 3-Color DFS Detection, Starvation-Aware Cost Recovery & POSIX Concurrency Harness"
    p.font.size = Pt(10.5)
    p.font.color.rgb = TEXT_MUTED

    p = tf1.add_paragraph()
    p.text = "\nTeam Members (3 Students):\n• [Student 1 Name] ([Student 1 Roll No])\n• [Student 2 Name] ([Student 2 Roll No])\n• [Student 3 Name] ([Student 3 Roll No])"
    p.font.size = Pt(10.5)
    p.font.color.rgb = TEXT_MAIN

    p = tf1.add_paragraph()
    p.text = "\nDeliverables: C11 Core Engine • Interactive CLI Shell • HTML5 SVG Dashboard • Scenario Parser"
    p.font.size = Pt(9.5)
    p.font.color.rgb = BLUE

    # Right Column: Hero Graphic
    hero_path = "C:/DeadDoctor/assets/deadlockdoctor_hero.jpg"
    if os.path.exists(hero_path):
        s1.shapes.add_picture(hero_path, Inches(7.2), Inches(1.15), width=Inches(4.95))

    # ========================================================
    # SLIDE 2: PROBLEM STATEMENT & PROJECT OBJECTIVES
    # ========================================================
    s2 = prs.slides.add_slide(blank_layout)
    add_bg(s2)
    add_header(s2, "Section 1: Project Identification & Formulation (5 Marks)", "Problem Statement & Core Project Objectives", TERRACOTTA)

    # Left Card: Problem Statement
    add_card(s2, Inches(0.85), Inches(1.45), Inches(5.65), Inches(5.5), "The Concurrency Deadlock Crisis", RED, border_color=BORDER_DARK)
    tb = s2.shapes.add_textbox(Inches(1.1), Inches(2.0), Inches(5.15), Inches(4.7))
    tf = tb.text_frame
    tf.word_wrap = True

    p = tf.paragraphs[0]
    p.text = "Multi-threaded OS runtimes frequently freeze due to resource contention satisfying all 4 Coffman conditions simultaneously:"
    p.font.size = Pt(11)
    p.font.color.rgb = TEXT_MAIN

    coffman = [
        ("1. Mutual Exclusion:", "Resources (mutexes, GPU, disks) are non-shareable and held exclusively."),
        ("2. Hold and Wait:", "Processes hold allocated resources while actively waiting for others."),
        ("3. No Preemption:", "OS cannot forcibly reclaim mutexes without corrupting application state."),
        ("4. Circular Wait:", "Cyclic dependency chains leave threads trapped indefinitely.")
    ]
    for k, v in coffman:
        p = tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(10.5)
        p.font.color.rgb = TEXT_MUTED

    p = tf.add_paragraph()
    p.text = "\nTraditional Fallback Failures: Static timeouts and blind SIGKILL restarts cause severe data loss, wasted CPU compute, and cascading pipeline starvation."
    p.font.size = Pt(10)
    p.font.bold = True
    p.font.color.rgb = TERRACOTTA

    # Right Card: Project Objectives
    add_card(s2, Inches(6.8), Inches(1.45), Inches(5.68), Inches(5.5), "Core Objectives & Technical Scope", GREEN, border_color=BORDER_DARK)
    tb = s2.shapes.add_textbox(Inches(7.05), Inches(2.0), Inches(5.18), Inches(4.7))
    tf = tb.text_frame
    tf.word_wrap = True

    objs = [
        ("1. Complete Avoidance Engine:", "Implement Dijkstra's Banker's Algorithm with safe sequence verification in O(P²×R)."),
        ("2. Sub-Millisecond Detection:", "Build 3-Color DFS cycle search on Wait-For Graphs (WFG) in O(V+E) + multi-instance reduction."),
        ("3. Starvation-Aware Recovery:", "Formulate multi-attribute cost metric C(Pi) to abort optimal victims and release blocked threads."),
        ("4. Havender's Prevention:", "Enforce linear hierarchical ordering and hold-and-wait validation."),
        ("5. Real POSIX Watchdog:", "Intercept live multithreaded mutex deadlocks in <100ms with thread-safe telemetry."),
        ("6. Dual Diagnostic UI:", "Provide an interactive CLI shell with native OS passthrough + modern SVG web dashboard.")
    ]
    for i, (k, v) in enumerate(objs):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"{k} {v}"
        p.font.size = Pt(10.5)
        p.font.color.rgb = TEXT_MAIN

    # ========================================================
    # SLIDE 3: REQUIREMENTS SPECIFICATION
    # ========================================================
    s3 = prs.slides.add_slide(blank_layout)
    add_bg(s3)
    add_header(s3, "Section 2: Requirements Analysis (10 Marks)", "Functional & Non-Functional Engineering Specifications", NAVY)

    # Left Card: Functional Requirements
    add_card(s3, Inches(0.85), Inches(1.45), Inches(5.65), Inches(5.5), "Functional Requirements (FR-01 to FR-06)", BLUE, border_color=BORDER_DARK)
    tb = s3.shapes.add_textbox(Inches(1.1), Inches(2.0), Inches(5.15), Inches(4.7))
    tf = tb.text_frame
    tf.word_wrap = True

    frs = [
        ("FR-01 Dynamic State Matrices:", "Tracks Allocation, Need, Max Claim, and Available matrices up to 64 processes & 32 resources."),
        ("FR-02 Banker's Safety Engine:", "Simulates grants and derives valid execution sequences <P_i1, ..., P_in> before state commits."),
        ("FR-03 3-Color DFS Graph Traversal:", "Constructs bipartite RAG and reduced WFG to identify directed back-edges in O(V+E)."),
        ("FR-04 Vector Matrix Reduction:", "Identifies contention sets for multi-instance resource types."),
        ("FR-05 Cost-Based Recovery:", "Evaluates 4 victim selection policies (cheapest, lowest priority, fewest resources, starvation-safe)."),
        ("FR-06 Prevention Guard:", "Enforces Havender's linear ordering and pre-allocation protocols.")
    ]
    for i, (k, v) in enumerate(frs):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(10)
        p.font.color.rgb = TEXT_MAIN

    # Right Card: Non-Functional Requirements & Environment
    add_card(s3, Inches(6.8), Inches(1.45), Inches(5.68), Inches(5.5), "Non-Functional Requirements (NFR-01 to NFR-05)", AMBER, border_color=BORDER_DARK)
    tb = s3.shapes.add_textbox(Inches(7.05), Inches(2.0), Inches(5.18), Inches(4.7))
    tf = tb.text_frame
    tf.word_wrap = True

    nfrs = [
        ("NFR-01 Sub-Millisecond Response:", "Cycle detection and Banker's safety verification executes in <50 microseconds."),
        ("NFR-02 Zero Memory Leaks:", "100% clean memory verification under AddressSanitizer (ASan) & UBSan."),
        ("NFR-03 Thread-Safety & Concurrency:", "POSIX mutex synchronization protects all matrix state transitions."),
        ("NFR-04 Strict ISO C11 Standards:", "Built with GCC flags -std=c11 -Wall -Wextra -Wpedantic with 0 warnings/errors."),
        ("NFR-05 Extensible Scenario Parser:", "Deterministic replay of .dd benchmark scripts for regression testing."),
        ("I/O Architecture:", "ANSI colored CLI REPL + REST API JSON exporter for SVG web visualization.")
    ]
    for i, (k, v) in enumerate(nfrs):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(10)
        p.font.color.rgb = TEXT_MAIN

    # ========================================================
    # SLIDE 4: SYSTEM ARCHITECTURE & FLOW DIAGRAM (VISUAL)
    # ========================================================
    s4 = prs.slides.add_slide(blank_layout)
    add_bg(s4)
    add_header(s4, "Section 3: Design & Architecture (10 Marks)", "System Architecture & End-to-End Execution Pipeline Flowchart", PURPLE)

    # Embed High-Resolution Flow Diagram
    diag_path = "C:/DeadDoctor/assets/system_flow_diagram.png"
    if os.path.exists(diag_path):
        s4.shapes.add_picture(diag_path, Inches(0.85), Inches(1.4), width=Inches(11.633))

    # ========================================================
    # SLIDE 5: ALGORITHMS & MATHEMATICAL FORMULATIONS
    # ========================================================
    s5 = prs.slides.add_slide(blank_layout)
    add_bg(s5)
    add_header(s5, "Section 3: Algorithmic Rigor & Formulations", "Deadlock Graph Topologies & Mathematical Formulations", NAVY)

    # Embed RAG vs WFG Graphic on Top
    rag_path = "C:/DeadDoctor/assets/rag_wfg_diagram.png"
    if os.path.exists(rag_path):
        s5.shapes.add_picture(rag_path, Inches(0.85), Inches(1.35), width=Inches(11.633))

    # Bottom Left Card: Banker's & 3-Color DFS
    add_card(s5, Inches(0.85), Inches(4.3), Inches(5.65), Inches(2.7), "1. Banker's Algorithm & 3-Color DFS", BLUE, border_color=BORDER_DARK)
    tb = s5.shapes.add_textbox(Inches(1.05), Inches(4.7), Inches(5.25), Inches(2.1))
    tf = tb.text_frame
    tf.word_wrap = True

    b_text = [
        ("Banker's Safety Condition:", "Need[i][j] = Max[i][j] - Allocation[i][j]"),
        ("Work Vector Simulation:", "If Finish[i] == false & Need[i] <= Work => Work += Alloc[i]"),
        ("3-Color DFS Cycle Math:", "WHITE (unvisited) -> GRAY (active in stack) -> BLACK (visited). A directed edge to a GRAY node confirms a deadlock cycle in O(V + E).")
    ]
    for i, (k, v) in enumerate(b_text):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(9.5)
        p.font.color.rgb = TEXT_MAIN

    # Bottom Right Card: Starvation Cost Recovery
    add_card(s5, Inches(6.8), Inches(4.3), Inches(5.68), Inches(2.7), "2. Multi-Attribute Penalty Metric & Prevention", GREEN, border_color=BORDER_DARK)
    tb = s5.shapes.add_textbox(Inches(7.0), Inches(4.7), Inches(5.28), Inches(2.1))
    tf = tb.text_frame
    tf.word_wrap = True

    r_text = [
        ("Victim Cost Formula:", "C(Pi) = wp·Priority(Pi) + wh·Held(Pi) + ww·Work(Pi) - ws·Starve(Pi)"),
        ("Optimized Selection:", "Victim = argmin_{P ∈ Deadlocked} C(P) (lowest penalty score is aborted)."),
        ("Havender's Linear Ordering:", "Strict ascending resource rank f(Ri) < f(Rj) mathematically guarantees cycle impossibility.")
    ]
    for i, (k, v) in enumerate(r_text):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(9.5)
        p.font.color.rgb = TEXT_MAIN

    # ========================================================
    # SLIDE 6: COURSE OUTCOMES (CO) MAPPING & RUBRIC
    # ========================================================
    s6 = prs.slides.add_slide(blank_layout)
    add_bg(s6)
    add_header(s6, "Curriculum Alignment & Rubric Coverage", "Course Outcomes (CO-1 to CO-5) Implementation Evidence", GREEN)

    # Master Table for CO Mapping
    table_shape = s6.shapes.add_table(6, 4, Inches(0.85), Inches(1.45), Inches(11.633), Inches(5.4))
    table = table_shape.table
    table.columns[0].width = Inches(1.1)
    table.columns[1].width = Inches(3.2)
    table.columns[2].width = Inches(4.633)
    table.columns[3].width = Inches(2.7)

    headers = ["Course Outcome", "OS Domain & Syllabus Focus", "DeadlockDoctor Implementation Evidence", "Code Files & Evidence"]
    for col_idx, h in enumerate(headers):
        cell = table.cell(0, col_idx)
        cell.fill.solid()
        cell.fill.fore_color.rgb = NAVY
        p = cell.text_frame.paragraphs[0]
        p.text = h
        p.font.size = Pt(10.5)
        p.font.bold = True
        p.font.color.rgb = RGBColor(255, 255, 255)

    co_data = [
        ("CO-1", "OS Services, System Calls & Shell Interface", "Custom interactive REPL shell with command dispatching, help system, ANSI table visualizer, and seamless native OS passthrough (pwd, ls, cd, cat).", "src/main.c\nsrc/ui/visualizer.c"),
        ("CO-2", "Process Management, States & Scheduling", "Process lifecycle control (RUNNING, BLOCKED, TERMINATED), priority queues, state vector updates, and cost-based preemption/abort.", "src/core/resource_mgr.c\nsrc/recovery/recovery.c"),
        ("CO-3", "Concurrency, Synchronization & Mutexes", "Thread-safe matrix access with POSIX mutex locks, multithreaded lock race simulation harness, and asynchronous watchdog timer.", "src/demo/live_demo.c\ninclude/demo/live_demo.h"),
        ("CO-4", "Memory Management & Runtime Safety", "Dynamic 2D/1D matrix allocations, zero leaks verified with AddressSanitizer & UndefinedBehaviorSanitizer (-fsanitize=address,undefined).", "src/core/resource_mgr.c\nMakefile (make memcheck)"),
        ("CO-5", "File Subsystems & Scenario Scripting", "Deterministic .dd scenario script parser with lexer/tokenizer for reproducible deadlock benchmarks, state dumps, and audit logs.", "src/scenario/scenario.c\nscenarios/*.dd")
    ]

    for row_idx, row in enumerate(co_data, start=1):
        bg = WHITE_CARD if row_idx % 2 != 0 else RGBColor(241, 245, 249)
        for col_idx, text in enumerate(row):
            cell = table.cell(row_idx, col_idx)
            cell.fill.solid()
            cell.fill.fore_color.rgb = bg
            p = cell.text_frame.paragraphs[0]
            p.text = text
            p.font.size = Pt(9.5)
            p.font.color.rgb = TEXT_MAIN
            if col_idx == 0:
                p.font.bold = True
                p.font.color.rgb = TERRACOTTA

    # ========================================================
    # SLIDE 7: TEST VERIFICATION & BENCHMARKS
    # ========================================================
    s7 = prs.slides.add_slide(blank_layout)
    add_bg(s7)
    add_header(s7, "Section 4: Verification & Benchmarking", "Unit Test Suite, Performance Latency & Memory Audit", BLUE)

    # Embed Benchmark Chart on Left
    bench_path = "C:/DeadDoctor/assets/benchmark_chart.png"
    if os.path.exists(bench_path):
        s7.shapes.add_picture(bench_path, Inches(0.85), Inches(1.4), width=Inches(7.2))

    # Right Column: ASan & Scenarios Summary Cards
    add_card(s7, Inches(8.25), Inches(1.4), Inches(4.233), Inches(2.6), "Memory Audit & ASan Verification", GREEN, border_color=BORDER_DARK)
    tb = s7.shapes.add_textbox(Inches(8.45), Inches(1.85), Inches(3.833), Inches(2.0))
    tf = tb.text_frame
    tf.word_wrap = True

    asan_items = [
        ("• Leak Sanitizer:", "0 bytes leaked in 0 allocations."),
        ("• AddressSanitizer:", "0 heap buffer overruns / use-after-free."),
        ("• UndefinedBehavior:", "0 integer overflows / null dereferences."),
        ("• Valgrind Target:", "All heap memory freed on shell exit.")
    ]
    for i, (k, v) in enumerate(asan_items):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"{k} {v}"
        p.font.size = Pt(9.5)
        p.font.color.rgb = TEXT_MAIN

    add_card(s7, Inches(8.25), Inches(4.2), Inches(4.233), Inches(2.75), "Automated Scenario Matrix", PURPLE, border_color=BORDER_DARK)
    tb = s7.shapes.add_textbox(Inches(8.45), Inches(4.65), Inches(3.833), Inches(2.1))
    tf = tb.text_frame
    tf.word_wrap = True

    scenarios = [
        ("• 01_bankers_safe.dd:", "Safe sequence derived [PASS]"),
        ("• 02_bankers_unsafe.dd:", "Unsafe request blocked [PASS]"),
        ("• 03_single_instance_cycle.dd:", "Cycle detected & resolved [PASS]"),
        ("• 04_multi_instance_deadlock.dd:", "Vector reduction [PASS]"),
        ("• 05_starvation_recovery.dd:", "Penalty protection [PASS]"),
        ("• 06_complex_mesh.dd:", "Dense graph resolution [PASS]")
    ]
    for i, (k, v) in enumerate(scenarios):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"{k} {v}"
        p.font.size = Pt(9)
        p.font.color.rgb = TEXT_MAIN

    # ========================================================
    # SLIDE 8: LIVE DEMO & CAPSTONE WRAP-UP
    # ========================================================
    s8 = prs.slides.add_slide(blank_layout)
    add_bg(s8)
    add_header(s8, "Section 5: Demonstration & Deliverables", "Live Execution Suite, Interactive Shell & Review Deliverables", TERRACOTTA)

    # 3 Column Showcase Cards
    card_w = Inches(3.68)
    gap = Inches(0.28)

    # Col 1: CLI Shell
    c1_left = Inches(0.85)
    add_card(s8, c1_left, Inches(1.45), card_w, Inches(4.4), "1. Interactive CLI REPL", BLUE, border_color=BORDER_DARK)
    tb = s8.shapes.add_textbox(c1_left + Inches(0.2), Inches(1.95), card_w - Inches(0.4), Inches(3.7))
    tf = tb.text_frame
    tf.word_wrap = True
    cli_cmds = [
        ("show / matrices:", "Inspect Allocation, Need, Max & Available tables."),
        ("req <p> <r> <k>:", "Request resource units with dynamic safety check."),
        ("detect:", "Run 3-Color DFS on WFG and print cycle path."),
        ("recover:", "Execute cost penalty victim selection & abort."),
        ("banker:", "Calculate Banker's safe execution sequence."),
        ("mode <avoid|detect|prev>:", "Switch engine operating mode."),
        ("OS Passthrough:", "Execute pwd, ls, cd, cat seamlessly.")
    ]
    for i, (k, v) in enumerate(cli_cmds):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(9)
        p.font.color.rgb = TEXT_MAIN

    # Col 2: Web GUI
    c2_left = c1_left + card_w + gap
    add_card(s8, c2_left, Inches(1.45), card_w, Inches(4.4), "2. HTML5 Web Dashboard", GREEN, border_color=BORDER_DARK)
    tb = s8.shapes.add_textbox(c2_left + Inches(0.2), Inches(1.95), card_w - Inches(0.4), Inches(3.7))
    tf = tb.text_frame
    tf.word_wrap = True
    gui_features = [
        ("Visual Graph Canvas:", "SVG rendering of Bipartite RAG and Wait-For Graph (WFG)."),
        ("Live Matrix Inspector:", "Real-time updates of Allocation & Available chips."),
        ("Theme Switcher:", "Toggle between Dark Studio & Warm Cream palettes."),
        ("REST API Backend:", "Python HTTP micro-server bridging C state to browser."),
        ("Interactive Triggers:", "One-click safe/unsafe request dispatching.")
    ]
    for i, (k, v) in enumerate(gui_features):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(9.5)
        p.font.color.rgb = TEXT_MAIN

    # Col 3: Live Mutex Harness & Review Wrap-up
    c3_left = c2_left + card_w + gap
    add_card(s8, c3_left, Inches(1.45), card_w, Inches(4.4), "3. Real Pthread Deadlock Race", RED, border_color=BORDER_DARK)
    tb = s8.shapes.add_textbox(c3_left + Inches(0.2), Inches(1.95), card_w - Inches(0.4), Inches(3.7))
    tf = tb.text_frame
    tf.word_wrap = True
    demo_points = [
        ("POSIX Concurrency:", "Spawns 2 real threads locking pthread_mutex_t in reverse order."),
        ("Lock Race Creation:", "Worker1 holds MutexA -> wants MutexB; Worker2 holds MutexB -> wants MutexA."),
        ("Asynchronous Watchdog:", "Detects blocked state within 100ms and breaks deadlock safely."),
        ("Review-1 Outcome:", "100% of Review-1 milestones met across CO-1 through CO-5.")
    ]
    for i, (k, v) in enumerate(demo_points):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"• {k} {v}"
        p.font.size = Pt(9.5)
        p.font.color.rgb = TEXT_MAIN

    # Bottom Summary Banner
    add_card(s8, Inches(0.85), Inches(6.0), Inches(11.633), Inches(0.95), bg_color=NAVY, border_color=NAVY)
    tb = s8.shapes.add_textbox(Inches(1.1), Inches(6.1), Inches(11.133), Inches(0.75))
    tf = tb.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "DeadlockDoctor: Production-Grade Resource & Deadlock Management Engine • Fully Verified for Review - 1"
    p.font.size = Pt(11)
    p.font.bold = True
    p.font.color.rgb = RGBColor(255, 255, 255)

    p = tf.add_paragraph()
    p.text = "Summary: 24/24 Unit Tests Passed • 0 Memory Leaks (ASan) • Sub-millisecond Execution • Complete C11 + Web GUI Suite"
    p.font.size = Pt(9.5)
    p.font.color.rgb = RGBColor(226, 232, 240)

    # Save Presentation
    output_pptx = "C:/DeadDoctor/DeadlockDoctor_Review_Presentation.pptx"
    prs.save(output_pptx)
    print(f"[OK] Master Presentation successfully built at: {output_pptx}")

if __name__ == "__main__":
    build_presentation()
