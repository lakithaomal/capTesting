#include <Arduino.h>

#include <Wire.h>

/*
============================================================
WIRING DIAGRAM: Arduino Nano 33 IoT ↔ FDC1004
============================================================

POWER CONNECTIONS:
------------------
Nano 33 IoT 3.3V  -------->  FDC1004 VDD
Nano 33 IoT GND   -------->  FDC1004 GND

IMPORTANT:
- Do NOT connect 5V to FDC1004 (3.3V device only)
- Place a 0.1µF decoupling capacitor between VDD and GND
  as close to the FDC1004 chip as possible.

I2C CONNECTIONS:
----------------
Nano 33 IoT SDA  -------->  FDC1004 SDA
Nano 33 IoT SCL  -------->  FDC1004 SCL

(No level shifter required – Nano 33 IoT is 3.3V logic.)

SENSOR CONNECTION (Single-Ended Measurement):
----------------------------------------------
FDC1004 CIN1  ----||----  GND
                  Csensor

- One side of the capacitor/sensor goes to CIN1
- Other side goes to GND
- Keep the CIN1 trace very short
- Avoid breadboard for pF-level measurements

Default I2C Address: 0x50
============================================================
*/

#define FDC_ADDR 0x50

// Registers
#define MEAS1_MSB   0x00
#define MEAS1_LSB   0x01
#define MEAS_CONF1  0x08
#define FDC_CONF    0x0C

void write16(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(FDC_ADDR);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);   // MSB
  Wire.write(value & 0xFF);          // LSB
  Wire.endTransmission();
}

uint16_t read16(uint8_t reg) {
  Wire.beginTransmission(FDC_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(FDC_ADDR, 2);

  uint16_t value = (Wire.read() << 8);
  value |= Wire.read();
  return value;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  delay(100);

  // Configure Measurement 1: CIN1 vs GND
  write16(MEAS_CONF1, 0x1080);

  // Start continuous conversion (100 samples/sec)
  write16(FDC_CONF, 0x0440);

  Serial.println("FDC1004 Started...");
}

void loop() {

  uint16_t msb = read16(MEAS1_MSB);
  uint16_t lsb = read16(MEAS1_LSB);

  // Combine into 24-bit signed
  int32_t raw = ((int32_t)msb << 8) | (lsb >> 8);

  // Sign extend 24-bit
  if (raw & 0x800000) {
    raw -= 1 << 24;
  }

  // Convert to pF (0.5 fF per LSB)
  float capacitance_pf = raw * 0.0005;

  Serial.print("Capacitance: ");
  Serial.print(capacitance_pf, 6);
  Serial.println(" pF");

  delay(500);
}