#!/usr/bin/env python3
"""
DeadlockDoctor - High-Resolution System Architecture & Execution Flow Diagram Generator
Renders a clean, modern engineering pipeline diagram with zero overlaps.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches

def generate_flow_diagram(output_path="C:/DeadDoctor/assets/system_flow_diagram.png"):
    fig = plt.figure(figsize=(16, 9.5), dpi=300)
    ax = fig.add_subplot(111)
    ax.set_xlim(0, 16)
    ax.set_ylim(0, 9.5)
    ax.axis('off')

    # Color Palette - Professional Modern Editorial
    BG_COLOR = "#F8F6F0"       # Warm Cream / Alabaster
    CARD_BG = "#FFFFFF"        # White Card
    BORDER = "#CBD5E1"         # Slate Border
    PRIMARY = "#0F172A"        # Slate 900
    ACCENT_BLUE = "#1D4ED8"    # Blue 700
    ACCENT_RED = "#B91C1C"     # Red 700
    ACCENT_GREEN = "#15803D"   # Green 700
    ACCENT_AMBER = "#B45309"   # Amber 700
    ACCENT_PURPLE = "#6D28D9"  # Purple 700
    MUTED = "#475569"          # Slate 600

    fig.patch.set_facecolor(BG_COLOR)
    ax.set_facecolor(BG_COLOR)

    def draw_box(x, y, w, h, title, subtitle="", bg=CARD_BG, border=BORDER, title_color=PRIMARY, text_color=MUTED, lw=1.5, radius=0.15):
        box = patches.FancyBboxPatch((x, y), w, h, boxstyle=f"round,pad=0.03,rounding_size={radius}",
                                     facecolor=bg, edgecolor=border, linewidth=lw, zorder=2)
        ax.add_patch(box)
        if title:
            ax.text(x + w/2, y + h - 0.28, title, ha='center', va='center',
                    fontsize=10.5, fontweight='bold', color=title_color, zorder=3, family='sans-serif')
        if subtitle:
            ax.text(x + w/2, y + (h - 0.28)/2, subtitle, ha='center', va='center',
                    fontsize=8.5, color=text_color, zorder=3, family='sans-serif', multialignment='center')

    def draw_arrow(x1, y1, x2, y2, color=MUTED, lw=2, label=""):
        ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                    arrowprops=dict(arrowstyle="-|>", color=color, lw=lw, shrinkA=2, shrinkB=2,
                                    mutation_scale=14), zorder=4)
        if label:
            mid_x = (x1 + x2) / 2
            mid_y = (y1 + y2) / 2
            ax.text(mid_x, mid_y, label, ha='center', va='center', fontsize=8,
                    fontweight='bold', color=color, zorder=5,
                    bbox=dict(boxstyle="round,pad=0.2", facecolor=CARD_BG, edgecolor=color, lw=0.8, alpha=0.95))

    # --- TOP HEADER BANNER ---
    ax.text(8.0, 9.1, "DeadlockDoctor: System Architecture & Execution Pipeline",
            ha='center', va='center', fontsize=16, fontweight='bold', color=PRIMARY, family='sans-serif')
    ax.text(8.0, 8.75, "End-to-End Request Routing, Algorithmic Decision Engines, Detection & Recovery Subsystems",
            ha='center', va='center', fontsize=10, color=MUTED, family='sans-serif')

    # ==================== LAYER 1: USER INTERFACES (TOP) ====================
    draw_box(0.8, 7.3, 4.3, 1.15, "1. Interactive CLI Shell", "REPL Engine with Auto-complete,\nHistory & Native OS Passthrough",
             border=ACCENT_BLUE, title_color=ACCENT_BLUE, bg="#EFF6FF")
    draw_box(5.8, 7.3, 4.4, 1.15, "2. Scenario File Parser", "Direct Execution of .dd Scripts\n(Reproducible Benchmarking)",
             border=ACCENT_PURPLE, title_color=ACCENT_PURPLE, bg="#FAF5FF")
    draw_box(10.9, 7.3, 4.3, 1.15, "3. Web GUI Dashboard", "Real-Time SVG Graph Visualizer\n& Matrix Inspector (REST API)",
             border=ACCENT_GREEN, title_color=ACCENT_GREEN, bg="#F0FDF4")

    # Connect Layer 1 to Central Router
    draw_arrow(2.95, 7.3, 6.8, 6.4, color=ACCENT_BLUE, lw=2, label="CLI Input")
    draw_arrow(8.0, 7.3, 8.0, 6.4, color=ACCENT_PURPLE, lw=2, label=".dd Script")
    draw_arrow(13.05, 7.3, 9.2, 6.4, color=ACCENT_GREEN, lw=2, label="REST API")

    # ==================== LAYER 2: CORE DISPATCHER & STATE (MIDDLE) ====================
    draw_box(4.8, 5.2, 6.4, 1.2, "Central Engine & State Repository",
             "• Allocation, Need, Max, Available Matrices [Thread-Safe Mutex]\n• Dynamic Mode Switcher (Avoidance | Detection | Prevention)",
             border=PRIMARY, title_color=PRIMARY, bg=CARD_BG, lw=2)

    # Connect Central Router to 3 Algorithmic Branches
    draw_arrow(5.5, 5.2, 2.95, 4.4, color=ACCENT_BLUE, lw=2, label="Mode: Avoid")
    draw_arrow(8.0, 5.2, 8.0, 4.4, color=ACCENT_RED, lw=2, label="Mode: Detect")
    draw_arrow(10.5, 5.2, 13.05, 4.4, color=ACCENT_AMBER, lw=2, label="Mode: Prev")

    # ==================== LAYER 3: THE 3 ALGORITHMIC ENGINES ====================
    # Engine A: Avoidance
    draw_box(0.8, 3.1, 4.3, 1.3, "Banker's Avoidance Engine",
             "• Speculative Resource Grant\n• Safety Sequence Search: O(P²×R)\n• Safe State: Grant | Unsafe: Block",
             border=ACCENT_BLUE, title_color=ACCENT_BLUE, bg="#F8FAFC")

    # Engine B: Detection & Graph Engine
    draw_box(5.8, 3.1, 4.4, 1.3, "Cycle Detection Engine",
             "• Wait-For Graph (WFG) Builder\n• 3-Color DFS Back-Edge Search\n• Multi-Instance Matrix Reduction",
             border=ACCENT_RED, title_color=ACCENT_RED, bg="#FEF2F2")

    # Engine C: Prevention
    draw_box(10.9, 3.1, 4.3, 1.3, "Prevention Engine",
             "• Havender's Resource Ordering\n• Strict Ascending Rank Hierarchy\n• Hold-and-Wait Rule Enforcer",
             border=ACCENT_AMBER, title_color=ACCENT_AMBER, bg="#FFFBEB")

    # Connect Detection to Recovery
    draw_arrow(8.0, 3.1, 8.0, 2.5, color=ACCENT_RED, lw=2, label="Deadlock!")

    # ==================== LAYER 4: RECOVERY SUBSYSTEM ====================
    draw_box(5.8, 1.5, 4.4, 1.0, "Cost Recovery Engine",
             "• Penalty Metric: C(Pi) = wp·P + wh·H + ww·W - ws·S\n• Optimal Victim Abortion & Lock Preemption",
             border=ACCENT_RED, title_color=ACCENT_RED, bg="#FFF1F2", lw=2)

    # ==================== LAYER 5: EXECUTION STATE & VISUALIZATION (BOTTOM) ====================
    draw_box(0.8, 0.3, 6.5, 0.9, "State Commit & Process Unblocking",
             "• Resources Released/Allocated • Blocked Queues Awakened",
             border=ACCENT_GREEN, title_color=ACCENT_GREEN, bg="#F0FDF4")

    draw_box(8.7, 0.3, 6.5, 0.9, "Visual Topology & Execution Telemetry",
             "• ANSI Terminal Matrices • SVG Graphs • Audit Trail Logs",
             border=PRIMARY, title_color=PRIMARY, bg="#F8FAFC")

    # Connect Engines to State Commit
    draw_arrow(2.95, 3.1, 2.95, 1.2, color=ACCENT_GREEN, lw=2, label="Safe Path")
    draw_arrow(5.8, 2.0, 4.5, 1.2, color=ACCENT_GREEN, lw=2, label="Victim Aborted")
    draw_arrow(13.05, 3.1, 13.05, 1.2, color=PRIMARY, lw=2, label="Log Event")

    # Connect Commit to Telemetry
    draw_arrow(7.3, 0.75, 8.7, 0.75, color=PRIMARY, lw=1.5, label="Sync UI")

    plt.tight_layout()
    plt.savefig(output_path, dpi=300, facecolor=fig.get_facecolor(), edgecolor='none', bbox_inches='tight')
    plt.close()
    print(f"[OK] High-resolution flow diagram generated cleanly at: {output_path}")

if __name__ == "__main__":
    generate_flow_diagram()
