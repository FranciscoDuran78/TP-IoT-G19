# TPI IoT - PlatformIO + Wokwi

Proyecto inicial para el Trabajo Práctico Integrador de Tecnologías para la Automatización. El entorno de desarrollo es PlatformIO con framework Arduino y simulación local en Wokwi.

## Estructura

```text
TPI IoT/
├── platformio.ini
├── wokwi.toml
├── diagram.json
├── include/
│   ├── TPIConfig.h
│   ├── secrets.h
│   └── secrets.example.h
├── lib/
│   ├── HardwareController/src/
│   ├── TelegramBotService/src/
│   └── ThingSpeakService/src/
└── src/
    └── main.cpp
```

## Primeros pasos

1. Abrir la carpeta `TPI IoT` como proyecto en VS Code con PlatformIO.
2. Editar `include/secrets.h` con la red Wi-Fi, el token del bot y la Write API Key de ThingSpeak.
3. Compilar con la tarea **PlatformIO: Build** o con `pio run`. PlatformIO descargará las bibliotecas declaradas en `platformio.ini`.
4. Para usar la placa física, conectar la ESP32 y ejecutar **PlatformIO: Upload** o `pio run --target upload`.
5. Para Wokwi, abrir `diagram.json` y ejecutar **Wokwi: Start Simulator** después de compilar. `wokwi.toml` apunta al firmware y al archivo ELF que genera esta configuración.
6. Para observar mensajes, abrir el monitor serie a 115200 baudios.

La simulación usa `Wokwi-GUEST` como red inicial. Para Telegram y ThingSpeak se necesitan credenciales reales. El control local puede desarrollarse primero sin completar esas claves.

## Qué contiene el código inicial

- `src/main.cpp`: inicialización del ESP32, conexión Wi-Fi, lectura periódica y ciclo principal.
- `lib/HardwareController`: DHT22, potenciómetro, LEDs, OLED, modos y búfer de comandos.
- `lib/TelegramBotService`: recepción y atención de comandos.
- `lib/ThingSpeakService`: envío periódico y bajo demanda de temperatura y humedad.
- `include/TPIConfig.h`: pines, límites e intervalos.

## Comandos previstos

- `/start`: menú de texto con formato Markdown y emojis.
- `/umbral <valor>`: configura el umbral en voltios.
- `/estado`: informa temperatura, humedad, voltaje del potenciómetro y estados de los LEDs.
- `/autoon` y `/autooff`: activa o desactiva el control automático.
- `/thingup`: envía una lectura a ThingSpeak.
- `/displayled`, `/displaypot` y `/displaydht`: muestra el estado solicitado en la OLED y mantiene los últimos tres comandos válidos en la parte inferior.

El enunciado no da nombres para comandos manuales de LEDs. Como propuesta para habilitar el modo manual, el código agrega `/ledazulon`, `/ledazuloff`, `/ledverdeon` y `/ledverdeoff`; confirmen esos nombres con el equipo docente.

## Puntos del enunciado para confirmar

1. **Umbral:** el documento da `0.5 V` por defecto, pero indica un rango válido de `1.0 a 1.5 V`. El código mantiene ambos datos literalmente: inicia en `0.5 V` y vuelve a ese valor si recibe uno fuera del rango.
2. **Línea base automática:** al ejecutar `/autoon`, el código toma el voltaje actual del potenciómetro como línea base y enciende el LED azul cuando la lectura supera `línea base + umbral`. Esta interpretación está aislada en `HardwareController::updateAutomaticControl()` para poder cambiarla si la cátedra define otra regla.
3. **LED verde:** el enunciado enumera el LED verde, pero no especifica su comportamiento automático. Por ahora inicia apagado y puede controlarse mediante los comandos manuales propuestos.
4. **ThingSpeak:** se usa `field1 = temperatura` y `field2 = humedad`. Cambiar el mapeo si el canal del grupo usa otros campos.
5. **Entrega `.ino`:** PlatformIO usa `src/main.cpp` como sketch principal. El enunciado también solicita un archivo `.ino`; al preparar la entrega, generar una copia final del sketch con esa extensión y adjuntarla junto con el proyecto Wokwi y sus bibliotecas propias.

## Notas de credenciales

`include/secrets.h` contiene marcadores para completar y está excluido por `.gitignore`. No subir tokens ni claves a repositorios compartidos. `include/secrets.example.h` queda como plantilla. El cliente HTTPS de Telegram usa `setInsecure()` como configuración de prototipo; para una entrega final, reemplazarlo por verificación de certificado.
