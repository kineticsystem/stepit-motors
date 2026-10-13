# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

StepIt is a ROS 2 (Jazzy) project for controlling stepper motors via a Teensy 4.x microcontroller. It uses `ros2_control` as the hardware abstraction layer. The robot can run in simulation (fake motors) without any physical hardware.

## Build & Development Commands

**Install dependencies:**
```bash
./bin/update.sh        # runs: rosdep install --ignore-src --from-paths . -y -r
```

**Build:**
```bash
./bin/build.sh         # colcon build with Debug, compile_commands.json, symlink-install
```

**Run all tests:**
```bash
./bin/test.sh          # colcon test + test-result --all --verbose
```

**Run a single test package:**
```bash
colcon test --packages-select stepit_driver --return-code-on-test-failure
colcon test-result --all --verbose
```

**Run a specific test binary directly (after build):**
```bash
./build/stepit_driver/tests/test_default_driver
```

**Lint/format:**
```bash
pre-commit run -a                          # run all pre-commit hooks manually
clang-format -i <file>                    # format a single C++ file
```

**Launch the simulation:**
```bash
source install/setup.bash
ros2 launch robot_description robot.launch.py   # add launch_rviz:=true to open RViz
```

Whether the fake or the real driver is used is set by the launch argument `use_dummy` (`true` by
default, i.e. simulation), next to `usb_port` and `baud_rate`: `robot.launch.py` passes them to
xacro as the arguments of `src/robot_description/urdf/stepit.urdf.xacro`. Never change them in the
xacro to configure a robot: StepIt Macro sets them as launch arguments.

## Architecture

The project is a ROS 2 workspace with two top-level directories:

- **`src/`** — ROS 2 packages (ament/colcon)
- **`modules/`** — git submodules (non-ROS C++ libraries)

### ROS 2 Packages (`src/`)

| Package | Role |
|---|---|
| `stepit_hardware` | `ros2_control` hardware interface plugin (`StepitHardware`, exported as `stepit_driver/StepitHardware`) |
| `stepit_driver` | `Driver` interface and implementations (`DefaultDriver`, `FakeDriver`) used by `StepitHardware` |
| `stepit_motors_msgs` | `MotorsStatus`, the message of `/motors/status` |
| `robot_description` | URDF/xacro robot model, RViz config, controllers config, `robot.launch.py` |
| `robot_bringup` | Top-level launch: includes `robot_description` |
| `stepit_hardware_tests` | Integration tests requiring real hardware |
| `stepit_mcu` | PlatformIO project for Teensy firmware (not built by colcon) |

### Submodules (`modules/`)

- **`serial`** — Low-level cross-platform serial port C++ library (wjwwood/serial)
- **`framed-serial`** — the `framed_serial` ROS 2 package: framed, CRC-checked messages over the serial port, shared with StepIt Freezer ([kineticsystem/framed-serial](https://github.com/kineticsystem/framed-serial)). CI fetches both submodules through `stepit.repos`, since its checkout does not recurse into submodules: add a new submodule there too.

### Key Design Pattern: `stepit_hardware` / `stepit_driver`

`StepitHardware` (a `hardware_interface::SystemInterface` plugin) delegates all hardware communication to a `Driver` interface:

- **`DefaultDriver`** — real hardware, communicates via `FramedSerial` → framed serial → Teensy
- **`FakeDriver`** — simulation, uses `FakeMotor` with `VelocityControl`/`PositionControl` internally
- **`DefaultDriverFactory` / `DriverFactory`** — the factory is injected at construction time; in tests a mock factory is used

The `StepitHardware` reads joint configuration (motor IDs, max velocity, acceleration) from the URDF `<ros2_control>` block via `HardwareInfo`.

`StepitHardware` publishes whether it talks to the controller on `~/status` of the node ros2_control gives the hardware, named after the hardware: `motors` in `stepit.urdf.xacro`, hence `/motors/status`, which StepIt Macro's page reads, so never rename the hardware. The topic is transient local, published on a change and every second by a timer on that node. `read()` and `write()` run in the realtime loop and never publish: they only record a change with `set_status()`. Without an executor, e.g. in a test that calls `on_init()` directly, there is no node and nothing is published; the status tests call `init()` with an executor.

### Communication Stack (real hardware)

```
StepitHardware → DefaultDriver → FramedSerial (framed-serial submodule) → serial (submodule) → USB/serial → Teensy
```

Frames are delimited by `0x7E`, `0x7E`/`0x7D` in the data are escaped with `0x7D` and XOR `0x20`, and each frame ends with a CRC-16/KERMIT. The protocol carries typed request/response messages defined in `stepit_driver/msgs/`.

### Testing Approach

Tests use **GMock** (`ament_add_gmock`). Mocks live alongside tests:
- `src/stepit_driver/tests/mock/mock_framed_serial.hpp` — mocks the serial layer for `test_default_driver`
- `src/stepit_hardware/tests/mock/mock_driver.hpp` / `mock_driver_factory.hpp` — mock the driver for `test_stepit_hardware`
- `src/stepit_hardware/tests/fake/fake_hardware_info.hpp` — constructs `HardwareInfo` for unit tests without a URDF

## Code Standards

- C++20, compiled with `-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast`
- Formatting: `clang-format` (pre-commit hook, style in `.clang-format`). The hook calls the
  **unversioned** binary, so the version is whatever the environment provides: Ubuntu 24.04,
  the dev container and CI all give 18. Do not pin a version in the hook without also pinning
  it in `ci-format.yml`, or CI and local will format differently.
- Python formatting: `black`
- Spell check: `codespell` (pre-commit)
- Pre-commit hooks run automatically on commit; run `pre-commit run -a` to check all files manually

## CI

Three GitHub Actions workflows:
- `industrial_ci.yml` — builds and tests in Docker (Ubuntu 24.04 + ROS Jazzy) via ros-industrial/industrial_ci
- `ci-format.yml` — pre-commit hooks (clang-format, black, codespell)
- `ci-ros-lint.yml` — ROS-specific linters

Local CI can be run with [Nektos `act`](https://github.com/nektos/act); see "How to run GitHub
Actions locally" in `README.md` for the setup it needs (runner image mapping, and moving
`build/`, `install/` and `log/` aside before running `industrial_ci`).
