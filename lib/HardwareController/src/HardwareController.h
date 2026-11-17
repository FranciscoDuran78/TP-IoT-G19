#pragma once

#include <Adafruit_SH110X.h>
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
  void showStatusScreen(const String &wifiLine);
  bool oledReady() const;

 private:
  float readPotentiometerV();
  void drawCommandHistory();
  bool initOled();

  DHT dht_;
  Adafruit_SH1106G display_;
  TelemetryData currentData_;
  float thresholdV_;
  float automaticBaselineV_;
  bool automaticMode_;
  bool oledReady_;
  String commandHistory_[3];
};
