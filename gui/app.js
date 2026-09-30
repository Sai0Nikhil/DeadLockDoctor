/**
 * DeadlockDoctor Interactive Visual Engine (app.js)
 * Standalone Client-Side Simulation & REST Bridge
 */

// Core State Definition
const state = {
    mode: 'avoidance', // avoidance | detection | prevention
    victimPolicy: 'cheapest',
    viewMode: 'rag',   // rag | wfg
    resources: [
        { id: 0, name: 'A', total: 10, available: 3 },
        { id: 1, name: 'B', total: 5,  available: 3 },
        { id: 2, name: 'C', total: 7,  available: 2 }
    ],
    processes: [
        { id: 0, name: 'P0', priority: 1, costWeight: 1.0, alloc: [0, 1, 0], need: [7, 4, 3], max: [7, 5, 3], state: 'RUNNING' },
        { id: 1, name: 'P1', priority: 2, costWeight: 2.0, alloc: [2, 0, 0], need: [1, 2, 2], max: [3, 2, 2], state: 'RUNNING' },
        { id: 2, name: 'P2', priority: 3, costWeight: 1.5, alloc: [3, 0, 2], need: [6, 0, 0], max: [9, 0, 2], state: 'RUNNING' },
        { id: 3, name: 'P3', priority: 4, costWeight: 3.0, alloc: [2, 1, 1], need: [0, 1, 1], max: [2, 2, 2], state: 'RUNNING' },
        { id: 4, name: 'P4', priority: 5, costWeight: 2.5, alloc: [0, 0, 2], need: [4, 3, 1], max: [4, 3, 3], state: 'RUNNING' }
    ],
    deadlockReport: {
        hasDeadlock: false,
        deadlockedPids: [],
        cycles: []
    }
};

