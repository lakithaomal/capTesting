#include <Arduino.h>

// Counts pulses on Arduino Uno pin D5 (T1) using Timer1 hardware counter.
// Connect 74HC14 OUT -> D5



// wiring 

// Arduino 5V  -------------------- SN74HC14N Pin 14 (VCC)
// Arduino GND -------------------- SN74HC14N Pin 7  (GND)

// 0.1uF decoupling cap:
// Pin 14 (VCC) ----||---- Pin 7 (GND)

// Oscillator:
// Pin 2 (1Y OUT) ---[ 10 MΩ ]--- Pin 1 (1A IN)
// Pin 1 (1A IN)  ---||--- GND   (Cunknown 1–10 pF)

// Arduino frequency input:
// Pin 2 (1Y OUT) ---------------- Arduino D5

const unsigned long GATE_MS = 200 ;  // measurement window (ms)

void setup() {
  Serial.begin(9600);

  pinMode(5, INPUT);  // D5 = T1 (Timer1 external clock input)

  // Stop Timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
}

unsigned long countPulses(unsigned long gate_ms) {
  // Reset counter
  TCNT1 = 0;

  // Timer1 clock source = external clock on T1 pin, rising edge
  // CS12..CS10 = 111
  TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);

  unsigned long t0 = millis();
  while (millis() - t0 < gate_ms) {
    // just wait while hardware counts
  }

  // Stop Timer1
  TCCR1B = 0;

  // Read 16-bit counter safely
  noInterrupts();
  unsigned int counts = TCNT1;
  interrupts();

  return counts;
}

void loop() {
  unsigned long counts = countPulses(GATE_MS);

  // counts = number of rising edges in gate time
  float freq = counts * (1000.0 / (float)GATE_MS);  // Hz

  Serial.print("counts = ");
  Serial.print(counts);
  Serial.print("  f = ");
  Serial.print(freq, 1);
  Serial.println(" Hz");

  delay(500);
}