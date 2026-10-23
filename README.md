# StepIt Motors

[![CI](https://github.com/kineticsystem/stepit-motors/actions/workflows/industrial_ci.yml/badge.svg)](https://github.com/kineticsystem/stepit-motors/actions/workflows/industrial_ci.yml)
[![Format](https://github.com/kineticsystem/stepit-motors/actions/workflows/ci-format.yml/badge.svg)](https://github.com/kineticsystem/stepit-motors/actions/workflows/ci-format.yml)
[![Linters](https://github.com/kineticsystem/stepit-motors/actions/workflows/ci-ros-lint.yml/badge.svg)](https://github.com/kineticsystem/stepit-motors/actions/workflows/ci-ros-lint.yml)

<img src="docs/images/logo.png" width="50%">

## Table of Contents <!-- omit in toc -->

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
  - [Protecting the Drivers](#protecting-the-drivers)
  - [Cooling the Drivers](#cooling-the-drivers)
- [Install StepIt Motors on the Microcontroller](#install-stepit-motors-on-the-microcontroller)
- [Install StepIt Motors on the Local Computer](#install-stepit-motors-on-the-local-computer)
  - [Chekout the Git Repository](#chekout-the-git-repository)
  - [Pre-Commit Hooks](#pre-commit-hooks)
  - [Build the Project](#build-the-project)
- [Running the Application](#running-the-application)
- [How to run GitHub Actions locally](#how-to-run-github-actions-locally)

## Introduction

StepIt Motors is a project to control stepper motors with a Teensy microcontroller and ROS2. Watch [this video](https://github.com/user-attachments/assets/e67d46ce-e133-4e34-bab8-7d924be3dee4)

The other documents, on the hardware and the design of the changes, are listed in [docs/README.md](docs/README.md).

## Prerequisites

To run StepIt Motors, we need a computer with Ubuntu 24.04. The preferred way to run it is inside a Docker container.

If you want to run StepIt Motors on your host machine, you must install ROS2. Please refer to the document [Install ROS2 Jazzy on Ubuntu](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html).

By default, StepIt Motors runs in simulation mode so we do not an need actual hardware to play around with it.

For a real application, we recommend attaching the stepper motors to a Teensy microcontroller 4.0 or 4.1. We can hook the motors in many different ways but we suggest the following hardware configuration.

- 1 x [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) or Teensy 4.0.
- 1 x [PiBot multi-stepper motor driver board rev2.3 ](https://www.pibot.com/pibot-multi-stepper-motor-driver-board-rev2-3.html)
- 5 x [TMC2208](https://shop.watterott.com/SilentStepStick-TMC2208-Stepper-Motor-Driver-with-soldered-pinheaders)
- 5 x [SilentStepStick Protector](https://shop.watterott.com/SilentStepStick-Protector-for-Stepper-Motor-Drivers), one under each TMC2208: see [Protecting the Drivers](#protecting-the-drivers).
- 5 x heatsinks for the TMC2208, small blue aluminium ones with thermal adhesive tape: see [Cooling the Drivers](#cooling-the-drivers).

The Teensy is connected to a computer using a USB cable. For a portable application, we can use a Raspberry Pi.

### Protecting the Drivers

Each TMC2208 sits on a [Watterott SilentStepStick Protector](https://learn.watterott.com/silentstepstick/protector/), a small board plugged into the driver's socket on the PiBot board, with the driver plugged into it. It adds flyback diodes on the four motor outputs, and its version 2 one more diode, from the logic supply `VIO` to the motor supply `VM`.

<table>
  <tr>
    <td><img src="docs/hardware/TMC2208.jpg" width="150" alt="The SilentStepStick TMC2208 driver"></td>
    <td><img src="docs/hardware/Protector.jpg" width="150" alt="The SilentStepStick Protector"></td>
  </tr>
  <tr>
    <td>SilentStepStick TMC2208</td>
    <td>SilentStepStick Protector</td>
  </tr>
</table>

Photos: [Watterott](https://github.com/watterott/SilentStepStick), [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

A stepper motor is also a generator, and the diodes give the current it makes a safe way back, instead of through the driver:

- **A motor turned by hand.** Moving an axis by hand while the drivers are off makes a voltage that goes back into the driver's outputs and can lift `VM` or partly power the driver. The Trinamic drivers are sensitive to it, and an axis is often moved by hand to set it up.
- **A coil opened under current.** A motor unplugged while its coil carries current makes a voltage spike that can destroy the driver at once. The diodes take most of the spike.

The MOSFETs of the TMC2208 have diodes of their own, so the protector is a margin, not a necessity. It costs about as much as a driver, much less than finding which axis died and why.

What it costs:

- **One more row of contacts** between the board and the driver. A loose one gives an axis that skips steps or stops now and then: push both boards fully into their sockets.
- **The driver sits higher.** Check that its heatsink still has air around it.
- **Two rules of Watterott's**, because of the diode from `VIO` to `VM`: never enable the drivers while `VM` is off, and never power anything else from `VM`, e.g. a fan, or its current comes from `VIO`.

> [!IMPORTANT]
> The protector does not make plugging a motor under power safe: never plug or unplug a motor while the drivers are powered.

### Cooling the Drivers

Each TMC2208 carries a small aluminium heatsink, stuck on with its thermal adhesive tape. Its chip sits on the underside of the SilentStepStick and sends its heat through the board, so the heatsink goes on the **top side of the board**, the side that faces up once the driver is plugged in, not on the chip. See [the SilentStepStick FAQ](https://learn.watterott.com/silentstepstick/faq/).

- **Up to 850 mA RMS** per motor, a small heatsink is enough, according to Watterott.
- **Above 850 mA RMS**, use a heatsink that covers nearly the whole top of the board, and a fan.
- **Leave air around the drivers** in any case: the heatsink only spreads the heat, the air carries it away.

The current of each motor is set by the potentiometer of its TMC2208: set it no higher than the motor needs, which also keeps the driver cool.

## Install StepIt Motors on the Microcontroller

This step is only required if you use a real hardware, otherwise skip to the following section.

We developed code for the Teensy microcontroller using Visual Studio Code because it provides very good tools to format and validate the code. To achieve this goal, we must install [PlatformIO](https://platformio.org) extension which supports different microcontrollers including Arduino. Installing Arduino IDE is not required.

If PlatformIO cannot find the Python interpreter, install the following:

`sudo apt install python3-venv`

To connect StepIt Motors to a Teensy, you must install a udev rule on your host first. Without this rule, non-root users generally can't access the Teensy's HID interface that the loader/programmer needs.

```bash
cd /tmp
wget https://www.pjrc.com/teensy/00-teensy.rules
sudo cp /tmp/00-teensy.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
```

If necessary, unplug/replug the Teensy.

To flash the microcontroller code into the Teensy:

- Open VSCode
- Install the extension PlarformIO
- Open the subfolder `stepit_mcu` in VSCode
- Select the correct device and flash it

The microcontroller is usually shown as `/dev/ttyACM0`. To drive it instead of the fake motors, start the application with `use_dummy:=false`, see [Running the Application](#running-the-application).

## Install StepIt Motors on the Local Computer

### Chekout the Git Repository

Check out this git repository, including all required submodules.

```
git clone --recurse-submodules git@github.com:kineticsystem/stepit-motors.git
```

If you missed the switch `--recurse-submodules`, you can clone all dependencies with the following commands:

```
cd stepit-motors
git submodule update --init --recursive
```

Remember to enable recursion for relevant git commands, such that regular commands recurse into submodules by default.

```
git config --global submodule.recurse true
```

### Pre-Commit Hooks

Additionally, you should install git pre-commit hooks using the following command:

```
pre-commit install
```

### Build the Project

The preferred way to build and run StepIt Motors is to use a Docker container. It is defined in [`docker/docker-compose.yml`](docker/docker-compose.yml) and driven by the [`docker/dock.sh`](docker/dock.sh) script. See [docker/README.md](docker/README.md) for more details.

> [!IMPORTANT]
> The docker container provides a default user `developer` with password `developer`. That user may run `sudo` without being asked for it, so that the scripts in `bin` also work from a non-interactive shell, e.g.
> `docker exec stepit-motors update.sh`.

Build the image and create the container. The script always mounts the repo it belongs to, so it can be called from anywhere:

```
./docker/dock.sh stepit-motors build
```

This is also how you pick up a change to the `Dockerfile`: it rebuilds only the layers that changed, so there is no need to clean first.

Start the container with an interactive shell:

```
./docker/dock.sh stepit-motors start
```

The commands below assume you are inside the container (or, if you prefer not to use Docker, directly on a host machine with Ubuntu 24.04 and ROS 2 Jazzy installed).

Inside the container the scripts in [`bin`](bin) are on the `PATH` and aliased, so they can be called from any directory: they always act on the workspace root. Outside the container, call them by their path instead, e.g. `./bin/update.sh`.

Install all required dependencies.

```
update
```

Run Colcon to build the project.

```
build
```

Execute all tests.

```
test
```

## Running the Application

By default, the application runs with fake motors and a default active trajectory controller. Run the following commands to start StepIt Motors. RViz does not start by default.

```
source ~/ws/install/setup.bash
ros2 launch robot_bringup launch.py
```

To also start RViz and see the robot move, set the launch argument `launch_rviz`:

```
ros2 launch robot_bringup launch.py launch_rviz:=true
```

To drive the microcontroller instead of the fake motors, set `use_dummy`, and `usb_port` if it is not on `/dev/ttyACM0`:

```
ros2 launch robot_bringup launch.py use_dummy:=false usb_port:=/dev/ttyACM0
```

| Launch argument | Default | Description |
|---|---|---|
| `launch_rviz` | `false` | Start RViz. |
| `use_dummy` | `true` | Use fake motors instead of the microcontroller. |
| `usb_port` | `/dev/ttyACM0` | Serial port of the microcontroller. |
| `baud_rate` | `9600` | Speed of the serial port, the one of the firmware. |

They are xacro arguments of [`stepit.urdf.xacro`](src/robot_description/urdf/stepit.urdf.xacro), which passes them to the `<hardware>` of [`stepit.ros2_control.xacro`](src/robot_description/urdf/stepit.ros2_control.xacro).

The hardware, named `motors`, tells whether it talks to the controller on `/motors/status` (`stepit_motors_msgs/msg/MotorsStatus`): `connected`, the serial port, or `fake`, in `device`, and in `message` why it is not connected, e.g. the error of the serial port. ros2_control gives the hardware a node named after it, which publishes the status when it changes and every second, and keeps the last one for a client that subscribes late. When a read or a write fails, e.g. because the Teensy was unplugged, ros2_control deactivates the hardware, and it stays disconnected until `ros2_control_node` starts again:

```
ros2 topic echo /motors/status --qos-durability transient_local
```

Open a different terminal (if using Docker, attach to the same running container with `./docker/dock.sh stepit-motors start`) and run the following command to rotate `joint1` by 6.28 rad clockwise:

```
ros2 topic pub -1 /joint_trajectory_controller/joint_trajectory trajectory_msgs/msg/JointTrajectory "{
  joint_names: [joint1],
  points: [
    {
      positions: [-6.28],
      time_from_start: {
        sec: 2
      }
    }
  ]
}"
```

To control a trajectory with a sequence of positions and velocities run

```
ros2 topic pub -1 /joint_trajectory_controller/joint_trajectory trajectory_msgs/msg/JointTrajectory "{
  joint_names: ["joint1", "joint2", "joint3", "joint4", "joint5"],
  points: [
    {
      positions: [0, 0, 0, 0, 0],
      velocities: [0, 0, 0, 0, 0],
      time_from_start: {
        sec: 0,
        nanosec: 0
      }
    },
    {
      positions: [31.4159265358979,37.6991118430775,43.9822971502571,50.2654824574367,56.5486677646163],
      velocities: [0, 0, 0, 0, 0],
      time_from_start: {
        sec: 10,
        nanosec: 0
      }
    },
    {
      positions: [0, 0, 0, 0, 0],
      velocities: [0, 0, 0, 0, 0],
      time_from_start: {
        sec: 12,
        nanosec: 0
      }
    },
    {
      positions: [-31.4159265358979,-37.6991118430775,-43.9822971502571,-50.2654824574367,-56.5486677646163],
      velocities: [0, 0, 0, 0, 0],
      time_from_start: {
        sec: 17,
        nanosec: 0
      }
    },
    {
      positions: [0, 0, 0, 0, 0],
      velocities: [0, 0, 0, 0, 0],
      time_from_start: {
        sec: 20,
        nanosec: 0
      }
    },
  ]
}"
```
The trajectory is a list of waypoints, each of them containing the desired position and velocity of each joint at a given time.

> [!IMPORTANT]
> Only one controller must be active at a time: `velocity_controller`, `position_controller`, or `joint_trajectory_controller`.

To use the velocity controller, deactivate the current controller before activating another one:

```
ros2 control set_controller_state joint_trajectory_controller inactive -c /controller_manager
ros2 control set_controller_state velocity_controller active -c /controller_manager
```

Then run the following commands:

```
ros2 topic pub -1 /velocity_controller/commands std_msgs/msg/Float64MultiArray "data: [6.28,-6.28,0,0,0]"
```

Each value is the target velocity, in rad/s, for the corresponding joint listed under `velocity_controller.joints` in `controllers.yaml` (currently `joint1` through `joint5`), so the array must have exactly one value per joint.

To drive the velocity controller with a gamepad, see [Driving the Robot with a Gamepad](https://github.com/kineticsystem/stepit-macro/blob/main/docs/Gamepad.md) in StepIt Macro, which switches the controllers through its commander.

The controller `position_controller` is also loaded but inactive. Deactivate any previous controller as per previous note.

```
ros2 control set_controller_state velocity_controller inactive -c /controller_manager
ros2 control set_controller_state position_controller active -c /controller_manager
```

```
ros2 topic pub -1 /position_controller/commands std_msgs/msg/Float64MultiArray "data: [6.28, 6.28, 6.28, 6.28, 6.28]"
```

## How to run GitHub Actions locally

At each commit, the GitHub repository runs all available tests using [GitHub actions](https://docs.github.com/en/actions) and [Industrial CI](https://github.com/ros-industrial/industrial_ci).

A GitHub action fires up a docker container with Ubuntu 24.04 and ROS2 Jazzy, checks out and builds the code inside the docker container and runs all tests.

Sometimes, it may be desirable to execute the Continuous Integration pipeline locally. This is possible by using [Nektos](https://github.com/nektos/act).

Install the `act` command in the user folder `~/bin` as explained in the Nektos README.md file. The `.env` file at the root of the repository defines the global variables required by Industrial CI.

On its first run `act` asks interactively which runner image to use and aborts if it cannot
prompt, so choose the image up front. All three workflows run on `ubuntu-24.04`. Create
`~/.config/act/actrc` with:

```
-P ubuntu-latest=catthehacker/ubuntu:act-latest
-P ubuntu-24.04=catthehacker/ubuntu:act-24.04
-P ubuntu-22.04=catthehacker/ubuntu:act-22.04
```

Then run any of the three workflows from the root of the repository:

```
~/bin/act pull_request --workflows ./.github/workflows/ci-format.yml    -s GITHUB_TOKEN=""
~/bin/act pull_request --workflows ./.github/workflows/ci-ros-lint.yml  -s GITHUB_TOKEN=""
~/bin/act pull_request --workflows ./.github/workflows/industrial_ci.yml -s GITHUB_TOKEN=""
```

The repository is public and `act` runs against the working tree rather than checking the
code out, so no GitHub token is needed; the empty secret above simply stops `act` from
prompting for one.

`industrial_ci` runs as a Docker action that mounts the workspace, so it does not honour
`.gitignore` and `rosdep` ends up scanning `build/` and `install/` too, and fails.

The `build` script prevents this by dropping a `CATKIN_IGNORE` file into each of those directories, which is the marker `rosdep` looks for.