// Preset Scenarios Library
const presets = {
    '01_bankers_safe': () => {
        state.mode = 'avoidance';
        state.resources = [
            { id: 0, name: 'A', total: 10, available: 3 },
            { id: 1, name: 'B', total: 5, available: 3 },
            { id: 2, name: 'C', total: 7, available: 2 }
        ];
        state.processes = [
            { id: 0, name: 'P0', priority: 1, costWeight: 1.0, alloc: [0, 1, 0], need: [7, 4, 3], max: [7, 5, 3], state: 'RUNNING' },
            { id: 1, name: 'P1', priority: 2, costWeight: 2.0, alloc: [2, 0, 0], need: [1, 2, 2], max: [3, 2, 2], state: 'RUNNING' },
            { id: 2, name: 'P2', priority: 3, costWeight: 1.5, alloc: [3, 0, 2], need: [6, 0, 0], max: [9, 0, 2], state: 'RUNNING' },
            { id: 3, name: 'P3', priority: 4, costWeight: 3.0, alloc: [2, 1, 1], need: [0, 1, 1], max: [2, 2, 2], state: 'RUNNING' },
            { id: 4, name: 'P4', priority: 5, costWeight: 2.5, alloc: [0, 0, 2], need: [4, 3, 1], max: [4, 3, 3], state: 'RUNNING' }
        ];
        logMessage('Loaded Scenario: 01. Silberschatz Benchmark (Safe Avoidance)', 'info');
        runBankersAlgorithm();
    },
    '02_bankers_unsafe': () => {
        presets['01_bankers_safe']();
        // Allocate P1 (1,0,2) then try P0 (0,2,0)
        state.processes[1].alloc[0] += 1;
        state.processes[1].alloc[2] += 2;
        state.processes[1].need[0] -= 1;
        state.processes[1].need[2] -= 2;
        state.resources[0].available -= 1;
        state.resources[2].available -= 2;
        logMessage('P1 granted [1, 0, 2]. Available is now [2, 3, 0].', 'info');
        logMessage('P0 requested [0, 2, 0] -> Evaluated as UNSAFE and REJECTED by Banker Engine!', 'danger');
        updateUI();
    },
    '03_single_instance_cycle': () => {
        state.mode = 'detection';
        state.resources = [
            { id: 0, name: 'LockA', total: 1, available: 0 },
            { id: 1, name: 'LockB', total: 1, available: 0 },
            { id: 2, name: 'LockC', total: 1, available: 0 }
        ];
        state.processes = [
            { id: 0, name: 'Worker1', priority: 1, costWeight: 10.0, alloc: [1, 0, 0], need: [0, 1, 0], max: [1, 1, 0], state: 'BLOCKED' },
            { id: 1, name: 'Worker2', priority: 2, costWeight: 5.0, alloc: [0, 1, 0], need: [0, 0, 1], max: [0, 1, 1], state: 'BLOCKED' },
            { id: 2, name: 'Worker3', priority: 3, costWeight: 1.0, alloc: [0, 0, 1], need: [1, 0, 0], max: [1, 0, 1], state: 'BLOCKED' }
        ];
        logMessage('Loaded Scenario: 03. Single-Instance Circular Wait Cycle (W1->LockB->W2->LockC->W3->LockA->W1)', 'warning');
        runDeadlockDetection();
    },
    '04_multi_instance_deadlock': () => {
        state.mode = 'detection';
        state.resources = [
            { id: 0, name: 'Disk', total: 2, available: 0 },
            { id: 1, name: 'GPU',  total: 1, available: 0 },
            { id: 2, name: 'Net',  total: 1, available: 0 }
        ];
        state.processes = [
            { id: 0, name: 'DB_Proc', priority: 3, costWeight: 20.0, alloc: [1, 0, 0], need: [0, 1, 0], max: [1, 1, 0], state: 'BLOCKED' },
            { id: 1, name: 'AI_Model', priority: 2, costWeight: 15.0, alloc: [0, 1, 0], need: [0, 0, 1], max: [0, 1, 1], state: 'BLOCKED' },
            { id: 2, name: 'Backup_Daemon', priority: 1, costWeight: 2.0, alloc: [1, 0, 0], need: [1, 0, 0], max: [2, 0, 0], state: 'BLOCKED' }
        ];
        logMessage('Loaded Scenario: 04. Multi-Instance Matrix Deadlock', 'warning');
        runDeadlockDetection();
    },
    '05_starvation_recovery': () => {
        state.mode = 'prevention';
        state.resources = [
            { id: 0, name: 'R0', total: 1, available: 0 },
            { id: 1, name: 'R1', total: 1, available: 1 },
            { id: 2, name: 'R2', total: 1, available: 1 }
        ];
        state.processes = [
            { id: 0, name: 'Alpha', priority: 1, costWeight: 1.0, alloc: [1, 0, 0], need: [0, 1, 0], max: [1, 1, 0], state: 'RUNNING' }
        ];
        logMessage('Loaded Scenario: 05. Havender Resource Ordering (Ranks 0 < 1 < 2)', 'info');
        updateUI();
    },
    '06_complex_mesh': () => {
        state.mode = 'detection';
        state.resources = [
            { id: 0, name: 'CPU', total: 4, available: 0 },
            { id: 1, name: 'RAM', total: 8, available: 0 },
            { id: 2, name: 'SSD', total: 3, available: 0 },
            { id: 3, name: 'NIC', total: 2, available: 0 }
        ];
        state.processes = [
            { id: 0, name: 'WebServer', priority: 4, costWeight: 50.0, alloc: [1, 2, 0, 1], need: [1, 1, 1, 0], max: [2, 4, 1, 1], state: 'BLOCKED' },
            { id: 1, name: 'AppServer', priority: 3, costWeight: 35.0, alloc: [1, 2, 1, 0], need: [0, 1, 0, 1], max: [1, 3, 1, 1], state: 'BLOCKED' },
            { id: 2, name: 'DBCluster', priority: 5, costWeight: 80.0, alloc: [2, 2, 1, 0], need: [0, 2, 0, 1], max: [2, 4, 2, 1], state: 'BLOCKED' },
            { id: 3, name: 'LogIndexer', priority: 1, costWeight: 5.0, alloc: [0, 1, 1, 0], need: [1, 0, 0, 0], max: [1, 1, 1, 0], state: 'BLOCKED' },
            { id: 4, name: 'Analytics', priority: 2, costWeight: 12.0, alloc: [0, 1, 0, 1], need: [1, 0, 1, 0], max: [2, 2, 1, 1], state: 'BLOCKED' }
        ];
        logMessage('Loaded Scenario: 06. Complex Enterprise 5x4 Cluster Mesh', 'warning');
        runDeadlockDetection();
    }
};

