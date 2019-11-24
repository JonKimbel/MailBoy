#include <Wire.h>

// Auto-connect to the Particle cloud before running any code in this file.
// If we can't connect we can't send messages, so it doesn't matter if we end up
// waiting forever for cellular data.
SYSTEM_MODE(AUTOMATIC);

void sendSms();

volatile bool buttonClicked;

void setup() {
  // Opens serial port for debugging purposes.
  // Read messages sent over serial by running 'particle serial monitor'.
  Serial.begin(9600);

  pinMode(D5, INPUT_PULLUP);
  attachInterrupt(D5, sendSms, FALLING);
  Serial.println("Setup complete. MailBoy ready for input.");
}

void loop() {
  if (buttonClicked) {
    buttonClicked = false;
    Serial.println("Sending SMS.");
    Particle.publish("twilio_sms", "You have mail!", PRIVATE);
  }
}

void sendSms() {
  buttonClicked = true;
}
