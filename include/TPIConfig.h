#pragma once

#include <Arduino.h>

// Pines definidos en el enunciado.
constexpr uint8_t PIN_DHT22 = 33;
constexpr uint8_t PIN_POTENCIOMETRO = 32;
constexpr uint8_t PIN_LED_VERDE = 23;
constexpr uint8_t PIN_LED_AZUL = 2;
constexpr uint8_t PIN_OLED_SDA = 21;
constexpr uint8_t PIN_OLED_SCL = 22;

constexpr uint8_t OLED_DIRECCION_I2C = 0x3C;
constexpr uint16_t ADC_MAXIMO = 4095;
constexpr float ADC_VOLTAGE_MAX = 3.3f;

// El enunciado indica 0.5 V por defecto y un rango permitido de 1.0 a 1.5 V.
constexpr float UMBRAL_DEFAULT_V = 0.5f;
constexpr float UMBRAL_MIN_V = 1.0f;
constexpr float UMBRAL_MAX_V = 1.5f;

constexpr unsigned long INTERVALO_TELEGRAM_MS = 1000UL;
constexpr unsigned long INTERVALO_THINGSPEAK_MS = 20000UL;
constexpr unsigned long INTERVALO_WIFI_REINTENTO_MS = 10000UL;
