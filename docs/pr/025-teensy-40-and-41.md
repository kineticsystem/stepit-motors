# The Teensy 4.0 and the Teensy 4.1

Pull request [#25](https://github.com/kineticsystem/stepit-motors/pull/25), branch `teensy-40-and-41`.

## Goal

The rig of StepIt Macro now runs on a Teensy 4.0 instead of a Teensy 4.1. The firmware was built for the 4.1 only, and the documents said the 4.0 needed a change. StepIt Motors now works with both boards, and every document says so.

## Decisions

**Two builds, not one.** `platformio.ini` has a build per board, `teensy40` and `teensy41`, which share everything but their `board`. Each build is linked for the memory of its board, so one build cannot serve both.

**The Teensy 4.0 is the default.** It is the board of the rig. A Teensy 4.1 is flashed with `pio run -e teensy41 -t upload`.

**No change to the code.** The two boards have the same processor, and the pins the firmware drives, 0 to 15, sit at the same places on both. The PiBot board is wired the same way to either.

**The design notes keep the Teensy 4.1.** They record tests run on a 4.1. Only their flash command changes, to `-e teensy41`: without it, the command would now flash the 4.0 build.

**Flashing on the Raspberry Pi is documented.** PlatformIO Core on the Pi 5 downloads the compiler and the loader for ARM64 itself. The rig's Teensy 4.0 was flashed that way.

## Tested

On the rig's Raspberry Pi 5, with the Teensy 4.0 and no motors attached:

- Both builds compile.
- The Teensy 4.0 flashes with `pio run -e teensy40 -t upload`.
- The driver connects and reads firmware 1.1.1, `/joint_states` comes at 30 Hz, and position and velocity goals on the five joints come back as commanded.

## Open Items

The pins are not checked with motors on the Teensy 4.0. They are the same as on the 4.1 according to the pinouts of PJRC.
