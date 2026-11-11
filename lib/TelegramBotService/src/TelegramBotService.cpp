#include <TelegramBotService.h>

#include <WiFi.h>
#include <math.h>

#include <secrets.h>

namespace {
String formatValue(float value, const String &unit) {
  if (isnan(value)) {
    return "sin lectura";
  }
  return String(value, 1) + unit;
}
}

TelegramBotService::TelegramBotService(HardwareController &hardware,
                                       ThingSpeakService &thingSpeak)
    : bot_(TELEGRAM_BOT_TOKEN, secureClient_),
      hardware_(hardware),
      thingSpeak_(thingSpeak),
      lastPollMs_(0) {}

void TelegramBotService::begin() {
  // Facilita el prototipo. Para una version final, configurar certificado TLS.
  secureClient_.setInsecure();
}

void TelegramBotService::poll() {
  if (WiFi.status() != WL_CONNECTED ||
      String(TELEGRAM_BOT_TOKEN).length() == 0 ||
      String(TELEGRAM_BOT_TOKEN).startsWith("PEGAR")) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastPollMs_ < INTERVALO_TELEGRAM_MS) {
    return;
  }
  lastPollMs_ = now;

  int messageCount = bot_.getUpdates(bot_.last_message_received + 1);
  while (messageCount > 0) {
    for (int i = 0; i < messageCount; ++i) {
      handleMessage(bot_.messages[i].chat_id, bot_.messages[i].text);
    }
    messageCount = bot_.getUpdates(bot_.last_message_received + 1);
  }
}

void TelegramBotService::handleMessage(const String &chatId,
                                       const String &messageText) {
  String text = messageText;
  text.trim();

  const int firstSpace = text.indexOf(' ');
  String command = firstSpace < 0 ? text : text.substring(0, firstSpace);
  String argument = firstSpace < 0 ? "" : text.substring(firstSpace + 1);
  argument.trim();

  const int botSuffix = command.indexOf('@');
  if (botSuffix >= 0) {
    command = command.substring(0, botSuffix);
  }
  command.toLowerCase();

  if (command == "/start") {
    sendStartMenu(chatId);
    hardware_.rememberCommand(text);
    return;
  }

  if (command == "/estado") {
    sendStatus(chatId);
    hardware_.rememberCommand(text);
    return;
  }

  if (command == "/umbral") {
    const bool valid = hardware_.setThresholdV(argument.toFloat());
    if (valid) {
      sendReply(chatId, "Umbral configurado en " + String(hardware_.thresholdV(), 2) + " V.");
    } else {
      sendReply(chatId, "Valor fuera de 1.0 a 1.5 V. Se restablecio el valor original de 0.5 V.");
    }
    hardware_.rememberCommand(text);
    return;
  }

  if (command == "/autoon") {
    hardware_.setAutomaticMode(true);
    sendReply(chatId, "Modo automatico activado. Linea base: " +
                          String(hardware_.automaticBaselineV(), 2) + " V.");
    hardware_.rememberCommand(text);
    return;
  }

  if (command == "/autooff") {
    hardware_.setAutomaticMode(false);
    sendReply(chatId, "Modo automatico desactivado. Modo manual activado.");
    hardware_.rememberCommand(text);
    return;
  }

  if (command == "/thingup") {
    const bool sent = thingSpeak_.publishNow(hardware_.refreshSensors());
    sendReply(chatId, sent ? "Datos enviados a ThingSpeak." :
                             "No se pudieron enviar los datos. Revisa Wi-Fi, sensor y API Key.");
    if (sent) hardware_.rememberCommand(text);
    return;
  }

  if (command.startsWith("/display")) {
    const String component = command.substring(8);
    if (component == "led" || component == "pot" || component == "dht") {
      hardware_.rememberCommand(text);
      hardware_.showComponent(component);
      sendReply(chatId, "Estado mostrado en la pantalla OLED.");
    } else {
      hardware_.showUnknownDisplayCommand();
      sendReply(chatId, "Componente no reconocido. Usa /displayled, /displaypot o /displaydht.");
    }
    return;
  }

  // Comandos manuales propuestos porque el enunciado no especifica su sintaxis.
  if (command == "/ledazulon" || command == "/ledazuloff" ||
      command == "/ledverdeon" || command == "/ledverdeoff") {
    const bool enabled = command.endsWith("on");
    if (command.startsWith("/ledazul")) hardware_.setBlueLed(enabled);
    else hardware_.setGreenLed(enabled);
    sendReply(chatId, "LED actualizado.");
    hardware_.rememberCommand(text);
    return;
  }

  sendReply(chatId, "Comando no reconocido. Envia /start para ver el menu.");
}

void TelegramBotService::sendStartMenu(const String &chatId) {
  const String menu =
      "🤖 *TPI IoT*\n\n"
      "📊 /estado - consultar sensores y LEDs\n"
      "🎚️ /umbral 1.2 - configurar el umbral\n"
      "⚙️ /autoon - activar control automatico\n"
      "🖐️ /autooff - pasar a control manual\n"
      "☁️ /thingup - enviar datos a ThingSpeak\n"
      "🖥️ /displayled, /displaypot, /displaydht\n"
      "💡 /ledazulon, /ledazuloff, /ledverdeon, /ledverdeoff - control manual propuesto";
  bot_.sendMessage(chatId, menu, "Markdown");
}

void TelegramBotService::sendStatus(const String &chatId) {
  const TelemetryData data = hardware_.refreshSensors();
  String status = "Estado del sistema\n";
  status += "Temperatura: " + formatValue(data.temperatureC, " C") + "\n";
  status += "Humedad: " + formatValue(data.humidityPercent, " %") + "\n";
  status += "Potenciometro: " + String(data.potentiometerV, 2) + " V\n";
  status += "LED verde: ";
  status += data.greenLedOn ? "encendido\n" : "apagado\n";
  status += "LED azul: ";
  status += data.blueLedOn ? "encendido\n" : "apagado\n";
  status += hardware_.automaticMode() ? "Modo: automatico" : "Modo: manual";
  sendReply(chatId, status);
}

void TelegramBotService::sendReply(const String &chatId, const String &text) {
  bot_.sendMessage(chatId, text, "");
}
