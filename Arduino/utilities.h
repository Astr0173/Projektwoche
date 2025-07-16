/**
 * @file      utilities.h
 * @author    Lewis He
 * @license   MIT
 * @modified  2025 by Community, angepasst für Arduino Mega 2560
 */
#define RADIO_CS_PIN    53
#define RADIO_RST_PIN   9
#define RADIO_DIO0_PIN  2

#pragma once

// Eigene Board-Definition setzen
#define ARDUINO_MEGA_LORA_SX1276

// Ungenutzte Pins
#define UNUSED_PIN                   (255)

#if defined(ARDUINO_MEGA_LORA_SX1276)

// ───── Board-Name ────────────────────────────────
#define BOARD_VARIANT_NAME          "Arduino Mega 2560 LoRa"

// ───── LoRa-Modul: SX1276/SX1278 ────────────────
#ifndef USING_SX1276
#define USING_SX1276
#endif

// LoRa SPI-Pins (Mega2560 Hardware SPI: MOSI=51, MISO=50, SCK=52)
#define RADIO_CS_PIN                53    // SPI SS
#define RADIO_RST_PIN               9     // beliebiger GPIO
#define RADIO_DIO0_PIN              2     // digitaler Interrupt-Pin

// Optional:
#define RADIO_DIO1_PIN              UNUSED_PIN
#define RADIO_DIO2_PIN              UNUSED_PIN
#define RADIO_BUSY_PIN              UNUSED_PIN

// ───── I2C (Wire: SDA=20, SCL=21) ────────────────
#define I2C_SDA                     20
#define I2C_SCL                     21

// ───── OLED-Konfiguration (I2C SSD1306) ──────────
#define HAS_DISPLAY
#define DISPLAY_MODEL               U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#define DISPLAY_ADDR                0x3C
#define OLED_RST                    UNUSED_PIN

// ───── SD-Karte (optional: SPI) ──────────────────
#define HAS_SDCARD
#define SDCARD_CS                   4     // Beispiel: Digital 4
#define SDCARD_MOSI                 51
#define SDCARD_MISO                 50
#define SDCARD_SCLK                 52

// ───── LED & Button ──────────────────────────────
#define BOARD_LED                   13
#define LED_ON                      HIGH
#define LED_OFF                     LOW

#define BUTTON_PIN                  UNUSED_PIN

// ───── GPS-Konfiguration (optional via Serial1) ──
#define HAS_GPS
#define GPS_RX_PIN                  19    // RX1
#define GPS_TX_PIN                  18    // TX1
#define GPS_BAUD_RATE               9600

// ───── Batteriemessung (nicht verfügbar) ─────────
#define ADC_PIN                     UNUSED_PIN
#define BAT_ADC_PULLUP_RES          (100000.0)
#define BAT_ADC_PULLDOWN_RES        (100000.0)
#define BAT_MAX_VOLTAGE             (4.2)
#define BAT_VOL_COMPENSATION        (0.0)

#else
#error "Kein gültiges Board definiert! Bitte define ARDUINO_MEGA_LORA_SX1276 in utilities.h"
#endif

// ───── Automatische RadioTyp-Auswahl-String ─────
#if  defined(USING_SX1262)
#define RADIO_TYPE_STR  "SX1262"
#elif defined(USING_SX1276)
#define RADIO_TYPE_STR  "SX1276"
#elif defined(USING_SX1278)
#define RADIO_TYPE_STR  "SX1278"
#elif defined(USING_LR1121)
#define RADIO_TYPE_STR  "LR1121"
#elif defined(USING_SX1280)
#define RADIO_TYPE_STR  "SX1280"
#elif defined(USING_SX1280PA)
#define RADIO_TYPE_STR  "SX1280PA"
#else
#define RADIO_TYPE_STR  "Unknown"
#endif
