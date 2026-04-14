/*
 * ============================================================
 *  TOOLTRACK + CERRADURA RFID — v5
 *  Correcciones aplicadas:
 *    1. logEvent: recorte real a MAX_EVENTS (SPIFFS no crece infinito)
 *    2. WebSocket reintegrado (actualizaciones en tiempo real)
 *    3. GPIO 16/17 eliminados (no disponibles en DEVKIT-V1 30 pines)
 *       → Slot 3: GPIO 15  |  Slot 4: GPIO 2
 *    4. LED builtin eliminado (GPIO 2 reasignado a IR Slot 4)
 *  Diseño: usuario activo sin timeout (hasta que otro acceda)
 * ============================================================
 *
 *  PINOUT RC522 → ESP32 (DEVKIT-V1 30 pines):
 *    SDA  → GPIO 5  (D5)    SCK  → GPIO 18 (D18)
 *    MOSI → GPIO 23 (D23)   MISO → GPIO 19 (D19)
 *    RST  → GPIO 27 (D27)   3.3V → Pin 1   GND → Pin 2
 *
 *  CERRADURA:
 *    LED Verde  → GPIO 26 (D26)
 *    LED Rojo   → GPIO 25 (D25)
 *    Buzzer     → GPIO 33 (D33)
 *    Solenoide  → GPIO 12 (D12)  (usar transistor/relé)
 *
 *  SENSORES IR (DO digital, LOW = presente):
 *    Slot  1→GPIO13(D13)   2→GPIO14(D14)   3→GPIO15(D15)   4→GPIO2(D2)
 *    Slot  5→GPIO21(D21)   6→GPIO22(D22)   7→GPIO32(D32)   8→GPIO34(D34)*
 *    Slot  9→GPIO35(D35)*  10→GPIO4(D4)   11→GPIO36(VP)*  12→GPIO39(VN)*
 *    (* = input-only, sin pull-up interno, requiere pull-up externo 10kΩ)
 * ============================================================
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "dashboard_html.h"   // ← HTML del dashboard separado

// ============================================================
// WIFI
// ============================================================
const char* WIFI_SSID     = "ELO-FOREST";
const char* WIFI_PASSWORD = "Elo.forest1935.2024";

// ============================================================
// USUARIOS RFID
// ============================================================
struct RFIDUser { const char* uid; const char* name; };
const RFIDUser USERS[] = {
  { "1A212828", "Benjamin V." },
  { "A97EC6B2", "Arturo F."  },
  { "C29A713D", "Mario M."   }
};
const int NUM_USERS = sizeof(USERS) / sizeof(USERS[0]);

// ============================================================
// NOMBRES E ÍCONOS DE HERRAMIENTAS
// ============================================================
const char* TOOL_NAMES[12] = {
  "Martillo",        "Destornillador+", "Llave inglesa", "Alicate",
  "Sierra manual",   "Cinta metrica",   "Taladro",       "Nivel",
  "Llave torx",      "Tijeras acero",   "Desarmador plano", "Pinzas punta"
};
const char* TOOL_ICONS[12] = {
  "🔨","🔧","🔩","🪛","🪚","📏","🔌","📐","🔑","✂️","🪛","🔧"
};

// ============================================================
// PINES
// ============================================================
#define RFID_SS_PIN   5
#define RFID_RST_PIN  27
#define PIN_LED_VERDE 26
#define PIN_LED_ROJO  25
#define PIN_BUZZER    33
#define PIN_SOLENOIDE 12

// GPIO 16 y 17 no disponibles en DEVKIT-V1 de 30 pines (flash interna)
// GPIO 2 (ex LED builtin) reasignado a Slot 4
const uint8_t IR_PINS[12] = {
  13, 14, 15,  2,   // Slots  1-4
  21, 22, 32, 34,   // Slots  5-8
  35,  4, 36, 39    // Slots 9-12
};

// ============================================================
// TIEMPOS
// ============================================================
#define TIEMPO_SOLENOIDE    3000
#define TIEMPO_BEEP_OK      1000
#define TIEMPO_ALERTA       3000
#define INTERVALO_PARPADEO   250
#define DEBOUNCE_MS           80

// ============================================================
// SPIFFS
// ============================================================
#define MAX_EVENTS       100
#define SPIFFS_LOG_FILE  "/events.json"

// ============================================================
// OBJETOS
// ============================================================
MFRC522        rfid(RFID_SS_PIN, RFID_RST_PIN);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============================================================
// ESTADO GLOBAL
// ============================================================
char     currentUser[32] = "";     // Usuario activo (sin timeout)

// Solenoide (no bloqueante)
bool          solenoideActivo = false;
unsigned long solenoideInicio = 0;

// Alerta (no bloqueante)
bool          alertaActiva    = false;
unsigned long alertaInicio    = 0;
unsigned long ultimoParpadeo  = 0;
bool          estadoAlerta    = false;

// Beep (no bloqueante)
bool          beepActivo  = false;
unsigned long beepInicio  = 0;

// IR
bool          lastIR[12];
unsigned long debounceMs[12];
bool          toolTaken[12];
char          toolOwner[12][32];

// ============================================================
// SPIFFS — INICIALIZAR
// ============================================================
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Error al inicializar");
    return;
  }
  if (!SPIFFS.exists(SPIFFS_LOG_FILE)) {
    File f = SPIFFS.open(SPIFFS_LOG_FILE, "w");
    f.print("[]");
    f.close();
  }
  Serial.println("[SPIFFS] Listo.");
}

// ============================================================
// SPIFFS — GUARDAR EVENTO (con recorte real a MAX_EVENTS)
// ============================================================
void logEvent(uint8_t toolId, const char* user,
              const char* returnedBy, bool taken) {

  // --- Leer array existente ---
  DynamicJsonDocument doc(16384);
  {
    File f = SPIFFS.open(SPIFFS_LOG_FILE, "r");
    if (f) {
      DeserializationError err = deserializeJson(doc, f);
      f.close();
      if (err || !doc.is<JsonArray>()) doc.set(JsonArray());
    } else {
      doc.set(JsonArray());
    }
  }

  JsonArray arr = doc.as<JsonArray>();

  // --- Agregar nuevo evento ---
  JsonObject ev = arr.createNestedObject();
  ev["toolId"]     = toolId;
  ev["user"]       = user;
  ev["returnedBy"] = returnedBy ? returnedBy : "";
  ev["timestamp"]  = millis() / 1000;
  ev["taken"]      = taken;

  // --- Recorte real: si supera MAX_EVENTS, eliminar los más antiguos ---
  if ((int)arr.size() > MAX_EVENTS) {
    int excess = (int)arr.size() - MAX_EVENTS;

    DynamicJsonDocument trimmed(16384);
    JsonArray newArr = trimmed.to<JsonArray>();

    for (int i = excess; i < (int)arr.size(); i++) {
      newArr.add(arr[i]);
    }

    File f = SPIFFS.open(SPIFFS_LOG_FILE, "w");
    serializeJson(newArr, f);
    f.close();

    Serial.printf("[SPIFFS] Recortado a %d eventos.\n", MAX_EVENTS);
    return;
  }

  // --- Guardar sin recorte ---
  File f = SPIFFS.open(SPIFFS_LOG_FILE, "w");
  serializeJson(arr, f);
  f.close();

  if (taken)
    Serial.printf("[LOG] Tool %d TOMADA por %s\n", toolId, user);
  else
    Serial.printf("[LOG] Tool %d DEVUELTA por %s (tomada por %s)\n",
                  toolId, returnedBy, user);
}

// ============================================================
// SPIFFS — LEER TODOS LOS EVENTOS
// ============================================================
void getAllEvents(JsonArray& out) {
  File f = SPIFFS.open(SPIFFS_LOG_FILE, "r");
  if (!f) return;

  DynamicJsonDocument doc(16384);
  if (deserializeJson(doc, f) == DeserializationError::Ok && doc.is<JsonArray>()) {
    for (JsonObject ev : doc.as<JsonArray>()) out.add(ev);
  }
  f.close();
}

// ============================================================
// UTILIDADES
// ============================================================
String uidToString(byte* buf, byte len) {
  String s = "";
  for (byte i = 0; i < len; i++) {
    if (buf[i] < 0x10) s += "0";
    s += String(buf[i], HEX);
  }
  s.toUpperCase();
  return s;
}

const char* findUser(const String& uid) {
  for (int i = 0; i < NUM_USERS; i++)
    if (uid == USERS[i].uid) return USERS[i].name;
  return nullptr;
}

// ============================================================
// WEBSOCKET — enviar estado completo a un cliente o a todos
// ============================================================
void sendFullState(AsyncWebSocketClient* client = nullptr) {
  DynamicJsonDocument doc(4096);
  doc["type"]        = "full_state";
  doc["currentUser"] = currentUser;

  JsonArray arr = doc.createNestedArray("tools");
  for (int i = 0; i < 12; i++) {
    JsonObject o = arr.createNestedObject();
    o["slot"]    = i + 1;
    o["name"]    = TOOL_NAMES[i];
    o["icon"]    = TOOL_ICONS[i];
    o["present"] = !toolTaken[i];
    o["owner"]   = toolOwner[i];
  }

  String json;
  serializeJson(doc, json);
  if (client) client->text(json);
  else        ws.textAll(json);
}

// Evento puntual (herramienta tomada o devuelta)
void sendToolEvent(int i, const char* evType,
                   const char* taker, const char* returner) {
  DynamicJsonDocument doc(512);
  doc["type"]       = "tool_event";
  doc["slot"]       = i + 1;
  doc["name"]       = TOOL_NAMES[i];
  doc["icon"]       = TOOL_ICONS[i];
  doc["event"]      = evType;
  doc["present"]    = !toolTaken[i];
  doc["owner"]      = taker;
  doc["returnedBy"] = returner ? returner : "";

  String json;
  serializeJson(doc, json);
  ws.textAll(json);
}

// Evento de acceso RFID
void sendAccessEvent(const char* user, bool granted) {
  DynamicJsonDocument doc(256);
  doc["type"]        = "access_event";
  doc["user"]        = user;
  doc["granted"]     = granted;
  doc["currentUser"] = currentUser;

  String json;
  serializeJson(doc, json);
  ws.textAll(json);
}

// ============================================================
// WEBSOCKET — manejador de eventos de conexión
// ============================================================
void onWsEvent(AsyncWebSocket* srv, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WS] Cliente #%u conectado\n", client->id());
    sendFullState(client);
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WS] Cliente #%u desconectado\n", client->id());
  }
}

// ============================================================
// ACCESO
// ============================================================
void accesoPermitido(const char* user) {
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_ROJO,  LOW);

  beepActivo  = true;
  beepInicio  = millis();
  digitalWrite(PIN_BUZZER, HIGH);

  solenoideActivo = true;
  solenoideInicio = millis();
  digitalWrite(PIN_SOLENOIDE, HIGH);

  strncpy(currentUser, user, sizeof(currentUser) - 1);

  Serial.printf("[ACCESO] Usuario activo: %s\n", currentUser);
  sendAccessEvent(user, true);
}

void accesoDenegado(const char* uid) {
  alertaActiva   = true;
  alertaInicio   = millis();
  ultimoParpadeo = millis();
  estadoAlerta   = true;

  Serial.printf("[DENEGADO] UID: %s\n", uid);
  sendAccessEvent(uid, false);
}

// ============================================================
// RFID
// ============================================================
void handleRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid       = uidToString(rfid.uid.uidByte, rfid.uid.size);
  const char* user = findUser(uid);

  Serial.printf("[RFID] %s → %s\n", uid.c_str(), user ? user : "Desconocido");

  if (user) accesoPermitido(user);
  else      accesoDenegado(uid.c_str());

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ============================================================
// IR — SENSORES DE HERRAMIENTAS
// ============================================================
void handleIR() {
  for (int i = 0; i < 12; i++) {
    bool reading = (digitalRead(IR_PINS[i]) == LOW);

    if (reading == lastIR[i]) { debounceMs[i] = millis(); continue; }
    if ((millis() - debounceMs[i]) < DEBOUNCE_MS) continue;

    lastIR[i] = reading;

    if (!reading && !toolTaken[i]) {
      // ── HERRAMIENTA TOMADA ──
      toolTaken[i] = true;
      strncpy(toolOwner[i], currentUser, sizeof(toolOwner[i]) - 1);

      logEvent(i, currentUser, "", true);
      sendToolEvent(i, "salida", currentUser, nullptr);

    } else if (reading && toolTaken[i]) {
      // ── HERRAMIENTA DEVUELTA ──
      char owner[32];
      strncpy(owner, toolOwner[i], sizeof(owner) - 1);

      toolTaken[i]    = false;
      toolOwner[i][0] = '\0';

      logEvent(i, owner, currentUser, false);
      sendToolEvent(i, "entrada", owner, currentUser);
    }
  }
}

// ============================================================
// UPDATE NO BLOQUEANTE (solenoide, beep, alerta)
// ============================================================
void updateSistema() {
  // Solenoide
  if (solenoideActivo && millis() - solenoideInicio > TIEMPO_SOLENOIDE) {
    digitalWrite(PIN_SOLENOIDE, LOW);
    digitalWrite(PIN_LED_VERDE, LOW);
    solenoideActivo = false;
  }

  // Beep
  if (beepActivo && millis() - beepInicio > TIEMPO_BEEP_OK) {
    digitalWrite(PIN_BUZZER, LOW);
    beepActivo = false;
  }

  // Alerta (parpadeo LED rojo + buzzer)
  if (alertaActiva) {
    if (millis() - ultimoParpadeo > INTERVALO_PARPADEO) {
      estadoAlerta = !estadoAlerta;
      digitalWrite(PIN_LED_ROJO, estadoAlerta);
      digitalWrite(PIN_BUZZER,   estadoAlerta);
      ultimoParpadeo = millis();
    }
    if (millis() - alertaInicio > TIEMPO_ALERTA) {
      digitalWrite(PIN_LED_ROJO, LOW);
      digitalWrite(PIN_BUZZER,   LOW);
      alertaActiva = false;
    }
  }
}

// ============================================================
// WEB SERVER + WEBSOCKET
// ============================================================
void setupWeb() {
  // ── WebSocket ──
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // ── API: estado de herramientas ──
  server.on("/api/tools", HTTP_GET, [](AsyncWebServerRequest* req) {
    DynamicJsonDocument doc(4096);
    doc["currentUser"] = currentUser;

    JsonArray tools = doc.createNestedArray("tools");
    for (int i = 0; i < 12; i++) {
      JsonObject t = tools.createNestedObject();
      t["id"]      = i;
      t["name"]    = TOOL_NAMES[i];
      t["icon"]    = TOOL_ICONS[i];
      t["present"] = !toolTaken[i];
      t["owner"]   = toolOwner[i];
    }

    String res; serializeJson(doc, res);
    req->send(200, "application/json", res);
  });

  // ── API: histórico ──
  server.on("/api/events", HTTP_GET, [](AsyncWebServerRequest* req) {
    DynamicJsonDocument doc(16384);
    JsonArray events = doc.createNestedArray("events");
    getAllEvents(events);

    String res; serializeJson(doc, res);
    req->send(200, "application/json", res);
  });

  // ── API: limpiar histórico ──
  server.on("/api/clear-history", HTTP_POST, [](AsyncWebServerRequest* req) {
    File f = SPIFFS.open(SPIFFS_LOG_FILE, "w");
    f.print("[]");
    f.close();
    req->send(200, "application/json", "{\"status\":\"ok\"}");
    Serial.println("[SPIFFS] Histórico limpiado.");
  });

  // ── Dashboard HTML ──
  // El HTML vive en dashboard_html.h para mantener este archivo limpio
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    String html = buildDashboardHTML(TOOL_NAMES, TOOL_ICONS, 12);
    req->send(200, "text/html", html);
  });

  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "No encontrado");
  });

  server.begin();
  Serial.println("[WEB] Servidor HTTP + WebSocket activo.");
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n══════════════════════════════");
  Serial.println("  TOOLTRACK v5 — Iniciando");
  Serial.println("══════════════════════════════");

  // GPIO cerradura
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO,  OUTPUT);
  pinMode(PIN_BUZZER,    OUTPUT);
  pinMode(PIN_SOLENOIDE, OUTPUT);

  // Sensores IR
  for (int i = 0; i < 12; i++) {
    if (IR_PINS[i] >= 34) pinMode(IR_PINS[i], INPUT);
    else                   pinMode(IR_PINS[i], INPUT_PULLUP);

    lastIR[i]       = (digitalRead(IR_PINS[i]) == LOW);
    debounceMs[i]   = millis();
    toolTaken[i]    = false;
    toolOwner[i][0] = '\0';

    Serial.printf("  Slot %02d [%-18s] %s\n",
      i + 1, TOOL_NAMES[i], lastIR[i] ? "PRESENTE" : "AUSENTE");
  }

  // SPIFFS
  initSPIFFS();

  // SPI + RFID
  SPI.begin(18, 19, 23, RFID_SS_PIN);
  rfid.PCD_Init();
  Serial.println("[RFID] RC522 listo.");

  // WiFi
  Serial.printf("[WiFi] Conectando a %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int att = 0;
  while (WiFi.status() != WL_CONNECTED && att < 40) {
    delay(500); Serial.print("."); att++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[WiFi] Falló. Reiniciando...");
    delay(3000); ESP.restart();
  }
  Serial.printf("\n[WiFi] IP: http://%s\n", WiFi.localIP().toString().c_str());

  setupWeb();

  Serial.println("\n[SISTEMA] Listo. Acerca una tarjeta RFID...\n");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  handleRFID();
  handleIR();
  updateSistema();

  // Limpiar clientes WS desconectados cada 5 s
  static uint32_t lastClean = 0;
  if (millis() - lastClean > 5000) {
    ws.cleanupClients();
    lastClean = millis();
  }

  delay(5);
}
