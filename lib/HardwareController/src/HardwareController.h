#pragma once

#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Wire.h>

#include <TPIConfig.h>

struct TelemetryData {
  float temperatureC;
  float humidityPercent;
  float potentiometerV;
  bool greenLedOn;
  bool blueLedOn;
};

class HardwareController {
 public:
  HardwareController();

  void begin();
  TelemetryData refreshSensors();
  TelemetryData snapshot() const;
  void updateAutomaticControl();

  void setAutomaticMode(bool enabled);
  bool automaticMode() const;
  float automaticBaselineV() const;

  bool setThresholdV(float value);
  float thresholdV() const;

  void setGreenLed(bool enabled);
  void setBlueLed(bool enabled);

  void rememberCommand(const String &command);
  bool showComponent(const String &component);
  void showUnknownDisplayCommand();
  void showMessage(const String &message);

 private:
  float readPotentiometerV();
  void drawCommandHistory();

  DHT dht_;
  Adafruit_SSD1306 display_;
  TelemetryData currentData_;
  float thresholdV_;
  float automaticBaselineV_;
  bool automaticMode_;
  String commandHistory_[3];
};
