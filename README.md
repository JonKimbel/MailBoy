# MailBoy

Sends you a message when mail arrives in your IRL mailbox.

## Roadmap

1.  *[DONE]* Get basic code working
1.  *[DONE]* Implement power conservation functionality
1.  *[DONE]* Test on batteries, determine real-world battery life
1.  *[DONE]* Change sleep strategy to improve battery life
1.  *[DONE]* Monitor battery level & charging state, send text when charging
    finished
1.  *[DONE]* Send battery level along with button text messages
1.  *[DONE]* Check for failed send attempts, retry
1.  **[TODO]** Test on batteries again
1.  **[TODO]** Fix bug where device will sleep and immediately wake back up due
    to high level on D8
1.  **[TODO]** Send SMS on charger unplug
1.  **[TODO]** Determine how to trigger - letters are lightweight and a simple
    switch may be fooled by condensation weighing down the top plate.
1.  **[TODO]** Determine approach to guarding against moisture
1.  **[TODO]** Design 3D printable mechanism/enclosure
1.  **[TODO]** Assemble final gadget
1.  **[TODO]** Install in mailbox

## Parts

*   1x [Particle Boron](https://store.particle.io/products/boron-lte)
*   1x [large single-cell LiPo battery](https://amazon.com/gp/product/B01NAX9XYG)
*   3D printed parts *(TODO: design & link here)*

## Setup

### Toolchain

1.  Install the Particle CLI following the steps
    [here](https://docs.particle.io/tutorials/developer-tools/cli/).
1.  *COMPLETELY OPTIONAL:* Set up local build toolchain following
    [these steps](https://docs.particle.io/tutorials/developer-tools/cli/#compile-and-flash-code-locally)

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

To compile the device code, run the following from the boron/ directory:

```
particle compile boron --saveTo=out.bin
```

*NOTE: The "boron" in this command refers to the device model, the
[Particle Boron](https://store.particle.io/products/boron-lte). It is NOT
providing the name of the folder the code relies upon.*

## Flashing

To flash the compiled binary to a usb-connected device, you must first put the
device in DFU (device firmware upgrade) mode:

1.  Hold down the MODE and RESET buttons.
1.  Release the RESET button. The LED should flash purple.
1.  Once the LED flashes yellow, release the MODE button.

With the LED flashing yellow, you can run this flash command:

```
particle flash --usb out.bin
```

## Avoiding issues with the Boron

1.  Power the device sufficiently. Try running your code while connected to a
    known-good high-current USB charger, or use a Li-Po battery while debugging.
1.  Find an orientation where your Boron reliably connects to cellular and keep
    it there while working. It's frustrating to have to wait for it to connect
    to test a code change.
1.  It's not well-documented, but if the device is blinking magenta it's trying
    to update its firmware over the cellular network. To avoid a long wait, put
    the device in DFU mode (steps under [flashing section](#flashing)) and run
    `particle update`.
