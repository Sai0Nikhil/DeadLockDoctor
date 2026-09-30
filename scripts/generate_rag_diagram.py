#!/usr/bin/env python3
"""
DeadlockDoctor - Visual Graph Comparison: Bipartite RAG vs Wait-For Graph (WFG)
Generates high-resolution graphic for Deadlock Cycle Representation.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches

def generate_rag_wfg_diagram(output_path="C:/DeadDoctor/assets/rag_wfg_diagram.png"):
    fig = plt.figure(figsize=(14, 6), dpi=300)
    ax = fig.add_subplot(111)
    ax.set_xlim(0, 14)
    ax.set_ylim(0, 6)
    ax.axis('off')

    BG_COLOR = "#F8F6F0"
    fig.patch.set_facecolor(BG_COLOR)
    ax.set_facecolor(BG_COLOR)

    # Colors
    PROC_COLOR = "#1E293B"    # Slate 800 (Process Circles)
    RES_COLOR = "#0D9488"     # Teal 600 (Resource Squares)
    CYCLE_COLOR = "#DC2626"   # Red 600 (Deadlock Cycle Edges)
    SAFE_COLOR = "#2563EB"    # Blue 600 (Safe Edges)
    TEXT_MUTED = "#64748B"

    # Header
    ax.text(7.0, 5.6, "Deadlock Topology: Bipartite Resource Allocation Graph (RAG) vs Wait-For Graph (WFG)",
            ha='center', va='center', fontsize=12, fontweight='bold', color="#0F172A", family='sans-serif')

    # ==================== LEFT: RESOURCE ALLOCATION GRAPH (RAG) ====================
    ax.text(3.5, 4.9, "Bipartite RAG (Processes + Resources)",
            ha='center', va='center', fontsize=10.5, fontweight='bold', color="#0F172A", family='sans-serif')

    # Draw Processes (Circles)
    p0 = patches.Circle((2.0, 3.2), 0.55, facecolor="#EFF6FF", edgecolor=PROC_COLOR, lw=2, zorder=3)
    p1 = patches.Circle((5.0, 1.8), 0.55, facecolor="#EFF6FF", edgecolor=PROC_COLOR, lw=2, zorder=3)
    ax.add_patch(p0)
    ax.add_patch(p1)
    ax.text(2.0, 3.2, "P0", ha='center', va='center', fontsize=11, fontweight='bold', color=PROC_COLOR, zorder=4)
    ax.text(5.0, 1.8, "P1", ha='center', va='center', fontsize=11, fontweight='bold', color=PROC_COLOR, zorder=4)

    # Draw Resources (Squares)
    r0 = patches.Rectangle((4.4, 3.0), 1.1, 0.9, facecolor="#F0FDFA", edgecolor=RES_COLOR, lw=2, zorder=3)
    r1 = patches.Rectangle((1.5, 1.3), 1.1, 0.9, facecolor="#F0FDFA", edgecolor=RES_COLOR, lw=2, zorder=3)
    ax.add_patch(r0)
    ax.add_patch(r1)
    ax.text(4.95, 3.45, "LockA (R0)", ha='center', va='center', fontsize=9, fontweight='bold', color=RES_COLOR, zorder=4)
    ax.text(2.05, 1.75, "LockB (R1)", ha='center', va='center', fontsize=9, fontweight='bold', color=RES_COLOR, zorder=4)

    # Edges in RAG
    # P0 holds LockA (R0 -> P0)
    ax.annotate('', xy=(2.6, 3.2), xytext=(4.4, 3.4),
                arrowprops=dict(arrowstyle="-|>", color=CYCLE_COLOR, lw=2.2, mutation_scale=14), zorder=2)
    # P0 requests LockB (P0 -> R1)
    ax.annotate('', xy=(2.0, 2.25), xytext=(2.0, 2.65),
                arrowprops=dict(arrowstyle="-|>", color=CYCLE_COLOR, lw=2.2, mutation_scale=14), zorder=2)
    # P1 holds LockB (R1 -> P1)
    ax.annotate('', xy=(4.4, 1.8), xytext=(2.6, 1.75),
                arrowprops=dict(arrowstyle="-|>", color=CYCLE_COLOR, lw=2.2, mutation_scale=14), zorder=2)
    # P1 requests LockA (P1 -> R0)
    ax.annotate('', xy=(5.0, 3.0), xytext=(5.0, 2.35),
                arrowprops=dict(arrowstyle="-|>", color=CYCLE_COLOR, lw=2.2, mutation_scale=14), zorder=2)

    ax.text(3.5, 0.5, "Closed Bipartite Cycle: P0 -> R1 -> P1 -> R0 -> P0",
            ha='center', va='center', fontsize=9, color=CYCLE_COLOR, fontweight='bold')

    # ==================== DIVIDER LINE ====================
    ax.plot([7.0, 7.0], [0.8, 5.0], color="#CBD5E1", lw=1.5, linestyle="--")

    # ==================== RIGHT: WAIT-FOR GRAPH (WFG) ====================
    ax.text(10.5, 4.9, "Reduced Wait-For Graph (3-Color DFS)",
            ha='center', va='center', fontsize=10.5, fontweight='bold', color="#0F172A", family='sans-serif')

    w_p0 = patches.Circle((9.0, 2.5), 0.65, facecolor="#FEF2F2", edgecolor=CYCLE_COLOR, lw=2.5, zorder=3)
    w_p1 = patches.Circle((12.0, 2.5), 0.65, facecolor="#FEF2F2", edgecolor=CYCLE_COLOR, lw=2.5, zorder=3)
    ax.add_patch(w_p0)
    ax.add_patch(w_p1)
    ax.text(9.0, 2.5, "P0", ha='center', va='center', fontsize=12, fontweight='bold', color=CYCLE_COLOR, zorder=4)
    ax.text(12.0, 2.5, "P1", ha='center', va='center', fontsize=12, fontweight='bold', color=CYCLE_COLOR, zorder=4)

    # Circular wait directed edges with curve
    arc1 = patches.FancyArrowPatch((9.5, 2.8), (11.5, 2.8), connectionstyle="arc3,rad=0.35",
                                  arrowstyle="-|>", color=CYCLE_COLOR, lw=2.5, mutation_scale=16, zorder=2)
    arc2 = patches.FancyArrowPatch((11.5, 2.2), (9.5, 2.2), connectionstyle="arc3,rad=0.35",
                                  arrowstyle="-|>", color=CYCLE_COLOR, lw=2.5, mutation_scale=16, zorder=2)
    ax.add_patch(arc1)
    ax.add_patch(arc2)

    ax.text(10.5, 3.45, "P0 waits for P1 (LockB)", ha='center', va='center', fontsize=8.5, color=CYCLE_COLOR, fontweight='bold')
    ax.text(10.5, 1.55, "P1 waits for P0 (LockA)", ha='center', va='center', fontsize=8.5, color=CYCLE_COLOR, fontweight='bold')
    ax.text(10.5, 0.5, "Linear Cycle Search: O(V + E) -> Deadlock Trapped!",
            ha='center', va='center', fontsize=9, color=CYCLE_COLOR, fontweight='bold')

    plt.tight_layout()
    plt.savefig(output_path, dpi=300, facecolor=fig.get_facecolor(), edgecolor='none', bbox_inches='tight')
    plt.close()
    print(f"[OK] RAG vs WFG diagram generated cleanly at: {output_path}")

if __name__ == "__main__":
    generate_rag_wfg_diagram()
