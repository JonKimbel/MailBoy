#include <Wire.h>

// Auto-connect to the Particle cloud before running any code in this file.
// If we can't connect we can't send messages, so it doesn't matter if we end up
// waiting forever for cellular data.
SYSTEM_MODE(AUTOMATIC);

void setup() {
  // Opens serial port for debugging purposes.
  // Read messages sent over serial by running 'particle serial monitor'.
  Serial.begin(9600);

  Serial.println("MailBoy started.");
}

// void loop() { }
