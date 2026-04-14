# 🔧 ToolTrack

> Sistema inteligente de control de inventario de herramientas con monitoreo en tiempo real.

![Estado](https://img.shields.io/badge/estado-en%20desarrollo-orange)
![Plataforma](https://img.shields.io/badge/plataforma-ESP32-green)
![Licencia](https://img.shields.io/badge/licencia-MIT-blue)

---

## 📌 Descripción

ToolTrack es un sistema de control de herramientas diseñado para talleres y entornos técnicos que necesitan mantener organización y seguridad en el uso de sus equipos. Permite registrar en tiempo real qué herramienta es retirada, quién la toma y cuándo lo hace.

Su objetivo principal es evitar la pérdida de herramientas y mejorar la trazabilidad dentro del espacio de trabajo, utilizando un `ESP32` como núcleo y una interfaz web integrada para visualización y gestión.

---

## ⚙️ Características

- 🔐 Control de uso de herramientas (retiro y devolución)
- 🧾 Registro (log) en tiempo real
- 👤 Identificación del usuario que retira cada herramienta
- 🛠️ Seguimiento de herramientas retiradas
- 🌐 Interfaz web accesible desde cualquier navegador
- ⚡ Sistema embebido eficiente basado en ESP32
- 📡 Comunicación bidireccional vía WebSocket

---

## 🛠️ Stack tecnológico

| Capa | Tecnología |
|------|-----------|
| Hardware | ESP32 + RC522 RFID + Sensores IR |
| Firmware | Arduino IDE — C/C++ |
| Frontend | HTML/CSS/JS almacenado en `PROGMEM` |
| Comunicación | WiFi — ESP32 WebServer + WebSocket |

---

## 🚀 Primeros pasos

### Requisitos previos

- ESP32 (cualquier variante con WiFi integrado)
- Arduino IDE instalado con soporte para ESP32
- Librerías: `ESPAsyncWebServer`, `MFRC522`
- Acceso a red WiFi local

### Instalación

```bash
git clone https://github.com/your-username/tooltrack.git
cd tooltrack
```

1. Abre el proyecto en Arduino IDE
2. Selecciona la placa ESP32 y el puerto correcto
3. Configura las credenciales WiFi en el archivo principal
4. Compila y sube el código al dispositivo

---

## 📖 Uso

1. Conecta el ESP32 a la red WiFi
2. Abre un navegador y accede a la IP del dispositivo
3. Visualiza las herramientas disponibles en el dashboard
4. Registra la retirada y devolución de herramientas
5. Consulta el historial de actividad en tiempo real

---

## 🎯 Casos de uso

- 🔧 Talleres mecánicos
- 🔬 Laboratorios técnicos
- 🏫 Instituciones educativas
- 🤝 Espacios de trabajo compartidos

---

## 📌 Roadmap

- [ ] Sistema de autenticación más robusto
- [ ] Integración con base de datos externa
- [ ] Notificaciones en tiempo real
- [ ] Dashboard más avanzado con métricas
- [ ] Integración con RFID / NFC

---

## ⚠️ Estado del proyecto

🚧 **En desarrollo** — este proyecto está en progreso y puede sufrir cambios importantes.

---

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Puedes abrir un *issue* o enviar un *pull request* con tus mejoras.

---

## 📄 Licencia

Este proyecto está bajo la licencia [MIT](LICENSE).
