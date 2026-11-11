#include <WiFi.h>
#include <TPIConfig.h>
#include <secrets.h>
#include <HardwareController.h>
#include <ThingSpeakService.h>
#include <TelegramBotService.h>

HardwareController hardware;
ThingSpeakService thingSpeak;
TelegramBotService telegramBot(hardware, thingSpeak);

unsigned long ultimaLecturaSensores = 0;
unsigned long ultimoIntentoWiFi = 0;

void setup() {
  Serial.begin(115200);
  hardware.begin();
  hardware.refreshSensors();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  ultimoIntentoWiFi = millis();

  telegramBot.begin();
  Serial.println("TPI IoT iniciado");
}

void loop() {
  const unsigned long ahora = millis();

  if (WiFi.status() != WL_CONNECTED &&
      ahora - ultimoIntentoWiFi >= INTERVALO_WIFI_REINTENTO_MS) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    ultimoIntentoWiFi = ahora;
  }

  if (ahora - ultimaLecturaSensores >= 2000UL) {
    hardware.refreshSensors();
    ultimaLecturaSensores = ahora;
  }

  hardware.updateAutomaticControl();
  telegramBot.poll();
  thingSpeak.publishIfDue(hardware.snapshot());

  delay(10);
}
