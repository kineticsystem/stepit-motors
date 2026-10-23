# The Documents of StepIt Motors

How to build, run and test StepIt Motors is in the [README](../README.md). This folder holds everything else, one folder per topic.

| Folder | What it holds |
|---|---|
| [`hardware`](hardware/README.md) | The datasheets and pictures of the Teensy, the PiBot board, the TMC2208 drivers and their protectors. |
| [`design`](design) | Design notes of the changes, one per change, named after its date. |
| [`background`](background) | Reading on stepper motors. |
| [`images`](images) | The logo. |

## Design Notes

| Document | What it is |
|---|---|
| [Controller handshake](design/2026-09-22_controller_handshake_limits_and_id_validation.md) | The handshake with the controller: its identity, the IDs of the motors, their limits and the version of the firmware. |
| [Stop the motors on a rejected motion command](design/2026-09-23_stop_the_motors_on_a_rejected_motion_command.md) | What the firmware and the driver do when the firmware rejects a motion command. |

## Background

| Document | What it is |
|---|---|
| [Stepper Motors](background/StepperMotors.md) | How stepper motors work: phases, driving modes, construction and wiring. |
