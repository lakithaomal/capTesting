#include <Arduino.h>

const int FREQ_PIN = 2;              // connect 74HC14 OUT here (interrupt pin)
volatile unsigned long edges = 0;

void isr() { edges++; }

void setup() {
  Serial.begin(115200);
  pinMode(FREQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FREQ_PIN), isr, RISING);
}

void loop() {
  edges = 0;
  unsigned long t0 = millis();
  delay(200);                        // gate time
  unsigned long dt_ms = millis() - t0;

  noInterrupts();
  unsigned long e = edges;
  interrupts();

  float freq = (e * 1000.0) / dt_ms; // Hz
  Serial.print("f = ");
  Serial.print(freq, 1);
  Serial.println(" Hz");

  delay(500);
}