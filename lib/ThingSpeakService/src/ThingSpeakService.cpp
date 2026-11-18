#include <ThingSpeakService.h>

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <math.h>

#include <secrets.h>

ThingSpeakService::ThingSpeakService() : lastPublishMs_(0) {}

String ThingSpeakService::lastError() const {
  return lastError_;
}

bool ThingSpeakService::publishNow(const TelemetryData &data) {
  lastError_ = "";

  if (WiFi.status() != WL_CONNECTED) {
    lastError_ = "Sin WiFi";
    return false;
  }
  if (String(THINGSPEAK_WRITE_API_KEY).length() == 0 ||
      String(THINGSPEAK_WRITE_API_KEY).startsWith("PEGAR")) {
    lastError_ = "API Key vacia";
    return false;
  }
  if (isnan(data.temperatureC) || isnan(data.humidityPercent)) {
    lastError_ = "DHT sin lectura";
    return false;
  }

  String url = "http://api.thingspeak.com/update?api_key=";
  url += THINGSPEAK_WRITE_API_KEY;
  url += "&field1=";
  url += String(data.temperatureC, 2);
  url += "&field2=";
  url += String(data.humidityPercent, 2);

  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, url)) {
    lastError_ = "HTTP begin fallo";
    return false;
  }

  const int statusCode = http.GET();
  const String response = http.getString();
  http.end();

  Serial.print("ThingSpeak HTTP=");
  Serial.print(statusCode);
  Serial.print(" body=");
  Serial.println(response);

  if (statusCode != HTTP_CODE_OK) {
    lastError_ = "HTTP " + String(statusCode);
    return false;
  }

  const long entryId = response.toInt();
  if (entryId <= 0) {
    // 0 = API key invalida o updates demasiado seguidos (<15s en cuenta free).
    lastError_ = "Respuesta 0 (API Key o espera 15s)";
    return false;
  }

  lastError_ = "";
  return true;
}

void ThingSpeakService::publishIfDue(const TelemetryData &data) {
  const unsigned long now = millis();
  if (now - lastPublishMs_ < INTERVALO_THINGSPEAK_MS) {
    return;
  }

  if (publishNow(data)) {
    lastPublishMs_ = now;
  }
}
