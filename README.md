# TPI IoT G19 — TA 4k01 (consigna TPI-01)

Sistema embebido con ESP32: DHT22 + potenciómetro, LEDs, OLED, **ThingSpeak** y bot de **Telegram**.

Enunciado de referencia: [`consigna/TA4k01-TPI-01.pdf`](consigna/TA4k01-TPI-01.pdf).

## Hardware

| Periférico | Pin |
|------------|-----|
| DHT22 | GPIO 33 |
| Potenciómetro | GPIO 32 |
| LED verde | GPIO 23 |
| LED azul | GPIO 2 |
| OLED SDA / SCL | GPIO 21 / 22 |

## Estructura

```text
TP-IoT-G19/
├── platformio.ini
├── wokwi.toml / diagram.json
├── consigna/TA4k01-TPI-01.pdf
├── include/          TPIConfig.h, secrets.example.h
├── lib/              HardwareController, TelegramBotService, ThingSpeakService
├── src/main.cpp
└── entrega/TPI_IoT_G19.ino
```

## Setup

1. Copiar `include/secrets.example.h` → `include/secrets.h` y completar Wi-Fi, token Telegram y Write API Key de ThingSpeak.
2. Compilar con PlatformIO (`pio run` o **Build**).
3. Placa: **Upload** + monitor 115200.
4. Wokwi: abrir `diagram.json` → **Start Simulator** (después de compilar).

`secrets.h` no se sube a git.

ThingSpeak: `field1` = temperatura, `field2` = humedad.

## Comandos Telegram (TPI-01)

| Comando | Acción |
|---------|--------|
| `/start` | Menú Markdown con emojis (texto plano) |
| `/umbral <valor>` | Umbral en [1.0; 1.5] V; default 0.5 V; fuera de rango → vuelve a 0.5 |
| `/estado` | Temp, humedad, pot y LEDs |
| `/autoon` | Modo auto: toma el pot actual como línea base; LED azul si pot > base + umbral |
| `/autooff` | Modo manual |
| `/thingup` | Envía temp/humedad a ThingSpeak |
| `/displayled` `/displaypot` `/displaydht` | OLED + últimos 3 comandos OK |

Extras para modo manual (el enunciado no nombra sintaxis): `/ledazulon`, `/ledazuloff`, `/ledverdeon`, `/ledverdeoff`.

## Checklist entrega UV (`4k01TP02/26`)

- [ ] `secrets.h` con credenciales reales (no subirlas)
- [ ] Prueba en placa UTN (lunes)
- [ ] Wokwi + `entrega/TPI_IoT_G19.ino` + libs propias
- [ ] Canal ThingSpeak del grupo
- [ ] Integrantes en selector UV (máx. 6)

Fechas: grupos pares 11/11 · impares **18/11**.