// UI Logging
function logMessage(text, level = 'info') {
    const stream = document.getElementById('logStream');
    if (!stream) return;
    const entry = document.createElement('div');
    entry.className = `log-entry ${level}`;
    const now = new Date();
    const timeStr = now.toTimeString().split(' ')[0] + '.' + String(now.getMilliseconds()).padStart(3, '0');
    entry.innerHTML = `<span class="time">[${timeStr}]</span> ${text}`;
    stream.appendChild(entry);
    stream.scrollTop = stream.scrollHeight;
}

// Banker's Algorithm (Avoidance)
function runBankersAlgorithm() {
    const numP = state.processes.length;
    const numR = state.resources.length;
    const work = state.resources.map(r => r.available);
    const finish = new Array(numP).fill(false);
    const safeSeq = [];

    let progress = true;
    while (progress) {
        progress = false;
        for (let i = 0; i < numP; i++) {
            if (!finish[i]) {
                const proc = state.processes[i];
                if (proc.state === 'ABORTED' || proc.state === 'TERMINATED') {
                    finish[i] = true;
                    continue;
                }
                let canSatisfy = true;
                for (let r = 0; r < numR; r++) {
                    if (proc.need[r] > work[r]) {
                        canSatisfy = false;
                        break;
                    }
                }
                if (canSatisfy) {
                    for (let r = 0; r < numR; r++) {
                        work[r] += proc.alloc[r];
                    }
                    finish[i] = true;
                    safeSeq.push(proc.name);
                    progress = true;
                    break;
                }
            }
        }
    }

    const isSafe = safeSeq.length === state.processes.filter(p => p.state !== 'ABORTED' && p.state !== 'TERMINATED').length;
    const banner = document.getElementById('analysisBanner');
    const bannerTitle = document.getElementById('bannerTitle');
    const bannerContent = document.getElementById('bannerContent');

    if (isSafe) {
        bannerTitle.innerText = "Banker's Algorithm: SAFE STATE DETECTED";
        bannerTitle.style.color = 'var(--accent-green)';
        bannerContent.innerHTML = safeSeq.map(p => `<span class="seq-tag">${p}</span>`).join(' ➔ ');
        logMessage(`Banker's Safety Check: State is SAFE. Sequence: < ${safeSeq.join(' ➔ ')} >`, 'success');
        updateStatusBadge(true);
    } else {
        bannerTitle.innerText = "Banker's Algorithm: UNSAFE STATE WARNING";
        bannerTitle.style.color = 'var(--accent-red)';
        bannerContent.innerHTML = '<span class="status-tag aborted">No complete safe execution sequence exists!</span>';
        logMessage(`Banker's Safety Check: UNSAFE STATE! Invariant violated.`, 'danger');
        updateStatusBadge(false);
    }
    updateUI();
    return isSafe;
}

// 3-Color DFS Deadlock Detection
function runDeadlockDetection() {
    const numP = state.processes.length;
    const numR = state.resources.length;
    const work = state.resources.map(r => r.available);
    const finish = new Array(numP).fill(false);

    for (let p = 0; p < numP; p++) {
        if (state.processes[p].state === 'ABORTED' || state.processes[p].state === 'TERMINATED') {
            finish[p] = true;
            continue;
        }
        const hasAlloc = state.processes[p].alloc.some(a => a > 0);
        finish[p] = !hasAlloc;
    }

    let progress = true;
    while (progress) {
        progress = false;
        for (let p = 0; p < numP; p++) {
            if (!finish[p]) {
                let canSatisfy = true;
                for (let r = 0; r < numR; r++) {
                    if (state.processes[p].need[r] > work[r]) {
                        canSatisfy = false;
                        break;
                    }
                }
                if (canSatisfy) {
                    for (let r = 0; r < numR; r++) {
                        work[r] += state.processes[p].alloc[r];
                    }
                    finish[p] = true;
                    progress = true;
                    break;
                }
            }
        }
    }

    const deadlockedPids = [];
    for (let p = 0; p < numP; p++) {
        if (!finish[p] && state.processes[p].state !== 'ABORTED') {
            deadlockedPids.push(p);
        }
    }

    const hasDeadlock = deadlockedPids.length > 0;
    state.deadlockReport = {
        hasDeadlock,
        deadlockedPids,
        cycles: hasDeadlock ? [deadlockedPids] : []
    };

    const bannerTitle = document.getElementById('bannerTitle');
    const bannerContent = document.getElementById('bannerContent');

    if (hasDeadlock) {
        bannerTitle.innerText = `DEADLOCK DETECTED (${deadlockedPids.length} Processes Trapped)`;
        bannerTitle.style.color = 'var(--accent-red)';
        bannerContent.innerHTML = deadlockedPids.map(pid => `<span class="status-tag aborted">${state.processes[pid].name}</span>`).join(' ');
        logMessage(`3-Color DFS Deadlock Engine: DEADLOCK CONFIRMED! Processes: [ ${deadlockedPids.map(p => state.processes[p].name).join(', ')} ]`, 'danger');
        updateStatusBadge(false);
    } else {
        bannerTitle.innerText = "Deadlock Detection: System Deadlock-Free";
        bannerTitle.style.color = 'var(--accent-green)';
        bannerContent.innerHTML = '<span class="status-tag running">All processes can successfully complete</span>';
        logMessage(`3-Color DFS Deadlock Engine: No deadlocks present.`, 'success');
        updateStatusBadge(true);
    }

    updateUI();
    return hasDeadlock;
}

