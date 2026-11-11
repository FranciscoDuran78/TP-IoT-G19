#include <HardwareController.h>

#include <math.h>

HardwareController::HardwareController()
    : dht_(PIN_DHT22, DHT22),
      display_(128, 64, &Wire, -1),
      thresholdV_(UMBRAL_DEFAULT_V),
      automaticBaselineV_(0.0f),
      automaticMode_(false) {
  currentData_.temperatureC = NAN;
  currentData_.humidityPercent = NAN;
  currentData_.potentiometerV = 0.0f;
  currentData_.greenLedOn = false;
  currentData_.blueLedOn = false;
}

void HardwareController::begin() {
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);
  pinMode(PIN_POTENCIOMETRO, INPUT);
  analogReadResolution(12);

  setGreenLed(false);
  setBlueLed(false);
  dht_.begin();

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!display_.begin(SSD1306_SWITCHCAPVCC, OLED_DIRECCION_I2C)) {
    Serial.println("No se pudo inicializar la pantalla OLED");
    return;
  }

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println("TPI IoT listo");
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

  // Interpretacion inicial: el umbral es una diferencia sobre la linea base.
  // Confirmar esta interpretacion con el equipo/docente antes de cerrar el TPI.
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
  display_.print("Ultimos comandos:");
  display_.setCursor(0, 40);
  display_.print(commandHistory_[0]);
  display_.setCursor(0, 48);
  display_.print(commandHistory_[1]);
  display_.setCursor(0, 56);
  display_.print(commandHistory_[2]);
}

bool HardwareController::showComponent(const String &component) {
  const TelemetryData data = refreshSensors();
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);

  if (component == "led") {
    display_.println("Estado LEDs");
    display_.print("Verde: ");
    display_.println(data.greenLedOn ? "encendido" : "apagado");
    display_.print("Azul: ");
    display_.println(data.blueLedOn ? "encendido" : "apagado");
  } else if (component == "pot") {
    display_.println("Potenciometro");
    display_.print("Voltaje: ");
    display_.print(data.potentiometerV, 2);
    display_.println(" V");
    display_.print("Umbral: ");
    display_.print(thresholdV_, 2);
    display_.println(" V");
  } else if (component == "dht") {
    display_.println("Sensor DHT22");
    display_.print("Temp: ");
    if (isnan(data.temperatureC)) display_.println("sin lectura");
    else { display_.print(data.temperatureC, 1); display_.println(" C"); }
    display_.print("Hum: ");
    if (isnan(data.humidityPercent)) display_.println("sin lectura");
    else { display_.print(data.humidityPercent, 1); display_.println(" %"); }
  } else {
    showUnknownDisplayCommand();
    return false;
  }

  drawCommandHistory();
  display_.display();
  return true;
}

void HardwareController::showUnknownDisplayCommand() {
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println("Comando display");
  display_.println("no identificado");
  drawCommandHistory();
  display_.display();
}

void HardwareController::showMessage(const String &message) {
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setTextColor(SSD1306_WHITE);
  display_.setCursor(0, 0);
  display_.println(message);
  drawCommandHistory();
  display_.display();
}
