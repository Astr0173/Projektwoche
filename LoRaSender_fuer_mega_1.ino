#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <SensirionI2cScd30.h>
#include <LoRa.h>

// ------ Pin-Definition für Dragino LoRa Shield ------
#define LORA_CS    10
#define LORA_RST   7
#define LORA_DIO0  2

// ------ Sensoradressen ------
#define BME680_I2C_ADDR  0x76
#define SCD30_ADDR       0x61
#define SEALEVELPRESSURE_HPA 1013.25

Adafruit_BME680 bme;
SensirionI2cScd30 scd30;
bool bmeFound = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();     // Mega: SDA=20, SCL=21

  // ----- BME680 Initialisieren -----
  Serial.println("Starte BME680...");
  if (!bme.begin(BME680_I2C_ADDR)) {
    Serial.println("BME680 nicht gefunden.");
    bmeFound = false;
  } else {
    bmeFound = true;
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
    Serial.println("BME680 bereit.");
  }

  // ----- SCD30 Initialisieren -----
  Serial.println("Starte SCD30...");
  scd30.begin(Wire, SCD30_ADDR);
  uint16_t err = scd30.startPeriodicMeasurement(0);
  if (err) {
    Serial.print("Fehler bei SCD30: ");
    Serial.println(err);
  } else {
    Serial.println("SCD30 bereit.");
  }

  // ----- LoRa (Dragino Shield) Initialisieren -----
  Serial.println("Initialisiere LoRa...");
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("[FATAL] LoRa Modul nicht gefunden!");
    while(true);
  }

  LoRa.setTxPower(17);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(10);
  LoRa.setPreambleLength(16);
  LoRa.setSyncWord(0xAB);
  LoRa.disableCrc();
  LoRa.disableInvertIQ();
  LoRa.setCodingRate4(7);

  Serial.println("LoRa bereit.\n");
}

void loop() {
  float bme_temp = NAN, bme_hum = NAN, bme_pres = NAN, bme_gas = NAN, bme_alt = NAN;
  float scd30_co2 = NAN, scd30_temp = NAN, scd30_hum = NAN;

  // ----- BME680 Lesen -----
  if (bmeFound && bme.performReading()) {
    bme_temp = bme.temperature;
    bme_hum  = bme.humidity;
    bme_pres = bme.pressure / 100.0;
    bme_gas  = bme.gas_resistance / 1000.0;
    bme_alt  = bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  // ----- SCD30 Lesen -----
  uint16_t ready = 0;
  if (!scd30.getDataReady(ready) && ready) {
    if (scd30.readMeasurementData(scd30_co2, scd30_temp, scd30_hum)) {
      scd30_co2 = scd30_temp = scd30_hum = NAN;
    }
  }

  // ----- Datenpaket zusammenbauen -----
  String payload = "";
  payload += "BME680:";
  payload += String(bme_temp, 2) + "," + String(bme_hum, 2) + "," +
             String(bme_pres, 2) + "," + String(bme_gas, 2) + "," + String(bme_alt, 2);
  payload += ";SCD30:";
  payload += String(scd30_co2, 2) + "," + String(scd30_temp, 2) + "," + String(scd30_hum, 2);

  // ----- Seriell ausgeben -----
  Serial.println("[SEND]");
  Serial.println(payload);
  Serial.println("---------------------------");

  // ----- Per LoRa senden -----
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();

  delay(5000); // 5 Sekunden Intervall
}
