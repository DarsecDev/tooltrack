#pragma once

// ============================================================
//  TOOLTRACK v5 — Dashboard HTML
//  Archivo separado para mantener el código principal limpio.
//  Uso: incluir con  #include "dashboard_html.h"
//       y llamar a   buildDashboardHTML()  en setupWeb()
// ============================================================

String buildDashboardHTML(const char* toolNames[], const char* toolIcons[], int numTools) {

  String html = R"rawhtml(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ToolTrack</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Barlow:wght@400;600;700&display=swap');
    :root {
      --bg:       #0d1117;
      --surface:  #161b22;
      --border:   #30363d;
      --accent:   #58a6ff;
      --green:    #3fb950;
      --red:      #f85149;
      --yellow:   #d29922;
      --text:     #c9d1d9;
      --muted:    #8b949e;
      --mono:     'Share Tech Mono', monospace;
      --sans:     'Barlow', sans-serif;
    }
    * { margin:0; padding:0; box-sizing:border-box; }
    body { background:var(--bg); color:var(--text); font-family:var(--sans); min-height:100vh; }

    /* ── HEADER ── */
    header {
      display:flex; align-items:center; justify-content:space-between;
      padding:16px 24px;
      border-bottom:1px solid var(--border);
      background:var(--surface);
    }
    header h1 { font-family:var(--mono); font-size:1.25rem; letter-spacing:.05em; color:var(--accent); }
    .ws-badge {
      display:flex; align-items:center; gap:8px;
      font-size:.75rem; color:var(--muted);
    }
    .ws-dot {
      width:8px; height:8px; border-radius:50%;
      background:var(--red); transition:background .4s;
    }
    .ws-dot.connected { background:var(--green); }

    /* ── USER BAR ── */
    .user-bar {
      display:flex; align-items:center; gap:12px;
      padding:10px 24px;
      background:#1a2230;
      border-bottom:1px solid var(--border);
      font-size:.85rem;
    }
    .user-label { color:var(--muted); }
    .user-name  { color:var(--accent); font-weight:700; font-family:var(--mono); }

    /* ── TABS ── */
    .tabs { display:flex; border-bottom:1px solid var(--border); background:var(--surface); }
    .tab-btn {
      padding:12px 24px; border:none; background:none;
      color:var(--muted); cursor:pointer; font-family:var(--sans); font-size:.9rem;
      border-bottom:2px solid transparent; transition:all .2s;
    }
    .tab-btn:hover  { color:var(--text); }
    .tab-btn.active { color:var(--accent); border-bottom-color:var(--accent); }

    /* ── CONTENIDO ── */
    .content { padding:24px; max-width:1400px; margin:0 auto; }
    .tab-pane { display:none; }
    .tab-pane.active { display:block; }

    /* ── GRID DE HERRAMIENTAS ── */
    .tools-grid {
      display:grid;
      grid-template-columns:repeat(auto-fill, minmax(200px, 1fr));
      gap:12px;
    }
    .tool-card {
      background:var(--surface);
      border:1px solid var(--border);
      border-radius:8px;
      padding:16px;
      transition:border-color .3s, transform .15s;
    }
    .tool-card:hover { transform:translateY(-2px); }
    .tool-card.present { border-left:3px solid var(--green); }
    .tool-card.absent  { border-left:3px solid var(--red); }
    .tool-card .icon   { font-size:1.6rem; margin-bottom:8px; }
    .tool-card .slot   { font-size:.7rem; color:var(--muted); font-family:var(--mono); }
    .tool-card .name   { font-weight:600; font-size:.9rem; margin:4px 0 8px; }
    .tool-card .status {
      display:inline-block; padding:2px 8px; border-radius:4px;
      font-size:.75rem; font-weight:700; font-family:var(--mono);
    }
    .tool-card.present .status { background:#1a3a2a; color:var(--green); }
    .tool-card.absent  .status { background:#3a1a1a; color:var(--red);   }
    .tool-card .owner  { margin-top:8px; font-size:.75rem; color:var(--muted); }

    /* ── LOG EN TIEMPO REAL ── */
    .live-log {
      background:var(--surface); border:1px solid var(--border);
      border-radius:8px; overflow:hidden; margin-bottom:20px;
    }
    .live-log-header {
      padding:10px 16px; background:#1a2230;
      font-size:.8rem; color:var(--muted); font-family:var(--mono);
      border-bottom:1px solid var(--border);
      display:flex; justify-content:space-between;
    }
    .live-log-body { max-height:180px; overflow-y:auto; }
    .log-row {
      display:flex; align-items:center; gap:12px;
      padding:8px 16px; border-bottom:1px solid var(--border);
      font-size:.8rem; font-family:var(--mono);
      animation: fadeIn .3s ease;
    }
    @keyframes fadeIn { from { opacity:0; transform:translateX(-6px); } to { opacity:1; } }
    .log-row .ts    { color:var(--muted); min-width:70px; }
    .log-row .ev    { min-width:70px; font-weight:700; }
    .log-row .ev.salida  { color:var(--red);    }
    .log-row .ev.entrada { color:var(--green);  }
    .log-row .ev.acceso  { color:var(--accent); }
    .log-row .ev.denied  { color:var(--yellow); }
    .log-row .msg   { color:var(--text); }

    /* ── TABLA HISTÓRICO ── */
    .hist-table { width:100%; border-collapse:collapse; font-size:.85rem; }
    .hist-table th {
      text-align:left; padding:10px 12px;
      background:#1a2230; color:var(--muted);
      font-weight:600; font-family:var(--mono);
      border-bottom:1px solid var(--border);
    }
    .hist-table td { padding:10px 12px; border-bottom:1px solid var(--border); }
    .hist-table tr:hover td { background:#1a2230; }
    .badge {
      display:inline-block; padding:2px 7px; border-radius:4px;
      font-size:.72rem; font-weight:700; font-family:var(--mono);
    }
    .badge.tomada   { background:#3a1a1a; color:var(--red);   }
    .badge.devuelta { background:#1a3a2a; color:var(--green); }

    .section-title {
      font-size:.8rem; font-family:var(--mono);
      color:var(--muted); text-transform:uppercase;
      letter-spacing:.1em; margin-bottom:12px;
    }
    .btn {
      padding:8px 18px; border-radius:6px; border:none;
      cursor:pointer; font-family:var(--sans); font-size:.85rem;
      font-weight:600; margin-top:16px; margin-right:8px;
      transition:opacity .2s;
    }
    .btn:hover { opacity:.8; }
    .btn-primary { background:var(--accent); color:#000; }
    .btn-danger  { background:var(--red);    color:#fff; }

    .empty { text-align:center; padding:32px; color:var(--muted); font-size:.9rem; }
  </style>
</head>
<body>

<header>
  <h1>⚙ ToolTrack</h1>
  <div class="ws-badge">
    <div class="ws-dot" id="wsDot"></div>
    <span id="wsStatus">Conectando...</span>
  </div>
</header>

<div class="user-bar">
  <span class="user-label">Usuario activo:</span>
  <span class="user-name" id="currentUser">—</span>
</div>

<div class="tabs">
  <button class="tab-btn active" onclick="switchTab('tools', this)">Herramientas</button>
  <button class="tab-btn"        onclick="switchTab('history', this)">Histórico</button>
</div>

<div class="content">

  <!-- ── TAB: HERRAMIENTAS ── -->
  <div id="tools" class="tab-pane active">

    <p class="section-title" style="margin-bottom:16px;">Actividad reciente</p>
    <div class="live-log">
      <div class="live-log-header">
        <span>Log en tiempo real</span>
        <span id="logCount">0 eventos</span>
      </div>
      <div class="live-log-body" id="liveLog"></div>
    </div>

    <p class="section-title">Estado de herramientas</p>
    <div class="tools-grid" id="toolsGrid"></div>
  </div>

  <!-- ── TAB: HISTÓRICO ── -->
  <div id="history" class="tab-pane">
    <p class="section-title">Historial completo (SPIFFS)</p>
    <div style="background:var(--surface);border:1px solid var(--border);border-radius:8px;overflow:hidden;">
      <table class="hist-table">
        <thead>
          <tr>
            <th>Herramienta</th>
            <th>Acción</th>
            <th>Tomada por</th>
            <th>Devuelta por</th>
            <th>Hora</th>
          </tr>
        </thead>
        <tbody id="histBody"></tbody>
      </table>
    </div>
    <button class="btn btn-danger"   onclick="clearHistory()">Limpiar histórico</button>
    <button class="btn btn-primary"  onclick="loadHistory()">Actualizar</button>
  </div>

</div>

<script>
  // ── ESTADO LOCAL ──────────────────────────────────────────
  const toolState = {};
  let logCount = 0;

  // ── WEBSOCKET ─────────────────────────────────────────────
  const WS_URL = `ws://${location.hostname}/ws`;
  let   socket;

  function connectWS() {
    socket = new WebSocket(WS_URL);

    socket.onopen = () => {
      document.getElementById('wsDot').classList.add('connected');
      document.getElementById('wsStatus').textContent = 'En línea';
    };

    socket.onclose = () => {
      document.getElementById('wsDot').classList.remove('connected');
      document.getElementById('wsStatus').textContent = 'Reconectando...';
      setTimeout(connectWS, 3000);
    };

    socket.onmessage = (ev) => {
      const msg = JSON.parse(ev.data);

      if (msg.type === 'full_state') {
        document.getElementById('currentUser').textContent = msg.currentUser || '—';
        msg.tools.forEach(t => { toolState[t.slot] = t; });
        renderGrid();

      } else if (msg.type === 'tool_event') {
        toolState[msg.slot] = {
          name: msg.name, icon: msg.icon,
          present: msg.present, owner: msg.owner
        };
        renderGrid();

        const action = msg.event === 'salida' ? 'TOMADA' : 'DEVUELTA';
        const who    = msg.event === 'salida'
                     ? msg.owner
                     : (msg.returnedBy || msg.owner);
        addLog(msg.event, `${msg.icon} ${msg.name} — ${action} por ${who}`);

      } else if (msg.type === 'access_event') {
        if (msg.granted) {
          document.getElementById('currentUser').textContent = msg.currentUser;
          addLog('acceso', `✔ ${msg.user} accedió al sistema`);
        } else {
          addLog('denied', `✘ Tarjeta desconocida: ${msg.user}`);
        }
      }
    };
  }

  // ── RENDER GRID ───────────────────────────────────────────
  function renderGrid() {
    const grid = document.getElementById('toolsGrid');
    grid.innerHTML = '';
    for (let slot = 1; slot <= 12; slot++) {
      const t = toolState[slot];
      if (!t) continue;
      const cls  = t.present ? 'present' : 'absent';
      const card = document.createElement('div');
      card.className = `tool-card ${cls}`;
      card.innerHTML = `
        <div class="icon">${t.icon}</div>
        <div class="slot">Slot ${slot}</div>
        <div class="name">${t.name}</div>
        <span class="status">${t.present ? '● PRESENTE' : '○ AUSENTE'}</span>
        <div class="owner">${t.present ? '' : (t.owner ? '→ ' + t.owner : '')}</div>
      `;
      grid.appendChild(card);
    }
  }

  // ── LOG EN TIEMPO REAL ─────────────────────────────────────
  function addLog(type, msg) {
    logCount++;
    document.getElementById('logCount').textContent = `${logCount} eventos`;

    const now  = new Date();
    const ts   = now.toTimeString().slice(0, 8);
    const body = document.getElementById('liveLog');

    const row  = document.createElement('div');
    row.className = 'log-row';
    row.innerHTML = `
      <span class="ts">${ts}</span>
      <span class="ev ${type}">${type.toUpperCase()}</span>
      <span class="msg">${msg}</span>
    `;
    body.prepend(row);

    // Limitar a 50 filas en pantalla
    while (body.children.length > 50) body.removeChild(body.lastChild);
  }

  // ── HISTÓRICO (fetch REST) ────────────────────────────────
  async function loadHistory() {
    const res   = await fetch('/api/events');
    const data  = await res.json();
    const tbody = document.getElementById('histBody');
    tbody.innerHTML = '';

    if (!data.events || data.events.length === 0) {
      tbody.innerHTML = '<tr><td colspan="5" class="empty">Sin eventos registrados</td></tr>';
      return;
    }

    [...data.events].reverse().forEach(ev => {
      const tr    = document.createElement('tr');
      const ts    = new Date(ev.timestamp * 1000).toLocaleString('es');
      const badge = ev.taken
        ? '<span class="badge tomada">TOMADA</span>'
        : '<span class="badge devuelta">DEVUELTA</span>';
      tr.innerHTML = `
        <td>${ev.toolId + 1}. ${TOOL_NAMES[ev.toolId] || ''}</td>
        <td>${badge}</td>
        <td>${ev.user || '—'}</td>
        <td>${ev.returnedBy || '—'}</td>
        <td>${ts}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  async function clearHistory() {
    if (!confirm('¿Limpiar todo el histórico? Esta acción no se puede deshacer.')) return;
    await fetch('/api/clear-history', { method: 'POST' });
    loadHistory();
  }

  // ── TABS ──────────────────────────────────────────────────
  function switchTab(id, btn) {
    document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
    document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
    document.getElementById(id).classList.add('active');
    btn.classList.add('active');
    if (id === 'history') loadHistory();
  }

  // ── NOMBRES DE HERRAMIENTAS (inyectados desde C++) ────────
  const TOOL_NAMES = )rawhtml";

  // Inyectar array de nombres dinámicamente
  html += "[";
  for (int i = 0; i < numTools; i++) {
    html += "\"";
    html += toolNames[i];
    html += "\"";
    if (i < numTools - 1) html += ",";
  }

  html += R"rawhtml(];

  // ── INICIO ────────────────────────────────────────────────
  connectWS();
</script>
</body>
</html>
)rawhtml";

  return html;
}
