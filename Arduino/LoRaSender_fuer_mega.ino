#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <SensirionI2cScd30.h>
#include <LoRa.h>
#include "LoRaBoards.h"
#include "utilities.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define BME680_I2C_ADDR_PRIMARY 0x76
#define BME680_I2C_ADDR_SECONDARY 0x77
#define SCD30_I2C_ADDR 0x61

// LoRa Pin-Definitionen für Arduino Mega (alternativ zu LoRaBoards.h)
#define RADIO_CS_PIN    53   // SPI CS
#define RADIO_RST_PIN   9    // beliebiger GPIO für Reset
#define RADIO_DIO0_PIN  2    // Interrupt-Pin

Adafruit_BME680 bme;
SensirionI2cScd30 scd30;
bool bmeFound = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // --- I2C Initialisierung für Mega ohne Pinparameter ---
  Wire.begin();

  // --- BME680 Initialisierung ---
  Serial.println(F("Starte BME680..."));
  if (!bme.begin(BME680_I2C_ADDR_PRIMARY)) {
    Serial.println("BME680 unter 0x76 nicht gefunden, versuche 0x77...");
    if (!bme.begin(BME680_I2C_ADDR_SECONDARY)) {
      Serial.println("BME680 nicht gefunden. Verkabelung oder Adresse pruefen!");
      bmeFound = false;
    } else {
      bmeFound = true;
    }
  } else {
    bmeFound = true;
  }

  if (bmeFound) {
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
    Serial.println("BME680 gefunden und initialisiert!");
  }

  // --- SCD30 Initialisierung ---
  Serial.println(F("Starte SCD30..."));
  scd30.begin(Wire, SCD30_I2C_ADDR);
  uint16_t error = scd30.startPeriodicMeasurement(0);
  if (error) {
    Serial.print("Fehler bei startPeriodicMeasurement: ");
    Serial.println(error);
  } else {
    Serial.println("SCD30 Messung gestartet");
  }

  // --- LoRa Initialisierung ---
  setupBoards();
  delay(1500);

#ifdef  RADIO_TCXO_ENABLE
  pinMode(RADIO_TCXO_ENABLE, OUTPUT);
  digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

#ifdef RADIO_CTRL
  digitalWrite(RADIO_CTRL, LOW);
#endif

  Serial.println("LoRa Sender");
  LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN); // Pins in LoRaBoards.h prüfen!
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(17);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(10);
  LoRa.setPreambleLength(16);
  LoRa.setSyncWord(0xAB);
  LoRa.disableCrc();
  LoRa.disableInvertIQ();
  LoRa.setCodingRate4(7);

  Serial.println("Setup abgeschlossen!\n");
}

void loop() {
  float bme_temp = NAN, bme_hum = NAN, bme_pres = NAN, bme_gas = NAN, bme_alt = NAN;
  float scd30_co2 = NAN, scd30_temp = NAN, scd30_hum = NAN;

  if (bmeFound && bme.performReading()) {
    bme_temp = b
