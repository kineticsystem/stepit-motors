# The Hardware

The datasheets and pictures of the hardware StepIt Motors drives: a Teensy 4.0 or 4.1, the PiBot board and its TMC2208 drivers. The parts to buy, and why each TMC2208 sits on a SilentStepStick Protector, are in [Prerequisites](../../README.md#prerequisites) of the README.

## Files

| File | What it is |
|---|---|
| [`PiBotMDRB.pdf`](PiBotMDRB.pdf) | The manual of the PiBot MDRB, the board that carries five stepper drivers: the jumpers of the microstepping and of the inverted logic. |
| [`PiBotMDRBSize.pdf`](PiBotMDRBSize.pdf) | A drawing of the PiBot MDRB, for its size. |
| [`TMC2208.jpg`](TMC2208.jpg) | A photo of the Watterott SilentStepStick TMC2208, the driver of each motor. |
| [`Protector.jpg`](Protector.jpg) | A photo of the Watterott SilentStepStick Protector, plugged under each TMC2208. |
| [`Teensy41.jpg`](Teensy41.jpg) | A photo of the Teensy 4.1, its top and its bottom, with the pin numbers printed on the bottom. |
| [`Teensy4.jpg`](Teensy4.jpg) | A photo of the Teensy 4.0, with its pin numbers. |

StepIt Motors runs on a Teensy 4.0 or 4.1, with the same wiring: the pins it drives, 0 to 15, are at the same places on both boards. The firmware has a build for each, `teensy40` and `teensy41` in [`src/stepit_mcu/platformio.ini`](../../src/stepit_mcu/platformio.ini); how to flash it is in [Install StepIt Motors on the Microcontroller](../../README.md#install-stepit-motors-on-the-microcontroller) of the README.

The photos of the TMC2208 and of the protector are by [Watterott](https://github.com/watterott/SilentStepStick), [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). The photo of the Teensy 4.1 is by [SpencerWF](https://commons.wikimedia.org/wiki/File:Teensy_4.1.jpg), [CC0](https://creativecommons.org/publicdomain/zero/1.0/).
