The firmware of StepIt Motors, a PlatformIO project for a Teensy 4.0 or a
Teensy 4.1: one build for each board, teensy40 (the default) and teensy41.

Flash the build of your board:

    pio run -e teensy40 -t upload
    pio run -e teensy41 -t upload

See "Install StepIt Motors on the Microcontroller" in the README at the root.
