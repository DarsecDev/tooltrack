#pragma once

// ═══════════════════════════════════════════════════════════
//  DASHBOARD_HTML.h — Página web del sistema ToolTrack
//  Se guarda en la memoria flash del ESP32 (PROGMEM)
//  Para modificar el diseño, edita las secciones marcadas
// ═══════════════════════════════════════════════════════════

const char DASHBOARD_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ToolTrack</title>

  <!-- Fuentes: Space Mono = números/etiquetas | DM Sans = texto general -->
  <link href="https://fonts.googleapis.com/css2?family=Space+Mono:wght@400;700&family=DM+Sans:wght@400;500;600&display=swap" rel="stylesheet">

  <style>

    /* ── COLORES DEL TEMA ─────────────────────────────────────
       Cambia estos valores para cambiar todo el diseño de una vez  */
    :root {
      --bg:    #0d0f12;   /* fondo de la página */
      --sur:   #161a20;   /* fondo de tarjetas */
      --sur2:  #1e242c;   /* fondo de elementos secundarios */
      --bor:   #2a3040;   /* color de bordes */
      --acc:   #00e5a0;   /* verde = disponible / conectado */
      --acc2:  #00b87a;   /* verde oscuro para texto */
      --wrn:   #ff6b35;   /* naranja = faltante / alerta */
      --inf:   #4da6ff;   /* azul = información */
      --txt:   #e8edf2;   /* texto principal */
      --mut:   #6b7585;   /* texto secundario (gris) */
      --mono:  'Space Mono', monospace;
      --sans:  'DM Sans', sans-serif;
    }

    /* ── RESET BASE ───────────────────────────────────────── */
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { background: var(--bg); color: var(--txt); font-family: var(--sans); min-height: 100vh; }

    /* ── HEADER ───────────────────────────────────────────── */
    header {
      background: var(--sur);
      border-bottom: 1px solid var(--bor);
      padding: 0 28px;
      height: 60px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      position: sticky; top: 0; z-index: 100;
    }
    .logo {
      font-family: var(--mono);
      font-size: 14px;
      letter-spacing: .08em;
      color: var(--acc);
      display: flex; align-items: center; gap: 10px;
    }
    .logo-sq {
      width: 26px; height: 26px;
      background: var(--acc);
      border-radius: 5px;
      display: grid; place-items: center;
    }
    .logo-sq svg { width: 14px; height: 14px; }
    .header-der { display: flex; align-items: center; gap: 16px; }

    /* Punto de conexión WebSocket */
    .badge {
      display: flex; align-items: center; gap: 6px;
      font-size: 11px; font-family: var(--mono); color: var(--mut);
      background: var(--sur2); border: 1px solid var(--bor);
      padding: 4px 11px; border-radius: 20px;
    }
    .dot { width: 7px; height: 7px; border-radius: 50%; background: red; transition: background .3s; }
    .dot.live { background: var(--acc); animation: pulso 2s infinite; }
    @keyframes pulso {
      0%, 100% { opacity: 1; transform: scale(1); }
      50%       { opacity: .5; transform: scale(.8); }
    }
    .reloj { font-family: var(--mono); font-size: 11px; color: var(--mut); }

    /* ── CONTENIDO PRINCIPAL ──────────────────────────────── */
    main { max-width: 1160px; margin: 0 auto; padding: 28px 20px; }

    /* Barra de alerta (herramientas faltantes) */
    .alerta {
      display: none;   /* JS la muestra con .alerta.show */
      background: #2a1408; border: 1px solid var(--wrn);
      border-radius: 8px; padding: 9px 16px; margin-bottom: 20px;
      font-size: 12px; color: var(--wrn); font-family: var(--mono);
    }
    .alerta.show { display: block; }

    /* ── TARJETAS DE ESTADÍSTICAS ─────────────────────────── */
    .stats {
      display: grid;
      grid-template-columns: repeat(4, 1fr);  /* 4 columnas */
      gap: 14px;
      margin-bottom: 28px;
    }
    .stat {
      background: var(--sur); border: 1px solid var(--bor);
      border-radius: 11px; padding: 18px 20px;
    }
    .stat-label { font-size: 10px; font-family: var(--mono); color: var(--mut); text-transform: uppercase; letter-spacing: .1em; margin-bottom: 7px; }
    .stat-num   { font-size: 30px; font-family: var(--mono); font-weight: 700; line-height: 1; }
    .stat-sub   { font-size: 11px; color: var(--mut); margin-top: 5px; }

    /* Colores de cada número */
    .stat-num.verde   { color: var(--acc); }
    .stat-num.naranja { color: var(--wrn); }
    .stat-num.azul    { color: var(--inf); }
    .stat-num.normal  { color: var(--txt); font-size: 17px; padding-top: 5px; }

    /* ── GRILLA DE HERRAMIENTAS ───────────────────────────── */
    .sec-header {
      display: flex; align-items: center; justify-content: space-between;
      margin-bottom: 14px;
    }
    .sec-titulo { font-family: var(--mono); font-size: 10px; text-transform: uppercase; letter-spacing: .12em; color: var(--mut); }
    .ws-label   { font-family: var(--mono); font-size: 10px; padding: 3px 9px; border-radius: 5px; border: 1px solid var(--bor); background: var(--sur2); color: var(--mut); }

    .grilla {
      display: grid;
      grid-template-columns: repeat(4, 1fr);  /* 4 columnas → cambia a 3 o 6 si quieres */
      gap: 12px;
      margin-bottom: 32px;
    }

    /* Tarjeta individual */
    .tarjeta {
      background: var(--sur); border: 1px solid var(--bor);
      border-radius: 11px; padding: 16px;
      cursor: pointer;
      transition: border-color .15s, transform .1s;
    }
    .tarjeta:hover { border-color: var(--acc); transform: translateY(-1px); }

    /* Verde oscuro = herramienta presente */
    .tarjeta.ok   { background: #0f2820; border-color: #1a4030; }

    /* Naranja oscuro + parpadeo = herramienta faltante */
    .tarjeta.miss { background: #2a1408; border-color: #4a2010; animation: parpadeo 2.5s ease-in-out infinite; }
    @keyframes parpadeo {
      0%, 100% { border-color: #4a2010; }
      50%       { border-color: var(--wrn); }
    }

    .slot-num    { font-family: var(--mono); font-size: 9px; color: var(--mut); margin-bottom: 8px; letter-spacing: .07em; }
    .h-icono     { font-size: 26px; margin-bottom: 8px; display: block; }
    .h-nombre    { font-size: 13px; font-weight: 600; margin-bottom: 4px; }
    .h-estado    { display: flex; align-items: center; gap: 5px; font-size: 10px; font-family: var(--mono); }
    .puntito     { width: 5px; height: 5px; border-radius: 50%; flex-shrink: 0; }

    .tarjeta.ok   .puntito  { background: var(--acc); }
    .tarjeta.ok   .h-estado { color: var(--acc2); }
    .tarjeta.miss .puntito  { background: var(--wrn); }
    .tarjeta.miss .h-estado { color: var(--wrn); }

    /* "Tomada por" — solo aparece si está faltante */
    .tomada-por { font-size: 9px; color: var(--mut); margin-top: 5px; font-family: var(--mono); overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

    /* ── TABLA DE REGISTRO ────────────────────────────────── */
    .log-caja { background: var(--sur); border: 1px solid var(--bor); border-radius: 11px; overflow: hidden; }
    .log-header {
      display: flex; align-items: center; justify-content: space-between;
      padding: 14px 18px; border-bottom: 1px solid var(--bor);
    }
    .log-titulo   { font-family: var(--mono); font-size: 11px; color: var(--mut); text-transform: uppercase; letter-spacing: .1em; }
    .log-contador { font-family: var(--mono); font-size: 10px; background: var(--sur2); border: 1px solid var(--bor); color: var(--mut); padding: 2px 9px; border-radius: 9px; }

    table { width: 100%; border-collapse: collapse; }
    th {
      font-family: var(--mono); font-size: 9px; text-transform: uppercase;
      letter-spacing: .09em; color: var(--mut);
      padding: 9px 18px; text-align: left;
      border-bottom: 1px solid var(--bor); font-weight: 400;
    }
    tr { border-bottom: 1px solid var(--bor); transition: background .1s; }
    tr:last-child { border-bottom: none; }
    tr:hover { background: var(--sur2); }
    td { padding: 11px 18px; font-size: 12px; }
    td.mono { font-family: var(--mono); font-size: 11px; color: var(--mut); }

    /* Etiquetas SALIDA / ENTRADA */
    .evento { display: inline-block; font-family: var(--mono); font-size: 9px; padding: 2px 7px; border-radius: 3px; letter-spacing: .05em; }
    .evento.sal { background: rgba(255,107,53,.15); color: var(--wrn); border: 1px solid rgba(255,107,53,.3); }
    .evento.ent { background: rgba(0,229,160,.10); color: var(--acc); border: 1px solid rgba(0,229,160,.25); }

    /* Avatar circular del usuario */
    .chip   { display: inline-flex; align-items: center; gap: 5px; font-size: 11px; }
    .avatar { width: 20px; height: 20px; border-radius: 50%; display: flex; align-items: center; justify-content: center; font-family: var(--mono); font-size: 8px; font-weight: 700; flex-shrink: 0; }

    /* ── MODAL (popup al hacer clic en tarjeta) ───────────── */
    .modal-fondo { display: none; position: fixed; inset: 0; background: rgba(0,0,0,.72); z-index: 200; align-items: center; justify-content: center; }
    .modal-fondo.abierto { display: flex; }
    .modal-caja  { background: var(--sur); border: 1px solid var(--bor); border-radius: 14px; padding: 26px; width: 340px; max-width: 92vw; }
    .modal-caja h3 { font-family: var(--mono); font-size: 13px; margin-bottom: 14px; color: var(--acc); }
    .modal-fila  { display: flex; justify-content: space-between; align-items: center; padding: 9px 0; border-bottom: 1px solid var(--bor); font-size: 12px; }
    .modal-fila:last-of-type { border-bottom: none; }
    .modal-clave { color: var(--mut); font-size: 10px; font-family: var(--mono); }
    .modal-valor { font-weight: 500; text-align: right; }
    .btn-cerrar  { margin-top: 18px; width: 100%; background: var(--sur2); border: 1px solid var(--bor); color: var(--txt); padding: 9px; border-radius: 7px; cursor: pointer; font-family: var(--mono); font-size: 11px; transition: border-color .15s; }
    .btn-cerrar:hover { border-color: var(--acc); color: var(--acc); }

    /* ── RESPONSIVE ───────────────────────────────────────── */
    @media (max-width: 700px) {
      .stats  { grid-template-columns: repeat(2, 1fr); }
      .grilla { grid-template-columns: repeat(2, 1fr); }
      header  { padding: 0 14px; }
      main    { padding: 18px 14px; }
    }

  </style>
</head>
<body>

<!-- ═══ HEADER ══════════════════════════════════════════════ -->
<header>
  <div class="logo">
    <div class="logo-sq">
      <svg viewBox="0 0 14 14" fill="none">
        <rect x="1" y="1" width="4" height="4" rx="1" fill="#0d0f12"/>
        <rect x="8" y="1" width="4" height="4" rx="1" fill="#0d0f12"/>
        <rect x="1" y="8" width="4" height="4" rx="1" fill="#0d0f12"/>
        <rect x="8" y="8" width="4" height="4" rx="1" fill="#0d0f12"/>
      </svg>
    </div>
    TOOLTRACK
  </div>
  <div class="header-der">
    <div class="badge">
      <div class="dot" id="dot"></div>
      <span id="conn">Conectando...</span>
    </div>
    <div class="reloj" id="reloj">--:--:--</div>
  </div>
</header>

<!-- ═══ CONTENIDO PRINCIPAL ═════════════════════════════════ -->
<main>

  <!-- Alerta: aparece automáticamente cuando faltan herramientas -->
  <div class="alerta" id="alerta"></div>

  <!-- Estadísticas: 4 números grandes -->
  <div class="stats">
    <div class="stat">
      <div class="stat-label">Disponibles</div>
      <div class="stat-num verde"   id="s-ok">--</div>
      <div class="stat-sub">de 12 herramientas</div>
    </div>
    <div class="stat">
      <div class="stat-label">Faltantes</div>
      <div class="stat-num naranja" id="s-miss">--</div>
      <div class="stat-sub">fuera del panel</div>
    </div>
    <div class="stat">
      <div class="stat-label">Eventos</div>
      <div class="stat-num azul"    id="s-ev">0</div>
      <div class="stat-sub">en esta sesión</div>
    </div>
    <div class="stat">
      <div class="stat-label">Última actividad</div>
      <div class="stat-num normal"  id="s-last">--:--</div>
      <div class="stat-sub"         id="s-who">Esperando...</div>
    </div>
  </div>

  <!-- Grilla: 12 tarjetas de herramientas -->
  <div class="sec-header">
    <span class="sec-titulo">Estado del panel — 12 slots</span>
    <span class="ws-label" id="ws-lbl">WebSocket</span>
  </div>
  <div class="grilla" id="grilla">
    <!-- Las tarjetas se crean con JavaScript al recibir datos del ESP32 -->
    <div style="grid-column:1/-1; text-align:center; padding:36px; color:var(--mut); font-family:var(--mono); font-size:12px;">
      Conectando al ESP32...
    </div>
  </div>

  <!-- Tabla de registro de movimientos -->
  <div class="log-caja">
    <div class="log-header">
      <span class="log-titulo">Registro de movimientos</span>
      <span class="log-contador" id="log-c">0 eventos</span>
    </div>
    <table>
      <thead>
        <tr>
          <th>Hora</th>
          <th>Herramienta</th>
          <th>Evento</th>
          <th>Usuario</th>
          <th>UID RFID</th>
        </tr>
      </thead>
      <tbody id="log-b">
        <tr>
          <td colspan="5" style="text-align:center; color:var(--mut); padding:22px; font-family:var(--mono); font-size:11px;">
            Sin eventos aún
          </td>
        </tr>
      </tbody>
    </table>
  </div>

</main>

<!-- ═══ MODAL (popup al hacer clic en una herramienta) ══════ -->
<div class="modal-fondo" id="modal" onclick="cerrarModal(event)">
  <div class="modal-caja">
    <h3 id="m-titulo">—</h3>
    <div class="modal-fila"><span class="modal-clave">Estado</span>          <span class="modal-valor" id="m-estado">—</span></div>
    <div class="modal-fila"><span class="modal-clave">Slot</span>            <span class="modal-valor" id="m-slot">—</span></div>
    <div class="modal-fila"><span class="modal-clave">Sensor IR</span>       <span class="modal-valor" id="m-ir">—</span></div>
    <div class="modal-fila"><span class="modal-clave">Último usuario</span>  <span class="modal-valor" id="m-user">—</span></div>
    <div class="modal-fila"><span class="modal-clave">Última actividad</span><span class="modal-valor" id="m-hora">—</span></div>
    <div class="modal-fila"><span class="modal-clave">UID RFID</span>        <span class="modal-valor" id="m-uid">—</span></div>
    <button class="btn-cerrar" onclick="document.getElementById('modal').classList.remove('abierto')">
      Cerrar
    </button>
  </div>
</div>


<!-- ═══ JAVASCRIPT ══════════════════════════════════════════ -->
<script>

  // ── DATOS ──────────────────────────────────────────────────
  let herramientas = [];  // estado de los 12 slots (viene del ESP32)
  let registros    = [];  // historial de movimientos
  let totalEventos = 0;

  // Colores para el avatar de cada usuario
  const PALETA = [
    ['#3b2fa0','#a89ff7'],
    ['#8b1a52','#f09ed0'],
    ['#0e5a3a','#6be8b0'],
    ['#7a3200','#ffb87a'],
    ['#1a3a7a','#7ab8ff'],
  ];
  const coloresAsignados = {};
  let indicePaleta = 0;

  // Devuelve el color de un usuario (lo asigna si es nuevo)
  function colorDeUsuario(nombre) {
    if (!coloresAsignados[nombre])
      coloresAsignados[nombre] = PALETA[indicePaleta++ % PALETA.length];
    return coloresAsignados[nombre];
  }

  // "Juan R." → "JR"
  function iniciales(nombre) {
    return (nombre || '?').split(' ').map(p => p[0] || '').join('').slice(0, 2).toUpperCase();
  }

  // 3 → "03"
  function dosDigitos(n) {
    return String(n).padStart(2, '0');
  }


  // ── DIBUJAR GRILLA DE HERRAMIENTAS ─────────────────────────
  function dibujarGrilla() {
    const grilla = document.getElementById('grilla');
    if (!herramientas.length) return;

    grilla.innerHTML = '';
    let presentes = 0, faltantes = 0;

    herramientas.forEach(h => {
      h.present ? presentes++ : faltantes++;

      // Crear tarjeta
      const div = document.createElement('div');
      div.className = 'tarjeta ' + (h.present ? 'ok' : 'miss');
      div.onclick = () => abrirModal(h);
      div.innerHTML =
        `<div class="slot-num">SLOT ${dosDigitos(h.slot)}</div>
         <span class="h-icono">${h.icon || '🔧'}</span>
         <div class="h-nombre">${h.name}</div>
         <div class="h-estado">
           <div class="puntito"></div>
           ${h.present ? 'Disponible' : 'Faltante'}
         </div>
         ${!h.present && h.lastUser
           ? `<div class="tomada-por">→ ${h.lastUser} · ${h.lastTime}</div>`
           : ''}`;

      grilla.appendChild(div);
    });

    // Actualizar números de estadísticas
    document.getElementById('s-ok').textContent   = presentes;
    document.getElementById('s-miss').textContent = faltantes;

    // Mostrar u ocultar la barra de alerta
    const alerta = document.getElementById('alerta');
    if (faltantes > 0) {
      const nombres = herramientas.filter(h => !h.present).map(h => h.name).join(', ');
      alerta.textContent = `⚠  ${faltantes} faltante${faltantes > 1 ? 's' : ''}: ${nombres}`;
      alerta.classList.add('show');
    } else {
      alerta.classList.remove('show');
    }
  }


  // ── DIBUJAR TABLA DE REGISTROS ─────────────────────────────
  function dibujarRegistros() {
    const tbody = document.getElementById('log-b');
    document.getElementById('log-c').textContent = registros.length + ' eventos';

    if (!registros.length) {
      tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; color:var(--mut); padding:22px; font-family:var(--mono); font-size:11px;">Sin eventos aún</td></tr>`;
      return;
    }

    tbody.innerHTML = '';

    // Mostrar máximo 30 registros
    registros.slice(0, 30).forEach(ev => {
      const [bg, fg] = colorDeUsuario(ev.user);
      const fila = document.createElement('tr');

      fila.innerHTML =
        `<td class="mono">${ev.time}</td>
         <td>${ev.icon || ''}&nbsp;${ev.tool}</td>
         <td><span class="evento ${ev.event === 'salida' ? 'sal' : 'ent'}">${ev.event.toUpperCase()}</span></td>
         <td>
           <div class="chip">
             <div class="avatar" style="background:${bg}; color:${fg}">${iniciales(ev.user)}</div>
             ${ev.user}
           </div>
         </td>
         <td class="mono">${ev.uid}</td>`;

      tbody.appendChild(fila);
    });
  }


  // ── MODAL ───────────────────────────────────────────────────
  function abrirModal(h) {
    document.getElementById('m-titulo').textContent = `${h.icon || ''} ${h.name}`;

    const estado = document.getElementById('m-estado');
    estado.textContent = h.present ? '✓ Disponible' : '⚠ Faltante';
    estado.style.color = h.present ? 'var(--acc)' : 'var(--wrn)';

    document.getElementById('m-slot').textContent  = 'Slot ' + dosDigitos(h.slot);
    document.getElementById('m-ir').textContent    = h.present ? 'Bloqueado (presente)' : 'Libre (ausente)';
    document.getElementById('m-user').textContent  = h.lastUser || '—';
    document.getElementById('m-hora').textContent  = h.lastTime || '—';
    document.getElementById('m-uid').textContent   = h.lastUID  || '—';

    document.getElementById('modal').classList.add('abierto');
  }

  function cerrarModal(evento) {
    // Solo cierra si hiciste clic en el fondo oscuro, no en la caja
    if (evento.target === document.getElementById('modal'))
      document.getElementById('modal').classList.remove('abierto');
  }


  // ── WEBSOCKET (comunicación en tiempo real con el ESP32) ────
  //
  //  El ESP32 envía dos tipos de mensajes:
  //    1) full_state  → estado completo de los 12 slots (al conectar)
  //    2) slot_event  → una herramienta fue tomada o devuelta
  //
  let socket, timerReconexion;

  function conectar() {
    // Se conecta al ESP32 usando la misma IP que sirve esta página
    socket = new WebSocket('ws://' + location.hostname + '/ws');

    socket.onopen = () => {
      document.getElementById('dot').classList.add('live');
      document.getElementById('conn').textContent  = 'ESP32 conectado';
      document.getElementById('ws-lbl').textContent = 'WS activo';
      document.getElementById('ws-lbl').style.color = 'var(--acc)';
      clearTimeout(timerReconexion);
    };

    socket.onclose = () => {
      document.getElementById('dot').classList.remove('live');
      document.getElementById('conn').textContent  = 'Desconectado';
      document.getElementById('ws-lbl').textContent = 'WS caído';
      document.getElementById('ws-lbl').style.color = 'var(--wrn)';
      // Intentar reconectar en 3 segundos
      timerReconexion = setTimeout(conectar, 3000);
    };

    socket.onerror = () => socket.close();

    socket.onmessage = (evento) => {
      let datos;
      try { datos = JSON.parse(evento.data); } catch { return; }

      if (datos.type === 'full_state') {
        // Recibir el estado completo al conectar
        herramientas = datos.tools;
        dibujarGrilla();
        dibujarRegistros();

      } else if (datos.type === 'slot_event') {
        // Actualizar solo la herramienta que cambió
        const idx = herramientas.findIndex(h => h.slot === datos.slot);
        if (idx !== -1) {
          herramientas[idx].present  = datos.present;
          herramientas[idx].lastUser = datos.user;
          herramientas[idx].lastUID  = datos.uid;
          herramientas[idx].lastTime = datos.time;
          herramientas[idx].icon     = datos.icon;
        }

        // Agregar al historial
        registros.unshift({
          time:  datos.time,
          tool:  datos.name,
          icon:  datos.icon,
          event: datos.event,
          user:  datos.user,
          uid:   datos.uid
        });

        // Actualizar estadísticas
        totalEventos++;
        document.getElementById('s-ev').textContent   = totalEventos;
        document.getElementById('s-last').textContent = datos.time.slice(0, 5);
        document.getElementById('s-who').textContent  = `${datos.name} · ${datos.user}`;

        dibujarGrilla();
        dibujarRegistros();
      }
    };
  }


  // ── RELOJ ───────────────────────────────────────────────────
  setInterval(() => {
    document.getElementById('reloj').textContent = new Date().toTimeString().slice(0, 8);
  }, 1000);


  // ── INICIAR ─────────────────────────────────────────────────
  conectar();

</script>
</body>
</html>
)HTML";