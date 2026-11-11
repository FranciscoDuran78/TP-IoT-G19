#include <ThingSpeakService.h>

#include <HTTPClient.h>
#include <WiFi.h>
#include <math.h>

#include <secrets.h>

ThingSpeakService::ThingSpeakService() : lastPublishMs_(0) {}

bool ThingSpeakService::publishNow(const TelemetryData &data) {
  if (WiFi.status() != WL_CONNECTED ||
      String(THINGSPEAK_WRITE_API_KEY).length() == 0 ||
      String(THINGSPEAK_WRITE_API_KEY).startsWith("PEGAR") ||
      isnan(data.temperatureC) || isnan(data.humidityPercent)) {
    return false;
  }

  String url = "http://api.thingspeak.com/update?api_key=";
  url += THINGSPEAK_WRITE_API_KEY;
  url += "&field1=";
  url += String(data.temperatureC, 2);
  url += "&field2=";
  url += String(data.humidityPercent, 2);

  HTTPClient http;
  if (!http.begin(url)) {
    return false;
  }

  const int statusCode = http.GET();
  const String response = http.getString();
  http.end();

  return statusCode == HTTP_CODE_OK && response.toInt() > 0;
}

void ThingSpeakService::publishIfDue(const TelemetryData &data) {
  const unsigned long now = millis();
  if (now - lastPublishMs_ < INTERVALO_THINGSPEAK_MS) {
    return;
  }

  lastPublishMs_ = now;
  publishNow(data);
}
