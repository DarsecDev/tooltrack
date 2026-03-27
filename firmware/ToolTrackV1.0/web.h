
#pragma once

const char DASHBOARD_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ToolTrack</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Space+Mono:wght@400;700&family=DM+Sans:wght@400;500;600&display=swap');
:root{--bg:#0d0f12;--sur:#161a20;--sur2:#1e242c;--bor:#2a3040;--acc:#00e5a0;--acc2:#00b87a;--wrn:#ff6b35;--inf:#4da6ff;--txt:#e8edf2;--mut:#6b7585;--mono:'Space Mono',monospace;--sans:'DM Sans',sans-serif;}
*{box-sizing:border-box;margin:0;padding:0;}
body{background:var(--bg);color:var(--txt);font-family:var(--sans);min-height:100vh;}
header{background:var(--sur);border-bottom:1px solid var(--bor);padding:0 28px;height:60px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;z-index:100;}
.logo{font-family:var(--mono);font-size:14px;letter-spacing:.08em;color:var(--acc);display:flex;align-items:center;gap:10px;}
.logo-sq{width:26px;height:26px;background:var(--acc);border-radius:5px;display:grid;place-items:center;}
.logo-sq svg{width:14px;height:14px;}
.hdr-r{display:flex;align-items:center;gap:16px;}
.badge{display:flex;align-items:center;gap:6px;font-size:11px;font-family:var(--mono);color:var(--mut);background:var(--sur2);border:1px solid var(--bor);padding:4px 11px;border-radius:20px;}
.dot{width:7px;height:7px;border-radius:50%;background:#e05;transition:background .3s;}
.dot.live{background:var(--acc);animation:pu 2s infinite;}
@keyframes pu{0%,100%{opacity:1;transform:scale(1)}50%{opacity:.5;transform:scale(.8)}}
.clk{font-family:var(--mono);font-size:11px;color:var(--mut);}
main{max-width:1160px;margin:0 auto;padding:28px 20px;}
.alert{display:none;background:#2a1408;border:1px solid var(--wrn);border-radius:8px;padding:9px 16px;margin-bottom:20px;font-size:12px;color:var(--wrn);font-family:var(--mono);}
.alert.show{display:block;}
.stats{display:grid;grid-template-columns:repeat(4,1fr);gap:14px;margin-bottom:28px;}
.sc{background:var(--sur);border:1px solid var(--bor);border-radius:11px;padding:18px 20px;}
.sl{font-size:10px;font-family:var(--mono);color:var(--mut);text-transform:uppercase;letter-spacing:.1em;margin-bottom:7px;}
.sv{font-size:30px;font-family:var(--mono);font-weight:700;line-height:1;}
.sv.ok{color:var(--acc)}.sv.wn{color:var(--wrn)}.sv.if{color:var(--inf)}.sv.ne{color:var(--txt);font-size:17px;padding-top:5px;}
.ss{font-size:11px;color:var(--mut);margin-top:5px;}
.sec-hdr{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px;}
.sec-t{font-family:var(--mono);font-size:10px;text-transform:uppercase;letter-spacing:.12em;color:var(--mut);}
.ws-lbl{font-family:var(--mono);font-size:10px;padding:3px 9px;border-radius:5px;border:1px solid var(--bor);background:var(--sur2);color:var(--mut);}
.grid{display:grid;grid-template-columns:repeat(4,1fr);gap:12px;margin-bottom:32px;}
.card{background:var(--sur);border:1px solid var(--bor);border-radius:11px;padding:16px;cursor:pointer;transition:border-color .15s,transform .1s;}
.card:hover{border-color:var(--acc);transform:translateY(-1px);}
.card.ok{background:#0f2820;border-color:#1a4030;}
.card.miss{background:#2a1408;border-color:#4a2010;animation:bb 2.5s ease-in-out infinite;}
@keyframes bb{0%,100%{border-color:#4a2010}50%{border-color:var(--wrn)}}
.sn{font-family:var(--mono);font-size:9px;color:var(--mut);margin-bottom:8px;letter-spacing:.07em;}
.ti{font-size:26px;margin-bottom:8px;display:block;}
.tn{font-size:13px;font-weight:600;margin-bottom:4px;}
.ts{display:flex;align-items:center;gap:5px;font-size:10px;font-family:var(--mono);}
.sd{width:5px;height:5px;border-radius:50%;flex-shrink:0;}
.card.ok .sd{background:var(--acc)}.card.ok .ts{color:var(--acc2)}
.card.miss .sd{background:var(--wrn)}.card.miss .ts{color:var(--wrn)}
.tb{font-size:9px;color:var(--mut);margin-top:5px;font-family:var(--mono);overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}
.log-box{background:var(--sur);border:1px solid var(--bor);border-radius:11px;overflow:hidden;}
.log-hdr{display:flex;align-items:center;justify-content:space-between;padding:14px 18px;border-bottom:1px solid var(--bor);}
.log-t{font-family:var(--mono);font-size:11px;color:var(--mut);text-transform:uppercase;letter-spacing:.1em;}
.log-c{font-family:var(--mono);font-size:10px;background:var(--sur2);border:1px solid var(--bor);color:var(--mut);padding:2px 9px;border-radius:9px;}
table{width:100%;border-collapse:collapse;}
th{font-family:var(--mono);font-size:9px;text-transform:uppercase;letter-spacing:.09em;color:var(--mut);padding:9px 18px;text-align:left;border-bottom:1px solid var(--bor);font-weight:400;}
tr{border-bottom:1px solid var(--bor);transition:background .1s;}
tr:last-child{border-bottom:none}tr:hover{background:var(--sur2)}
td{padding:11px 18px;font-size:12px;}
td.m{font-family:var(--mono);font-size:11px;color:var(--mut);}
.ev{display:inline-block;font-family:var(--mono);font-size:9px;padding:2px 7px;border-radius:3px;letter-spacing:.05em;}
.ev.sal{background:rgba(255,107,53,.15);color:var(--wrn);border:1px solid rgba(255,107,53,.3);}
.ev.ent{background:rgba(0,229,160,.1);color:var(--acc);border:1px solid rgba(0,229,160,.25);}
.uc{display:inline-flex;align-items:center;gap:5px;font-size:11px;}
.av{width:20px;height:20px;border-radius:50%;display:flex;align-items:center;justify-content:center;font-family:var(--mono);font-size:8px;font-weight:700;flex-shrink:0;}
.ov{display:none;position:fixed;inset:0;background:rgba(0,0,0,.72);z-index:200;align-items:center;justify-content:center;}
.ov.on{display:flex;}
.mod{background:var(--sur);border:1px solid var(--bor);border-radius:14px;padding:26px;width:340px;max-width:92vw;}
.mod h3{font-family:var(--mono);font-size:13px;margin-bottom:14px;color:var(--acc);}
.md{display:flex;justify-content:space-between;align-items:center;padding:9px 0;border-bottom:1px solid var(--bor);font-size:12px;}
.md:last-of-type{border-bottom:none}.mk{color:var(--mut);font-size:10px;font-family:var(--mono)}.mv{font-weight:500;text-align:right;}
.cbtn{margin-top:18px;width:100%;background:var(--sur2);border:1px solid var(--bor);color:var(--txt);padding:9px;border-radius:7px;cursor:pointer;font-family:var(--mono);font-size:11px;transition:border-color .15s;}
.cbtn:hover{border-color:var(--acc);color:var(--acc);}
@media(max-width:700px){.stats{grid-template-columns:repeat(2,1fr)}.grid{grid-template-columns:repeat(2,1fr)}header{padding:0 14px}main{padding:18px 14px}}
</style>
</head>
<body>
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
  <div class="hdr-r">
    <div class="badge"><div class="dot" id="dot"></div><span id="conn">Conectando...</span></div>
    <div class="clk" id="clk">--:--:--</div>
  </div>
</header>
<main>
  <div class="alert" id="alert"></div>
  <div class="stats">
    <div class="sc"><div class="sl">Disponibles</div><div class="sv ok" id="s-ok">--</div><div class="ss">de 12 herramientas</div></div>
    <div class="sc"><div class="sl">Faltantes</div><div class="sv wn" id="s-miss">--</div><div class="ss">fuera del panel</div></div>
    <div class="sc"><div class="sl">Eventos</div><div class="sv if" id="s-ev">0</div><div class="ss">en esta sesión</div></div>
    <div class="sc"><div class="sl">Última actividad</div><div class="sv ne" id="s-last">--:--</div><div class="ss" id="s-who">Esperando...</div></div>
  </div>
  <div class="sec-hdr">
    <span class="sec-t">Estado del panel — 12 slots</span>
    <span class="ws-lbl" id="ws-lbl">WebSocket</span>
  </div>
  <div class="grid" id="grid">
    <div style="grid-column:1/-1;text-align:center;padding:36px;color:var(--mut);font-family:var(--mono);font-size:12px;">Conectando al ESP32...</div>
  </div>
  <div class="log-box">
    <div class="log-hdr"><span class="log-t">Registro de movimientos</span><span class="log-c" id="log-c">0 eventos</span></div>
    <table>
      <thead><tr><th>Hora</th><th>Herramienta</th><th>Evento</th><th>Usuario</th><th>UID RFID</th></tr></thead>
      <tbody id="log-b"><tr><td colspan="5" style="text-align:center;color:var(--mut);padding:22px;font-family:var(--mono);font-size:11px;">Sin eventos aún</td></tr></tbody>
    </table>
  </div>
</main>
<div class="ov" id="ov" onclick="closeM(event)">
  <div class="mod">
    <h3 id="m-t">—</h3>
    <div class="md"><span class="mk">Estado</span><span class="mv" id="m-s">—</span></div>
    <div class="md"><span class="mk">Slot</span><span class="mv" id="m-sl">—</span></div>
    <div class="md"><span class="mk">Sensor IR</span><span class="mv" id="m-ir">—</span></div>
    <div class="md"><span class="mk">Último usuario</span><span class="mv" id="m-u">—</span></div>
    <div class="md"><span class="mk">Última actividad</span><span class="mv" id="m-tm">—</span></div>
    <div class="md"><span class="mk">UID RFID</span><span class="mv" id="m-uid">—</span></div>
    <button class="cbtn" onclick="document.getElementById('ov').classList.remove('on')">Cerrar</button>
  </div>
</div>
<script>
let tools=[],log=[],evCnt=0;
const PAL=[['#3b2fa0','#a89ff7'],['#8b1a52','#f09ed0'],['#0e5a3a','#6be8b0'],['#7a3200','#ffb87a'],['#1a3a7a','#7ab8ff']];
const UC={};let pi=0;
function uc(n){if(!UC[n])UC[n]=PAL[pi++%PAL.length];return UC[n];}
function ini(n){return(n||'?').split(' ').map(w=>w[0]||'').join('').slice(0,2).toUpperCase();}
function pad(n){return String(n).padStart(2,'0');}

function renderGrid(){
  const g=document.getElementById('grid');
  if(!tools.length)return;
  g.innerHTML='';
  let ok=0,miss=0;
  tools.forEach(t=>{
    if(t.present)ok++;else miss++;
    const d=document.createElement('div');
    d.className='card '+(t.present?'ok':'miss');
    d.onclick=()=>openM(t);
    d.innerHTML='<div class="sn">SLOT '+pad(t.slot)+'</div>'+
      '<span class="ti">'+(t.icon||'🔧')+'</span>'+
      '<div class="tn">'+t.name+'</div>'+
      '<div class="ts"><div class="sd"></div>'+(t.present?'Disponible':'Faltante')+'</div>'+
      (!t.present&&t.lastUser?'<div class="tb">→ '+t.lastUser+' · '+t.lastTime+'</div>':'');
    g.appendChild(d);
  });
  document.getElementById('s-ok').textContent=ok;
  document.getElementById('s-miss').textContent=miss;
  const al=document.getElementById('alert');
  if(miss>0){
    const nm=tools.filter(t=>!t.present).map(t=>t.name).join(', ');
    al.textContent='⚠  '+miss+' faltante'+(miss>1?'s':'')+': '+nm;
    al.classList.add('show');
  }else al.classList.remove('show');
}

function renderLog(){
  const tb=document.getElementById('log-b');
  document.getElementById('log-c').textContent=log.length+' eventos';
  if(!log.length){tb.innerHTML='<tr><td colspan="5" style="text-align:center;color:var(--mut);padding:22px;font-family:var(--mono);font-size:11px;">Sin eventos aún</td></tr>';return;}
  tb.innerHTML='';
  log.slice(0,30).forEach(ev=>{
    const c=uc(ev.user),tr=document.createElement('tr');
    tr.innerHTML='<td class="m">'+ev.time+'</td>'+
      '<td>'+(ev.icon||'')+'&nbsp;'+ev.tool+'</td>'+
      '<td><span class="ev '+(ev.event==='salida'?'sal':'ent')+'">'+ev.event.toUpperCase()+'</span></td>'+
      '<td><div class="uc"><div class="av" style="background:'+c[0]+';color:'+c[1]+'">'+ini(ev.user)+'</div>'+ev.user+'</div></td>'+
      '<td class="m">'+ev.uid+'</td>';
    tb.appendChild(tr);
  });
}

function openM(t){
  document.getElementById('m-t').textContent=(t.icon||'')+' '+t.name;
  const s=document.getElementById('m-s');
  s.textContent=t.present?'✓ Disponible':'⚠ Faltante';
  s.style.color=t.present?'var(--acc)':'var(--wrn)';
  document.getElementById('m-sl').textContent='Slot '+pad(t.slot);
  document.getElementById('m-ir').textContent=t.present?'Bloqueado (presente)':'Libre (ausente)';
  document.getElementById('m-u').textContent=t.lastUser||'—';
  document.getElementById('m-tm').textContent=t.lastTime||'—';
  document.getElementById('m-uid').textContent=t.lastUID||'—';
  document.getElementById('ov').classList.add('on');
}
function closeM(e){if(e.target===document.getElementById('ov'))document.getElementById('ov').classList.remove('on');}

let ws,rt;
function connect(){
  const dot=document.getElementById('dot'),ct=document.getElementById('conn'),wl=document.getElementById('ws-lbl');
  ws=new WebSocket('ws://'+location.hostname+'/ws');
  ws.onopen=()=>{dot.classList.add('live');ct.textContent='ESP32 conectado';wl.textContent='WS activo';wl.style.color='var(--acc)';clearTimeout(rt);};
  ws.onclose=()=>{dot.classList.remove('live');ct.textContent='Desconectado';wl.textContent='WS caído';wl.style.color='var(--wrn)';rt=setTimeout(connect,3000);};
  ws.onerror=()=>ws.close();
  ws.onmessage=(e)=>{
    let d;try{d=JSON.parse(e.data);}catch{return;}
    if(d.type==='full_state'){
      tools=d.tools;renderGrid();renderLog();
    }else if(d.type==='slot_event'){
      const i=tools.findIndex(t=>t.slot===d.slot);
      if(i!==-1){tools[i].present=d.present;tools[i].lastUser=d.user;tools[i].lastUID=d.uid;tools[i].lastTime=d.time;tools[i].icon=d.icon;}
      log.unshift({time:d.time,tool:d.name,icon:d.icon,event:d.event,user:d.user,uid:d.uid});
      evCnt++;document.getElementById('s-ev').textContent=evCnt;
      document.getElementById('s-last').textContent=d.time.slice(0,5);
      document.getElementById('s-who').textContent=d.name+' · '+d.user;
      renderGrid();renderLog();
    }
  };
}
setInterval(()=>{const n=new Date();document.getElementById('clk').textContent=n.toTimeString().slice(0,8);},1000);
connect();
</script>
</body>
</html>)HTML";