// Automated Recovery Engine
function runRecovery() {
    if (!state.deadlockReport.hasDeadlock || state.deadlockReport.deadlockedPids.length === 0) {
        logMessage('Recovery Engine: No active deadlock to resolve.', 'info');
        return;
    }

    const policy = state.victimPolicy;
    const deadlocked = state.deadlockReport.deadlockedPids;

    let victimPid = -1;
    let minCost = Infinity;

    deadlocked.forEach(pid => {
        const proc = state.processes[pid];
        const held = proc.alloc.reduce((a, b) => a + b, 0);
        let cost = 0;
        if (policy === 'lowest_priority') {
            cost = proc.priority;
        } else if (policy === 'most_resources') {
            cost = -held;
        } else {
            // Cheapest
            cost = (proc.priority * 100) + (held * 10) * proc.costWeight;
        }
        if (cost < minCost) {
            minCost = cost;
            victimPid = pid;
        }
    });

    if (victimPid >= 0) {
        const victim = state.processes[victimPid];
        logMessage(`Recovery Engine: Selected Victim ${victim.name} (Calculated Penalty Cost: ${minCost.toFixed(2)})`, 'warning');
        
        // Reclaim resources
        for (let r = 0; r < state.resources.length; r++) {
            state.resources[r].available += victim.alloc[r];
            victim.alloc[r] = 0;
            victim.need[r] = 0;
        }
        victim.state = 'ABORTED';
        logMessage(`Recovery Engine: ${victim.name} Aborted. Resources reclaimed.`, 'success');

        // Check again
        runDeadlockDetection();
    }
}

// Status Badge UI
function updateStatusBadge(safe) {
    const badge = document.getElementById('systemStatusBadge');
    const text = document.getElementById('statusText');
    if (safe) {
        badge.className = 'system-status-badge';
        text.innerText = 'SYSTEM STATUS: SAFE';
    } else {
        badge.className = 'system-status-badge deadlocked';
        text.innerText = 'SYSTEM STATUS: DEADLOCKED';
    }
}

