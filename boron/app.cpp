// API reference for the Boron:
// https://docs.particle.io/reference/device-os/firmware/boron

#include <Wire.h>

// Function declaration.
bool sendSms();

// Constants.
const int BUTTON_PIN = D5;

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

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Setup complete. MailBoy ready for input.");

  // If the button is already pressed, it's possible we reset from power loss
  // and never sent an SMS. Send one now just in case.
  if (digitalRead(BUTTON_PIN) == LOW) {
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
    sleepUntilButtonPressOrTimeout();
  }
}

void sleepUntilButtonPressOrTimeout() {
  // Sleep until the button is pressed. Using SLEEP_NETWORK_STANDBY for now to
  // avoid our SIM being blocked by the network while testing, see docs:
  // https://docs.particle.io/reference/device-os/firmware/boron/#sleep-sleep-
  Serial.println("MailBoy sleeping until next button press...");
  SleepResult sleep_result = System.sleep(
      /* wakeUpPin = */ BUTTON_PIN, /* edgeTriggerMode = */ FALLING,
      /* sleep mode = */ SLEEP_NETWORK_STANDBY,
      /* seconds (0 = no timeout) = */ 0);

  if (!sleep_result.wokenUpByPin()) {
    // Only send SMS if we're woken up by the button being pressed. We don't set
    // a timeout, but docs say the Boron won't sleep for more than 24 days in a
    // row.
    return;
  }

  Serial.println("MailBoy detected button press, attempting send...");
  if (!sendSms()) {
    retrying = true;
  }
}

void retrySend() {
  Serial.println("MailBoy sleeping for a minute and then retrying send...");
  // TODO: figure out a better combination of arguments to ensure we don't wake
  // up for pin interactions.
  System.sleep(
      /* wakeUpPin = */ BUTTON_PIN, /* edgeTriggerMode = */ FALLING,
      /* sleep mode = */ SLEEP_NETWORK_STANDBY,
      /* seconds (0 = no timeout) = */ 60);

  Serial.println("MailBoy retrying send...");
  if(sendSms()) {
    retrying = false;
  }
}

bool sendSms() {
  Serial.println("Attempting to send SMS.");
  bool success = Particle.publish("twilio_sms", "You have mail!", PRIVATE);
  if (!success) {
    Serial.println("Failed!");
  }
  return success;
}
