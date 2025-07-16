/**
 * @file      boards.h
 * @author    Lewis He
 * @license   MIT
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// Entferne ESP32-spezifisches nur, wenn nicht gebraucht
#if defined(ARDUINO_ARCH_ESP32)
#include <FS.h>
#include <WiFi.h>
#endif

// SD-Card Support, falls vorhanden
#ifdef HAS_SDCARD
#include <SD.h>
#endif

// U8g2 Display
#include <U8g2lib.h>

// Nur ein Dummy-PowerManager für AVR (XPowersLib ist nur für ESP32-Module mit AXP-Chips relevant)
#ifdef HAS_PMU
#define NO_PMU_ON_AVR
#endif

// ─── OLED-Konfiguration ─────────────────────────
#ifndef DISPLAY_MODEL
#define DISPLAY_MODEL           U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#endif

#ifndef OLED_WIRE_PORT
#define OLED_WIRE_PORT          Wire
#endif

#ifndef DISPLAY_ADDR
#define DISPLAY_ADDR            0x3C  // Standardadresse
#endif

#ifndef LORA_FREQ_CONFIG
#define LORA_FREQ_CONFIG        868.0  // Europa
#endif

// ─── Online-Markierungen für Setup-Status ────────
enum {
    POWERMANAGE_ONLINE  = _BV(0),
    DISPLAY_ONLINE      = _BV(1),
    RADIO_ONLINE        = _BV(2),
    GPS_ONLINE          = _BV(3),
    PSRAM_ONLINE        = _BV(4),
    SDCARD_ONLINE       = _BV(5),
    AXDL345_ONLINE      = _BV(6),
    BME280_ONLINE       = _BV(7),
    BMP280_ONLINE       = _BV(8),
    BME680_ONLINE       = _BV(9),
    QMC6310_ONLINE      = _BV(10),
    QMI8658_ONLINE      = _BV(11),
    PCF8563_ONLINE      = _BV(12),
    OSC32768_ONLINE     = _BV(13),
};

// ─── DeviceInfo Dummy ────────────────────────────
typedef struct {
    String          chipModel = "ATmega2560";
    float           psramSize = 0.0;
    uint8_t         chipModelRev = 0;
    uint8_t         chipFreq = 16;        // MHz
    uint8_t         flashSize = 256;      // KB
    uint8_t         flashSpeed = 16;      // MHz
} DevInfo_t;

// ─── Funktionsprototypen ─────────────────────────
void setupBoards(bool disable_u8g2 = false);
bool beginSDCard();
bool beginDisplay();
void disablePeripherals();
bool beginPower();
void printResult(bool radio_online);
void flashLed();
void scanDevices(TwoWire *w);
bool beginGPS();
bool recoveryGPS();
void loopPMU(void (*pressed_cb)(void));
float getTempForNTC();
void setupBLE();  // Bleibt leer für Mega

#ifdef HAS_PMU
extern XPowersLibInterface *PMU;
extern bool pmuInterrupt;
#endif

// ─── Display-Zugriff ─────────────────────────────
extern DISPLAY_MODEL *u8g2;

// ─── Hilfsfunktionen für Display-Layout ──────────
#define U8G2_HOR_ALIGN_CENTER(t)    ((u8g2->getDisplayWidth() -  (u8g2->getUTF8Width(t))) / 2)
#define U8G2_HOR_ALIGN_RIGHT(t)     ( u8g2->getDisplayWidth()  -  u8g2->getUTF8Width(t))

// ─── UART-Konfiguration für GPS ──────────────────
// Beim Arduino Mega gibt es mehrere serielle Ports
// z.B. Serial1 (TX1=18, RX1=19), Serial2, Serial3
#define SerialGPS Serial1  // Beispielsweise GPS über Serial1

// ─── Geräte-Masken ───────────────────────────────
extern uint32_t deviceOnline;

