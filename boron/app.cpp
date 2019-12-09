// API reference:
// https://docs.particle.io/reference/device-os/firmware/boron

#include <Wire.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function declaration.

void sleepUntilButtonPress();
void monitorChargeLevel();
void retrySend();
void sleepForSeconds(int seconds);
bool sendSms();

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constants.

// D8 is the only pin that can wake Boron from deep sleep.
// It should be attached to a button that connects to 3V3 when depressed.
const int BUTTON_PIN = D8;

// This pin should be connected to the middle of a voltage divider running from
// VUSB to GND. A voltage divider is required because the GPIO pins can't take
// 5V, so we need to knock it down to 3V3 logic levels to read it.
//
// To account for the Boron's minimum recognized high voltage (0.7*3.3V) and the
// possibility that the USB supply outputs more than 5V, the divider needs to
// supply somewhere between 47% to 60% of the VUSB voltage to this pin.
//
// For simplicity's sake, I recommend using 2x 100kΩ resistors. The high
// resistance means our detector will sip a tiny amount of power from the USB
// supply and won't run into issues with the 100MΩ input impedance of GPIO pins.
const int CHARGE_DETECT_PIN = D5;

const int MAX_RETRIES = 5;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Variables.

FuelGauge fuel;

bool chargingSmsSent = false;

bool retrying = false;
int retryCount = 0;

// Auto-connect to the Particle cloud before running any code in this file.
// If we can't connect we can't send messages, so it doesn't matter if we end up
// waiting forever for cellular data.
SYSTEM_MODE(AUTOMATIC);

void setup() {
  // Opens serial port for debugging purposes.
  // Read messages sent over serial by running 'particle serial monitor'.
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(CHARGE_DETECT_PIN, INPUT);

  if (digitalRead(CHARGE_DETECT_PIN) == HIGH) {
    Serial.println("MailBoy woke up connected to USB power.");
  } else if (digitalRead(BUTTON_PIN) == HIGH) {
    // If the button is already pressed, we likely just woke from sleep due to
    // it being pressed. Send an SMS now.
    Serial.println("MailBoy woke up with button already depressed.");
    if (!sendSms()) {
      retrying = true;
    }
  }
}

void loop() {
  if (retrying) {
    retrySend();
  } else if (digitalRead(CHARGE_DETECT_PIN) == HIGH) {
    monitorChargeLevel();
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

void monitorChargeLevel() {
  Serial.println(
      "MailBoy sleeping for 10s and then checking charge level...");
  sleepForSeconds(10);

  if (fuel.getSoC() > 80 && !chargingSmsSent) {
    chargingSmsSent = true;
    // TODO: send a different text message when charging is complete.
    if(!sendSms()) {
      retrying = true;
    }
  }
}

void retrySend() {
  Serial.println("MailBoy sleeping for a minute and then retrying send...");
  sleepForSeconds(60);

  Serial.println("MailBoy retrying send...");
  if(sendSms()) {
    retrying = false;
    retryCount = 0;
  } else {
    retryCount++;
  }

  if (retryCount == MAX_RETRIES) {
    retrying = false;
    retryCount = 0;
  }
}

void sleepForSeconds(int seconds) {
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
      /* seconds (0 = no timeout) = */ seconds);
}

bool sendSms() {
  Serial.println("Attempting to send SMS.");

  char message[30];
  sprintf(message, "You have mail! Battery: %.0f%%", fuel.getSoC());

  bool success = Particle.publish("twilio_sms", message, PRIVATE);
  if (!success) {
    Serial.println("Failed!");
  }
  return success;
}
