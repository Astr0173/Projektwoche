#include "LoRaBoards.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>

// Display-Objekt Zeiger
DISPLAY_MODEL *u8g2 = NULL;

// Dummy-DevInfo (kann nach Bedarf ergänzt werden)
static DevInfo_t devInfo;

uint32_t deviceOnline = 0x00;

// --- Power Management STUBS ---
bool beginPower() {
    // Für AVR/Mega gibt es kein XPowersLib, immer OK zurückgeben
    deviceOnline |= POWERMANAGE_ONLINE;
    return true;
}
void disablePeripherals() {
    // Keine Sonderaktionen für AVR
}
void loopPMU(void (*pressed_cb)(void)) {
    // Keinerlei Power-Management auf AVR
}

// --- SD-Karten Initialisierung ---
bool beginSDCard() {
#ifdef HAS_SDCARD
    // SS ist beim Arduino Mega Pin 53, kann aber je nach Shield abweichen.
    if (!SD.begin(53)) {
        Serial.println(F("SD Karte konnte nicht initialisiert werden!"));
        return false;
    }
    deviceOnline |= SDCARD_ONLINE;
    Serial.println(F("SD Karte erkannt."));
    return true;
#else
    return false;
#endif
}

// --- Display Initialisierung ---
bool beginDisplay() {
    Wire.beginTransmission(DISPLAY_ADDR);
    if (Wire.endTransmission() == 0) {
        Serial.print(F("Display gefunden bei 0x"));
        Serial.println(DISPLAY_ADDR, HEX);
        u8g2 = new DISPLAY_MODEL(U8G2_R0, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_ncenB08_tr);
        u8g2->drawStr(0,24,"LoRa/Mega2560");
        u8g2->sendBuffer();
        deviceOnline |= DISPLAY_ONLINE;
        return true;
    }
    Serial.println(F("Display nicht gefunden!"));
    return false;
}

// --- LoRa-Board Setup ---
void setupBoards(bool disable_u8g2) {
    // Beispiel: Serielle Schnittstelle, I2C
    Serial.begin(115200);
    Wire.begin();
    SPI.begin();

    if (!disable_u8g2)
        beginDisplay();

    beginPower();
    // Kein BLE und kein XPowers/PMU auf Mega
}

// --- Einfache Hilfsfunktionen ---
void printResult(bool radio_online) {
    if (radio_online) {
        Serial.println(F("LoRa online."));
        deviceOnline |= RADIO_ONLINE;
    } else {
        Serial.println(F("LoRa offline!"));
    }
}

void flashLed() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

// --- I2C Bus Scan ---
void scanDevices(TwoWire *w) {
    byte error, address;
    int nDevices = 0;
    Serial.println(F("Scanne I2C Bus..."));
    for(address = 1; address < 127; address++ ) {
        w->beginTransmission(address);
        error = w->endTransmission();
        if (error == 0) {
            Serial.print(F("I2C Gerät gefunden bei 0x"));
            Serial.println(address, HEX);
            nDevices++;
        }
    }
    if (nDevices == 0) Serial.println(F("Keine I2C Geräte gefunden"));
}

// --- Dummy-Implementierungen für BLE und GPS ---
void setupBLE() {
    // Nicht unterstützt auf AVR/Mega
}
bool beginGPS() {
    // Dummy: Nie ein GPS angeschlossen
    return false;
}
bool recoveryGPS() {
    // Dummy
    return false;
}
float getTempForNTC() {
    return NAN;
}
