#include <HardwareController.h>

#include <math.h>

HardwareController::HardwareController()
    : dht_(PIN_DHT22, DHT22),
      display_(128, 64, &Wire),
      thresholdV_(UMBRAL_DEFAULT_V),
      automaticBaselineV_(0.0f),
      automaticMode_(false),
      oledReady_(false) {
  currentData_.temperatureC = NAN;
  currentData_.humidityPercent = NAN;
  currentData_.potentiometerV = 0.0f;
  currentData_.greenLedOn = false;
  currentData_.blueLedOn = false;
}

bool HardwareController::initOled() {
  // Misma inicializacion que el apunte / TP2 de la placa UTN (SH1106).
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  delay(50);

  if (!display_.begin(OLED_DIRECCION_I2C, true)) {
    Serial.println("No se pudo inicializar OLED SH1106");
    oledReady_ = false;
    return false;
  }

  oledReady_ = true;
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SH110X_WHITE);
  display_.setCursor(0, 0);
  display_.println(F("TPI IoT G19"));
  display_.println(F("OLED SH1106 OK"));
  display_.display();
  Serial.println("OLED SH1106 OK");
  return true;
}

void HardwareController::begin() {
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);
  pinMode(PIN_POTENCIOMETRO, INPUT);
  analogReadResolution(12);

  setGreenLed(false);
  setBlueLed(false);
  dht_.begin();
  initOled();
}

bool HardwareController::oledReady() const {
  return oledReady_;
}

void HardwareController::showStatusScreen(const String &wifiLine) {
  if (!oledReady_) {
    return;
  }
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SH110X_WHITE);
  display_.setCursor(0, 0);
  display_.println(F("TPI IoT G19"));
  display_.println(wifiLine);
  display_.print(F("Temp: "));
  if (isnan(currentData_.temperatureC)) {
    display_.println(F("--"));
  } else {
    display_.print(currentData_.temperatureC, 1);
    display_.println(F(" C"));
  }
  display_.print(F("Hum: "));
  if (isnan(currentData_.humidityPercent)) {
    display_.println(F("--"));
  } else {
    display_.print(currentData_.humidityPercent, 1);
    display_.println(F(" %"));
  }
  display_.display();
}

TelemetryData HardwareController::refreshSensors() {
  currentData_.temperatureC = dht_.readTemperature();
  currentData_.humidityPercent = dht_.readHumidity();
  currentData_.potentiometerV = readPotentiometerV();
  currentData_.greenLedOn = digitalRead(PIN_LED_VERDE) == HIGH;
  currentData_.blueLedOn = digitalRead(PIN_LED_AZUL) == HIGH;
  return currentData_;
}

TelemetryData HardwareController::snapshot() const {
  return currentData_;
}

float HardwareController::readPotentiometerV() {
  const int raw = analogRead(PIN_POTENCIOMETRO);
  return (static_cast<float>(raw) * ADC_VOLTAGE_MAX) / ADC_MAXIMO;
}

void HardwareController::updateAutomaticControl() {
  currentData_.potentiometerV = readPotentiometerV();
  if (!automaticMode_) {
    return;
  }

  const float limite = automaticBaselineV_ + thresholdV_;
  setBlueLed(currentData_.potentiometerV > limite);
}

void HardwareController::setAutomaticMode(bool enabled) {
  if (enabled) {
    automaticBaselineV_ = readPotentiometerV();
    currentData_.potentiometerV = automaticBaselineV_;
  }
  automaticMode_ = enabled;
}

bool HardwareController::automaticMode() const {
  return automaticMode_;
}

float HardwareController::automaticBaselineV() const {
  return automaticBaselineV_;
}

bool HardwareController::setThresholdV(float value) {
  if (value < UMBRAL_MIN_V || value > UMBRAL_MAX_V) {
    thresholdV_ = UMBRAL_DEFAULT_V;
    return false;
  }

  thresholdV_ = value;
  return true;
}

float HardwareController::thresholdV() const {
  return thresholdV_;
}

void HardwareController::setGreenLed(bool enabled) {
  currentData_.greenLedOn = enabled;
  digitalWrite(PIN_LED_VERDE, enabled ? HIGH : LOW);
}

void HardwareController::setBlueLed(bool enabled) {
  currentData_.blueLedOn = enabled;
  digitalWrite(PIN_LED_AZUL, enabled ? HIGH : LOW);
}

void HardwareController::rememberCommand(const String &command) {
  commandHistory_[2] = commandHistory_[1];
  commandHistory_[1] = commandHistory_[0];
  commandHistory_[0] = command;
}

void HardwareController::drawCommandHistory() {
  display_.setCursor(0, 32);
  display_.print(F("Ult. 3 cmds:"));
  for (int i = 0; i < 3; ++i) {
    String line = commandHistory_[i];
    if (line.length() > 21) {
      line = line.substring(0, 21);
    }
    display_.setCursor(0, static_cast<int16_t>(40 + (i * 8)));
    display_.print(line);
  }
}

bool HardwareController::showComponent(const String &component) {
  if (!oledReady_) {
    return false;
  }
  const TelemetryData data = refreshSensors();
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SH110X_WHITE);
  display_.setCursor(0, 0);

  if (component == "led") {
    display_.println(F("Estado LED"));
    display_.print(F("Led verde: "));
    display_.println(data.greenLedOn ? F("encendido") : F("apagado"));
    display_.print(F("Led azul: "));
    display_.println(data.blueLedOn ? F("encendido") : F("apagado"));
  } else if (component == "pot") {
    display_.println(F("Potenciometro"));
    display_.print(F("Voltaje: "));
    display_.print(data.potentiometerV, 2);
    display_.println(F(" V"));
    display_.print(F("Umbral: "));
    display_.print(thresholdV_, 2);
    display_.println(F(" V"));
    display_.print(F("Base auto: "));
    display_.print(automaticBaselineV_, 2);
    display_.println(F(" V"));
  } else if (component == "dht") {
    display_.println(F("Sensor DHT22"));
    display_.print(F("Temp: "));
    if (isnan(data.temperatureC)) {
      display_.println(F("sin lectura"));
    } else {
      display_.print(data.temperatureC, 1);
      display_.println(F(" C"));
    }
    display_.print(F("Hum: "));
    if (isnan(data.humidityPercent)) {
      display_.println(F("sin lectura"));
    } else {
      display_.print(data.humidityPercent, 1);
      display_.println(F(" %"));
    }
  } else {
    showUnknownDisplayCommand();
    return false;
  }

  drawCommandHistory();
  display_.display();
  return true;
}

void HardwareController::showUnknownDisplayCommand() {
  if (!oledReady_) {
    return;
  }
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SH110X_WHITE);
  display_.setCursor(0, 0);
  display_.println(F("Comando display"));
  display_.println(F("no identificado"));
  drawCommandHistory();
  display_.display();
}

void HardwareController::showMessage(const String &message) {
  if (!oledReady_) {
    return;
  }
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SH110X_WHITE);
  display_.setCursor(0, 0);
  display_.println(message);
  drawCommandHistory();
  display_.display();
}