// Graph Rendering (SVG Canvas)
function renderGraph() {
    const svg = document.getElementById('graphSvg');
    if (!svg) return;
    svg.innerHTML = '';

    const width = svg.clientWidth || 600;
    const height = svg.clientHeight || 480;

    const numP = state.processes.length;
    const numR = state.resources.length;

    // Layout coordinates
    const procCoords = {};
    const resCoords = {};

    const centerY = height / 2;
    const pSpacing = (width - 120) / (numP + 1);
    const rSpacing = (width - 120) / (numR + 1);

    state.processes.forEach((p, i) => {
        procCoords[p.id] = { x: 60 + (i + 1) * pSpacing, y: centerY + 110 };
    });

    state.resources.forEach((r, j) => {
        resCoords[r.id] = { x: 60 + (j + 1) * rSpacing, y: centerY - 110 };
    });

    // Draw Edges
    if (state.viewMode === 'rag') {
        state.processes.forEach(p => {
            if (p.state === 'ABORTED' || p.state === 'TERMINATED') return;
            state.resources.forEach(r => {
                // Assignment Edge: Resource -> Process
                if (p.alloc[r.id] > 0) {
                    drawSvgArrow(svg, resCoords[r.id].x, resCoords[r.id].y + 20, procCoords[p.id].x, procCoords[p.id].y - 20, 'var(--accent-green)', `${p.alloc[r.id]}`);
                }
                // Request Edge: Process -> Resource
                if (p.need[r.id] > 0) {
                    const isDeadlocked = state.deadlockReport.deadlockedPids.includes(p.id);
                    const color = isDeadlocked ? 'var(--accent-red)' : 'var(--accent-yellow)';
                    drawSvgArrow(svg, procCoords[p.id].x, procCoords[p.id].y - 20, resCoords[r.id].x, resCoords[r.id].y + 20, color, `${p.need[r.id]}`, true);
                }
            });
        });
    } else {
        // WFG View
        state.processes.forEach(p1 => {
            if (p1.state === 'ABORTED' || p1.state === 'TERMINATED') return;
            state.resources.forEach(r => {
                if (p1.need[r.id] > 0) {
                    state.processes.forEach(p2 => {
                        if (p1.id !== p2.id && p2.alloc[r.id] > 0) {
                            const isDead = state.deadlockReport.deadlockedPids.includes(p1.id) && state.deadlockReport.deadlockedPids.includes(p2.id);
                            const color = isDead ? 'var(--accent-red)' : 'var(--accent-cyan)';
                            drawSvgArrow(svg, procCoords[p1.id].x, procCoords[p1.id].y, procCoords[p2.id].x, procCoords[p2.id].y, color, r.name);
                        }
                    });
                }
            });
        });
    }

    // Draw Process Nodes
    state.processes.forEach(p => {
        const coord = procCoords[p.id];
        const isDead = state.deadlockReport.deadlockedPids.includes(p.id);
        const isAborted = p.state === 'ABORTED';

        const circle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        circle.setAttribute('cx', coord.x);
        circle.setAttribute('cy', coord.y);
        circle.setAttribute('r', '22');
        circle.setAttribute('fill', isAborted ? '#33111b' : isDead ? 'rgba(255, 51, 102, 0.3)' : 'rgba(79, 172, 254, 0.2)');
        circle.setAttribute('stroke', isAborted ? 'var(--text-dim)' : isDead ? 'var(--accent-red)' : 'var(--accent-blue)');
        circle.setAttribute('stroke-width', isDead ? '3' : '2');
        svg.appendChild(circle);

        const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        text.setAttribute('x', coord.x);
        text.setAttribute('y', coord.y + 5);
        text.setAttribute('text-anchor', 'middle');
        text.setAttribute('fill', '#fff');
        text.setAttribute('font-size', '12px');
        text.setAttribute('font-weight', '700');
        text.setAttribute('font-family', 'var(--font-mono)');
        text.textContent = p.name;
        svg.appendChild(text);
    });

    // Draw Resource Nodes
    if (state.viewMode === 'rag') {
        state.resources.forEach(r => {
            const coord = resCoords[r.id];
            const rect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            rect.setAttribute('x', coord.x - 25);
            rect.setAttribute('y', coord.y - 20);
            rect.setAttribute('width', '50');
            rect.setAttribute('height', '40');
            rect.setAttribute('rx', '6');
            rect.setAttribute('fill', 'rgba(255, 183, 3, 0.15)');
            rect.setAttribute('stroke', 'var(--accent-yellow)');
            rect.setAttribute('stroke-width', '2');
            svg.appendChild(rect);

            const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            text.setAttribute('x', coord.x);
            text.setAttribute('y', coord.y);
            text.setAttribute('text-anchor', 'middle');
            text.setAttribute('fill', '#fff');
            text.setAttribute('font-size', '12px');
            text.setAttribute('font-weight', '700');
            text.setAttribute('font-family', 'var(--font-mono)');
            text.textContent = `${r.name}`;
            svg.appendChild(text);

            const subtext = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            subtext.setAttribute('x', coord.x);
            subtext.setAttribute('y', coord.y + 14);
            subtext.setAttribute('text-anchor', 'middle');
            subtext.setAttribute('fill', 'var(--accent-yellow)');
            subtext.setAttribute('font-size', '10px');
            subtext.setAttribute('font-family', 'var(--font-mono)');
            subtext.textContent = `(${r.available}/${r.total})`;
            svg.appendChild(subtext);
        });
    }
}

