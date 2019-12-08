// API reference:
// https://docs.particle.io/reference/device-os/firmware/boron

#include <Wire.h>

// Function declaration.
void sleepUntilButtonPress();
void retrySend();
bool sendSms();

// Constants.
const int BUTTON_PIN = D8; // This is the only pin that can wake Boron from deep
                           // sleep.
const FuelGauge fuel;

// Variables.
// TODO: add in logic for capping number of retries.
bool retrying = false;

// Auto-connect to the Particle cloud before running any code in this file.
// If we can't connect we can't send messages, so it doesn't matter if we end up
// waiting forever for cellular data.
SYSTEM_MODE(AUTOMATIC);

void setup() {
  // Opens serial port for debugging purposes.
  // Read messages sent over serial by running 'particle serial monitor'.
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  // If the button is already pressed, it's possible we reset from power loss
  // and never sent an SMS. Send one now just in case.
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("MailBoy woke up with button already depressed.");
    if (!sendSms()) {
      retrying = true;
    }
  }
}

void loop() {
  if (retrying) {
    retrySend();
  } else {
    sleepUntilButtonPress();
  }
}

void sleepUntilButtonPress() {
  Serial.println("MailBoy sleeping until next button press...");
  // Put the device into deep sleep until awakened by a high level on pin D8.
  //
  // When the device is woken back up, all RAM (including the program counter)
  // will be reset, so we'll start back in the setup() function.
  System.sleep(SLEEP_MODE_SOFTPOWEROFF);
}

void retrySend() {
  Serial.println("MailBoy sleeping for a minute and then retrying send...");
  // SLEEP_NETWORK_STANDBY saves more power for short sleeps and avoids our SIM
  // being blocked by the network for connecting & disconnecting too rapidly.
  // See docs:
  // https://docs.particle.io/reference/device-os/firmware/boron/#sleep-sleep-
  //
  // TODO: figure out a better combination of arguments to ensure we don't wake
  // up for pin interactions.
  System.sleep(
      /* wakeUpPin = */ BUTTON_PIN, /* edgeTriggerMode = */ RISING,
      /* sleep mode = */ SLEEP_NETWORK_STANDBY,
      /* seconds (0 = no timeout) = */ 60);

  Serial.println("MailBoy retrying send...");
  if(sendSms()) {
    retrying = false;
  }
}

bool sendSms() {
  Serial.println("Attempting to send SMS.");

  char message[29];
  char battery_percentage[5];
  dtostrf(
    	/* value = */ fuel.getSoC(),
    	/* min width of output = */ 1,
    	/* places after decimal = */ 0,
    	/* output = */ battery_percentage);
  sprintf(message, "You have mail! Battery: %s", battery_percentage);

  bool success = Particle.publish("twilio_sms", message, PRIVATE);
  if (!success) {
    Serial.println("Failed!");
  }
  return success;
}
