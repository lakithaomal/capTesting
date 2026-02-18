import smbus
import time

"""
============================================================
Raspberry Pi Zero  ↔  FDC1004 Wiring
============================================================

POWER:
------
Pi Pin 1  (3.3V)  -------->  FDC1004 VDD
Pi Pin 6  (GND)   -------->  FDC1004 GND

I2C:
----
Pi Pin 3  (GPIO2 / SDA)  ---->  FDC1004 SDA
Pi Pin 5  (GPIO3 / SCL)  ---->  FDC1004 SCL

NOTE:
- Do NOT connect 5V to FDC1004 (it is 3.3V only)
- No level shifter required (Pi is 3.3V logic)
- Add 0.1 µF capacitor between VDD and GND near chip

SENSOR CONNECTION:
------------------
CIN1  --------||--------  GND
              Csensor

- Keep trace from CIN1 short
- Avoid breadboard for pF-level sensing
- Do not touch sensor while measuring

Default I2C Address: 0x50
============================================================
"""



# https://www.tindie.com/products/protocentral/fdc1004-capacitance-sensor-breakout-board/

FDC_ADDR = 0x50
bus = smbus.SMBus(1)

# Register addresses
MEAS1_MSB = 0x00
MEAS1_LSB = 0x01
MEAS_CONF1 = 0x08
FDC_CONF = 0x0C

def write_register(reg, value):
    bus.write_word_data(FDC_ADDR, reg, ((value & 0xFF) << 8) | (value >> 8))

def read_register(reg):
    val = bus.read_word_data(FDC_ADDR, reg)
    return ((val & 0xFF) << 8) | (val >> 8)

# 1️⃣ Configure measurement 1 to use CIN1
# CHA = CIN1 (0), CHB = GND (100), CAPDAC disabled
write_register(MEAS_CONF1, 0x1080)

# 2️⃣ Start measurement, 100 samples/sec
write_register(FDC_CONF, 0x0440)

time.sleep(0.1)

while True:
    msb = read_register(MEAS1_MSB)
    lsb = read_register(MEAS1_LSB)

    raw = (msb << 8) | (lsb >> 8)

    # Convert to signed 24-bit
    if raw & 0x800000:
        raw -= 1 << 24

    capacitance_pf = raw * 0.0005

    print("Capacitance: {:.4f} pF".format(capacitance_pf))

    time.sleep(0.5)