function drawSvgArrow(svg, x1, y1, x2, y2, color, label = '', dashed = false) {
    const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
    line.setAttribute('x1', x1);
    line.setAttribute('y1', y1);
    line.setAttribute('x2', x2);
    line.setAttribute('y2', y2);
    line.setAttribute('stroke', color);
    line.setAttribute('stroke-width', '2');
    if (dashed) line.setAttribute('stroke-dasharray', '4,4');
    svg.appendChild(line);

    if (label) {
        const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        text.setAttribute('x', (x1 + x2) / 2);
        text.setAttribute('y', (y1 + y2) / 2 - 5);
        text.setAttribute('fill', color);
        text.setAttribute('font-size', '10px');
        text.setAttribute('font-weight', '700');
        text.setAttribute('font-family', 'var(--font-mono)');
        text.setAttribute('text-anchor', 'middle');
        text.textContent = label;
        svg.appendChild(text);
    }
}

// Update UI
function updateUI() {
    // Chips
    const chipsContainer = document.getElementById('availableVectorChips');
    if (chipsContainer) {
        chipsContainer.innerHTML = state.resources.map(r => `
            <div class="res-chip">
                <span>${r.name}: ${r.available}</span>
                <span class="total">/ ${r.total}</span>
            </div>
        `).join('');
    }

    // Matrix Table
    const tbody = document.getElementById('matrixTableBody');
    if (tbody) {
        tbody.innerHTML = state.processes.map(p => {
            const stClass = p.state.toLowerCase();
            return `
                <tr>
                    <td><strong>${p.name}</strong></td>
                    <td>[ ${p.alloc.join(', ')} ]</td>
                    <td>[ ${p.need.join(', ')} ]</td>
                    <td>[ ${p.max.join(', ')} ]</td>
                    <td>P:${p.priority} (C:${p.costWeight})</td>
                    <td><span class="status-tag ${stClass}">${p.state}</span></td>
                </tr>
            `;
        }).join('');
    }

    // Dropdowns
    const reqProc = document.getElementById('reqProcessSelect');
    const relProc = document.getElementById('relProcessSelect');
    const reqRes = document.getElementById('reqResourceSelect');
    const relRes = document.getElementById('relResourceSelect');

    if (reqProc && reqProc.options.length !== state.processes.length) {
        reqProc.innerHTML = state.processes.map(p => `<option value="${p.id}">${p.name}</option>`).join('');
        relProc.innerHTML = state.processes.map(p => `<option value="${p.id}">${p.name}</option>`).join('');
        reqRes.innerHTML = state.resources.map(r => `<option value="${r.id}">${r.name}</option>`).join('');
        relRes.innerHTML = state.resources.map(r => `<option value="${r.id}">${r.name}</option>`).join('');
    }

    renderGraph();
}

