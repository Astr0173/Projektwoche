#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <SensirionI2cScd30.h>

// Adressen
#define BME680_ADDRESS 0x76    // oder 0x77 je nach Beschriftung / Modul
#define SCD30_ADDRESS  0x61
#define SEALEVELPRESSURE_HPA 1013.25

// Objekte
Adafruit_BME680 bme;
SensirionI2cScd30 scd30;

bool bme_ok = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starte Sensorsystem...");

  Wire.begin();  // SDA = 20, SCL = 21 auf Mega

  // --- BME680 Initialisierung ---
  Serial.println("Initialisiere BME680...");
  if (!bme.begin(BME680_ADDRESS)) {
    Serial.println("BME680 nicht gefunden!");
  } else {
    bme_ok = true;
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);  // Temperatur in °C, Dauer in ms
    Serial.println("BME680 bereit.");
  }

  // --- SCD30 Initialisierung ---
  Serial.println("Initialisiere SCD30...");
  scd30.begin(Wire, SCD30_ADDRESS);
  uint16_t err = scd30.startPeriodicMeasurement(0);  // Kein Umgebungsdruck
  if (err) {
    Serial.print("SCD30 Fehler: ");
    Serial.println(err);
  } else {
    Serial.println("SCD30 bereit.");
  }
  Serial.println("------------------------------");
}

void loop() {
  float bme_temp = NAN, bme_hum = NAN, bme_pres = NAN, bme_gas = NAN, bme_alt = NAN;
  float co2 = NAN, scd_temp = NAN, scd_hum = NAN;

  // --- BME680 Auslesen ---
  if (bme_ok && bme.performReading()) {
    bme_temp = bme.temperature;
    bme_hum  = bme.humidity;
    bme_pres = bme.pressure / 100.0;      // Pa → hPa
    bme_gas  = bme.gas_resistance / 1000.0; // Ohm → kOhm
    bme_alt  = bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  // --- SCD30 Auslesen ---
  uint16_t ready = 0;
  if (!scd30.getDataReady(ready) && ready) {
    if (scd30.readMeasurementData(co2, scd_temp, scd_hum)) {
      co2 = scd_temp = scd_hum = NAN;
    }
  }

  // --- Serieller Monitor Ausgabe ---
  Serial.println("BME680:");
  Serial.print("  Temperatur: "); Serial.print(bme_temp); Serial.println(" °C");
  Serial.print("  Luftfeuchtigkeit: "); Serial.print(bme_hum); Serial.println(" %");
  Serial.print("  Luftdruck: "); Serial.print(bme_pres); Serial.println(" hPa");
  Serial.print("  Gaswiderstand: "); Serial.print(bme_gas); Serial.println(" kOhm");
  Serial.print("  Höhe: "); Serial.print(bme_alt); Serial.println(" m");

  Serial.println("SCD30:");
  Serial.print("  CO₂: "); Serial.print(co2); Serial.println(" ppm");
  Serial.print("  Temp.: "); Serial.print(scd_temp); Serial.println(" °C");
  Serial.print("  Luftfeuchtigkeit: "); Serial.print(scd_hum); Serial.println(" %");

  Serial.println("------------------------------\n");

  delay(5000);  // alle 5 Sekunden neue Werte
}
