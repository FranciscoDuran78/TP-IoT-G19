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
bool wifiAnunciadoEnOled = false;

void setup() {
  Serial.begin(115200);
  delay(300);
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
  // Reintento periodico con disconnect previo (evita "sta is connecting").
  if (WiFi.status() != WL_CONNECTED &&
      ahora - ultimoIntentoWiFi >= INTERVALO_WIFI_REINTENTO_MS) {
    Serial.print("Reintento WiFi SSID=");
    Serial.println(WIFI_SSID);
    wifiAnunciadoEnOled = false;
    WiFi.disconnect(true, true);
    delay(200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    ultimoIntentoWiFi = ahora;
  }

  if (ahora - ultimaLecturaSensores >= 2000UL) {
    hardware.refreshSensors();
    ultimaLecturaSensores = ahora;
    Serial.print("WiFi status=");
    Serial.print(static_cast<int>(WiFi.status()));
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(" IP=");
      Serial.print(WiFi.localIP());
      if (!wifiAnunciadoEnOled) {
        hardware.showStatusScreen(String("WiFi OK ") + WiFi.localIP().toString());
        wifiAnunciadoEnOled = true;
      }
    }
    Serial.println();
  }

  hardware.updateAutomaticControl();
  telegramBot.poll();
  thingSpeak.publishIfDue(hardware.snapshot());

  delay(10);
}