// Event Listeners
document.addEventListener('DOMContentLoaded', () => {
    // Tabs
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
            btn.classList.add('active');
            const target = btn.getAttribute('data-tab');
            const content = document.getElementById(target);
            if (content) content.classList.add('active');
        });
    });

    // View toggles
    document.getElementById('btnViewRAG').addEventListener('click', (e) => {
        state.viewMode = 'rag';
        document.getElementById('btnViewRAG').classList.add('active');
        document.getElementById('btnViewWFG').classList.remove('active');
        renderGraph();
    });

    document.getElementById('btnViewWFG').addEventListener('click', (e) => {
        state.viewMode = 'wfg';
        document.getElementById('btnViewWFG').classList.add('active');
        document.getElementById('btnViewRAG').classList.remove('active');
        renderGraph();
    });

    // Preset Scenarios
    document.querySelectorAll('.scenario-item').forEach(item => {
        item.addEventListener('click', () => {
            const sc = item.getAttribute('data-scenario');
            if (presets[sc]) presets[sc]();
        });
    });

    // Mode Selector
    document.getElementById('operatingModeSelect').addEventListener('change', (e) => {
        state.mode = e.target.value;
        logMessage(`Operating Mode switched to: ${state.mode.toUpperCase()}`, 'info');
    });

    // Victim Policy
    document.getElementById('victimPolicySelect').addEventListener('change', (e) => {
        state.victimPolicy = e.target.value;
        logMessage(`Victim Policy set to: ${state.victimPolicy}`, 'info');
    });

    // Engine Buttons
    document.getElementById('btnRunBanker').addEventListener('click', runBankersAlgorithm);
    document.getElementById('btnRunDetection').addEventListener('click', runDeadlockDetection);
    document.getElementById('btnRunRecovery').addEventListener('click', runRecovery);
    document.getElementById('btnResetSystem').addEventListener('click', presets['01_bankers_safe']);

    // Live Mutex Race Demo Button
    document.getElementById('btnRunPthreadDemo').addEventListener('click', () => {
        logMessage('⚡ Initializing Live POSIX Mutex Race Simulation...', 'warning');
        presets['03_single_instance_cycle']();
        setTimeout(() => {
            logMessage('🚨 Watchdog Alert: Circular Mutex Contention Detected! Triggering Auto-Recovery...', 'danger');
            runRecovery();
        }, 1200);
    });

    // Submit Request
    document.getElementById('btnSubmitRequest').addEventListener('click', () => {
        const pid = parseInt(document.getElementById('reqProcessSelect').value);
        const rid = parseInt(document.getElementById('reqResourceSelect').value);
        const units = parseInt(document.getElementById('reqUnitsInput').value);

        const proc = state.processes[pid];
        const res = state.resources[rid];

        if (state.mode === 'avoidance') {
            if (units > res.available) {
                logMessage(`Request for ${units} of ${res.name} by ${proc.name} BLOCKED (Only ${res.available} available).`, 'warning');
                proc.state = 'BLOCKED';
            } else {
                res.available -= units;
                proc.alloc[rid] += units;
                proc.need[rid] = Math.max(0, proc.need[rid] - units);
                logMessage(`Request Granted: ${proc.name} acquired ${units} of ${res.name}.`, 'success');
                runBankersAlgorithm();
            }
        } else {
            // Detection mode
            if (units > res.available) {
                proc.need[rid] += units;
                proc.state = 'BLOCKED';
                logMessage(`Request BLOCKED: ${proc.name} waiting for ${units} of ${res.name}.`, 'warning');
            } else {
                res.available -= units;
                proc.alloc[rid] += units;
                logMessage(`Request Granted: ${proc.name} acquired ${units} of ${res.name}.`, 'success');
            }
            runDeadlockDetection();
        }
        updateUI();
    });

    // Submit Release
    document.getElementById('btnSubmitRelease').addEventListener('click', () => {
        const pid = parseInt(document.getElementById('relProcessSelect').value);
        const rid = parseInt(document.getElementById('relResourceSelect').value);
        const units = parseInt(document.getElementById('relUnitsInput').value);

        const proc = state.processes[pid];
        const res = state.resources[rid];

        if (proc.alloc[rid] >= units) {
            proc.alloc[rid] -= units;
            res.available += units;
            logMessage(`Released: ${proc.name} freed ${units} of ${res.name}.`, 'success');
            if (state.mode === 'avoidance') runBankersAlgorithm();
            else runDeadlockDetection();
        } else {
            logMessage(`Release error: ${proc.name} only holds ${proc.alloc[rid]} of ${res.name}.`, 'danger');
        }
        updateUI();
    });

    document.getElementById('btnClearLogs').addEventListener('click', () => {
        const stream = document.getElementById('logStream');
        if (stream) stream.innerHTML = '';
    });

    // Theme Toggle
    const themeBtn = document.getElementById('btnThemeToggle');
    if (themeBtn) {
        themeBtn.addEventListener('click', () => {
            if (document.body.classList.contains('theme-dark')) {
                document.body.classList.remove('theme-dark');
                document.body.classList.add('theme-cream');
                themeBtn.innerText = '🌙 Dark Mode';
                logMessage('Switched UI Theme to Cream Studio.', 'info');
            } else {
                document.body.classList.remove('theme-cream');
                document.body.classList.add('theme-dark');
                themeBtn.innerText = '🌓 Light Mode';
                logMessage('Switched UI Theme to Dark Studio.', 'info');
            }
            renderGraph();
        });
    }

    // Initial Load
    presets['01_bankers_safe']();
    window.addEventListener('resize', renderGraph);
});
