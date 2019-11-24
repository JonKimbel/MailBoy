# MailBoy

Sends you a message when mail arrives in your IRL mailbox.

## Setup

### Webhooks

1.  Sign up for a [Twilio](https://www.twilio.com/) account. Twilio is the
    service that will send the text message to your phone number.
1.  Set up a Particle account, claim your Boron, and sign up for Particle device
    cloud if you haven't yet.
1.  Follow [these instructions](https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-particle-photon#set-up-a-particle-webhook)
    to connect Twilio to your Particle device cloud. Note that these
    instructions show "{{}}" characters in the POST URL that you need to enter,
    those characters should not be there.
1.  Click "TEST" on your new webhook integration, and confirm that you get a
    text message from Twilio.

### Git configuration (only required for contributors)

Run the following from this directory to set up the pre-commit checks.

```
git config core.hooksPath hooks
```

## Compiling

### Before you begin

1.  Install the Particle CLI following the steps
    [here](https://docs.particle.io/tutorials/developer-tools/cli/).
1.  COMPLETELY OPTIONAL: Set up local build toolchain following
    [these steps](https://docs.particle.io/tutorials/developer-tools/cli/#compile-and-flash-code-locally)

### Compiling & flashing

To compile the device code, run the following from the boron/ directory:

```
particle compile boron --saveTo=out.bin
```

NOTE: The "boron" in this command refers to the device model, the
[Particle Boron](https://store.particle.io/products/boron-lte). It is NOT
providing the name of the folder the code relies upon.

To flash the compiled binary to a usb-connected device:

1.  Hold down the MODE and RESET buttons.
1.  Release the RESET button. The LED should flash purple.
1.  Once the LED flashes yellow, release the MODE button.
1.  Run this flash command:

        particle flash --usb out.bin
