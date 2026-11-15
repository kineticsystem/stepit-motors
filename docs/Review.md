# StepIt Motors Review

## Table of Contents <!-- omit in toc -->

- [Scope and Method](#scope-and-method)
- [Verdict](#verdict)
- [The Packages and Their Boundaries](#the-packages-and-their-boundaries)
- [Interfaces and Abstractions](#interfaces-and-abstractions)
- [Separation of Concerns](#separation-of-concerns)
- [SOLID](#solid)
- [The Safety Nets](#the-safety-nets)
- [The Serial Link](#the-serial-link)
  - [The Handshake](#the-handshake)
  - [After a Failed Cycle](#after-a-failed-cycle)
  - [Which Thread Switches the Controllers](#which-thread-switches-the-controllers)
- [The Firmware](#the-firmware)
- [The Fake Motors](#the-fake-motors)
- [Coupling and Duplication](#coupling-and-duplication)
- [Testability and Tests](#testability-and-tests)
- [Implementation Issues](#implementation-issues)
- [Recommendations](#recommendations)

Reviewed on 2026-11-15, at commit `dc269f8`. It judges the design that [docs/ARCHITECTURE.md](ARCHITECTURE.md) describes, as that document stands in the working tree on the same day.

## Scope and Method

This review covers the whole module: the firmware of the Teensy in [`src/stepit_mcu`](../src/stepit_mcu), 15 source files and 1836 lines; [`stepit_driver`](../src/stepit_driver), 2575 lines with the fake motors; [`stepit_hardware`](../src/stepit_hardware), 892 lines; their tests, 2558 lines; the two board programs of [`stepit_hardware_tests`](../src/stepit_hardware_tests); the launch files, the xacro and [`controllers.yaml`](../src/robot_description/config/controllers.yaml); and the manifests and build files of every package. `serial`, third-party, is not reviewed; it was read only for what `open()` does.

We read every file of these parts, and the neighbours the rig's `CLAUDE.md` names: `rig.yaml` and `rig.launch.py` of StepIt Macro, its behaviours `CommandJointPositions` and `SwitchController`, the host side of `framed-serial`, the copy of the firmware's protocol layer in StepIt Freezer, compared file by file, and the three `.repos` files. For the two questions the architecture document could not settle, we read the source of `ros2_control` 4.48.1, the version installed in the rig's image, and then measured both.

Each suspected bug of the host was reproduced with a throwaway gmock probe, in a copy of the packages in a scratch workspace, built and run inside the `stepit-macro` image on ROS domain 82, then deleted: nothing was built in the repository. The probes of the link put the real `DefaultDriver` and the real `DefaultFramedSerial` on a fake `framed_serial::Serial` that plays the Teensy byte by byte, with the framing of `SerialPort.cpp` and the commands and conversions of `main.cpp`. The probes of the lifecycle wrap `StepitHardware` in `hardware_interface::System`, as `ros2_control` does. One probe started `robot.launch.py` on the fake motors, with two log lines added to the scratch copy of `stepit_hardware.cpp`, to see which thread switches the controllers. A finding marked **verified** failed its probe; the others are **from reading**.

The firmware cannot be tested without the board, and no board was touched. Its two conversion functions, `radiansToSteps()` and `stepsToRadians()`, are plain arithmetic, so they were compiled on the host side and run over 400,001 positions; every other finding of the firmware is from reading.

The whole suite: `colcon test` passes 57 gmock tests in 4 programs, with no failure and no skip, in about 6 s.

## Verdict

The design is sound, and its one idea holds: `StepitHardware` reaches the motors only through `Driver`, nothing in it tells the fake from the real driver by type, and the includes run one way. The safety nets that matter are there and were traced through the code: the firmware checks a whole command before applying any of it and stops every motor when it rejects one, its watchdog stops them 1 s after the host goes silent, a released joint gets a velocity of 0, and every stop decelerates. The switching of controllers is the most careful part of the module: its claims and its `stop_pending` flag are right for a switch that runs on another thread, which is what the rig asks for, measured below.

Three weaknesses, none of which moves a motor today:

- **The handshake pairs replies with requests by order alone, and reads a reply it does not measure.** One reply left over when the port opens, or one that comes after its 0.2 s, puts the handshake one reply behind: it then builds the command that pins the motors from the bytes of an `Info` reply, reading 10 bytes past its end, and sends it (finding 1, verified). The Teensy rejects that command only because two bytes of the limits happen to sit where motor ids are read.
- **A failed cycle is final.** After one failed `read()` or `write()` nothing is sent, the port stays open, and configuring the hardware again cannot work: only a restart of `ros2_control_node` brings the motors back (finding 2, verified). The motors are stopped by the firmware alone.
- **The fake motors are not the board** in five places that a robot description or an objective can meet (finding 4, verified), so a test on the fake does not show what the board does when a command is rejected or the hardware is deactivated.

**Safety for the rig.** No path was found on which a motor keeps turning after its controller is released, the hardware deactivated or the host lost. One finding is a safety issue in principle: nothing, on the host or in the firmware, refuses a position that is not a number or is out of range (finding 5), and nothing limits where a joint may go. It needs a client that publishes such a value.

## The Packages and Their Boundaries

The includes between the packages, counted with `grep`:

| From | To | Includes | Verdict |
|---|---|---|---|
| `stepit_hardware` | `stepit_driver` | 8 | As designed. |
| `stepit_hardware` | `hardware_interface`, `pluginlib`, `rclcpp` and `rclcpp_lifecycle`, `stepit_motors_msgs` | 8, 1, 3, 1 | As designed. |
| `stepit_driver` | `framed_serial` | 11 | As designed. |
| `stepit_driver` | `hardware_interface` | 6 | A documented price: the factories read `HardwareInfo`. `Driver` and the two drivers do not include it. |
| `stepit_driver` | `rclcpp` | 9 | `rclcpp::Time` in the `Driver` interface, and the logger. |
| `stepit_driver` | `stepit_hardware` | 0 | Kept: no cycle. |
| `stepit_hardware_tests` | `stepit_driver`, `framed_serial` | 3, 4 | As designed. |

| Rule | Verdict | Evidence |
|---|---|---|
| `StepitHardware` never opens a port or encodes a frame | Kept | It includes no header of `framed_serial`, and calls only the 7 methods of `Driver`. |
| It never tells the fake from the real driver by type | Kept | No `dynamic_cast` in the package. It reads `use_dummy` a second time, in `on_init()`, only to name the device on `/motors/status`. |
| `stepit_driver` knows no joint, controller or lifecycle state | Kept in the code | Its `package.xml` still declares `rclcpp_lifecycle` and `lifecycle_msgs`, which no file includes. |
| The launch arguments, not the xacro, configure a robot | Kept, but one argument is dead | `baud_rate` reaches the xacro and stops there: finding 8. |
| A package builds with what its manifest names | Broken twice | Finding 6. |

`FakeDriver` ships in the production library, under `include/stepit_driver/fake`. That is a choice, not a leak: the robot runs on it by default.

## Interfaces and Abstractions

| Interface | Where | Judgement |
|---|---|---|
| `Driver` | [`driver.hpp`](../src/stepit_driver/include/stepit_driver/driver.hpp) | One method per command, in radians and seconds: the right seam, and the 36 tests of the hardware stand on it. Five of its methods are `const` although each changes the controller, so `FakeDriver` keeps its motors in a `mutable` map. The `time` argument means two things: the real driver ignores it, the fake one moves its motors to it, and `StepitHardware` passes `rclcpp::Time{}` where it has no time, which the fake takes for a time 0: finding 4. |
| `DriverFactory` | [`driver_factory.hpp`](../src/stepit_driver/include/stepit_driver/driver_factory.hpp) | Used as designed: the plugin builds the real one, the tests give a factory of mocks. |
| `FramedSerialFactory`, `SerialFactory` | `stepit_driver/include` | Two more interfaces with one implementation each, never replaced: no test gives another, and the factories have no test ([Tests](ARCHITECTURE.md#tests)). Three factories for one chain of three objects is one seam used and two unused. |
| The messages of `stepit_driver/msgs` | 8 headers | Plain values. Their getters return vectors by value, so `read()` copies the motor states three times a cycle. `VelocityCommand` knows its command id, `PositionCommand` and `ConfigCommand` do not: the ids are spread over `velocity_command.cpp` and `default_driver.cpp`. `Response::Status` is built from the raw byte, so a byte that is neither `0x11` nor `0x12` is a third, unnamed status: `write()` compares with `Success`, but `on_configure()` compares the answer to `configure()` with `Failure` (line 223) and takes anything else for a success. |
| `framed_serial::FramedSerial` | the shared library | Mocked in `test_default_driver`: the tests see whole frames, each the answer to its request, never bytes, which is why finding 1 has no test. It has no call to drop unread bytes. |
| The firmware's `SerialPort` | [`SerialPort.cpp`](../src/stepit_mcu/src/SerialPort.cpp) | Calls the global `Serial` directly: the framing of the firmware cannot be compiled on the host. |
| The commands of `main.cpp` | [`main.cpp`](../src/stepit_mcu/src/main.cpp) | The parsing, the checks, the steppers and the globals are in one file of 640 lines. The checks are pure and would run on the host, but they sit between `AccelStepper` calls. |

## Separation of Concerns

- **The hardware interface keeps to its job.** It turns joints into driver calls, checks the description against the controller's limits, and records the status; the publishing is on a timer outside the loop.
- **The limits of the board are known in the right place**, the firmware, and reported in the handshake; the copy in `hardware_limits.hpp` exists only for the fake motors, and `get_info` compares the two on the board.
- **The choice of the driver is known twice**: `DefaultDriverFactory::create()` and `StepitHardware::on_init()` (line 155) each parse `use_dummy`, with the same rule written two ways.
- **The description states limits that are not true.** The joints of [`stepit.sample_robot.xacro`](../src/robot_description/urdf/stepit.sample_robot.xacro) are `revolute` with `lower="-3.14" upper="3.14"` and `velocity="0.0"`, while the rig's rail turns many times and the motors reach 18.8 rad/s. Nothing enforces them today: `enforce_command_limits` of the controller manager is off. Turning it on would clamp every move to half a turn.

## SOLID

| Principle | Verdict | Where |
|---|---|---|
| Single responsibility | Kept on the host; partly violated in the firmware | `StepitHardware`, `DefaultDriver`, `FakeDriver` and `FakeMotor` each do one thing. `main.cpp` holds the protocol, the checks, the motion and the watchdog. |
| Open/closed | Kept | Another board is another `Driver` and a factory; `StepitHardware` does not change. A new command changes `Driver` and its implementations, which is expected of a protocol. |
| Liskov substitution | Violated in behaviour | `FakeDriver` keeps the signatures of `Driver`, not what the board does behind them: [The Fake Motors](#the-fake-motors). |
| Interface segregation | No violation found | `Driver` has 7 methods; `StepitHardware` calls 6, and `disconnect()` is called by nothing: finding 2. |
| Dependency inversion | Kept | `StepitHardware` takes a `DriverFactory`, `DefaultDriver` a `FramedSerial`. |

## The Safety Nets

Each net of [Safety Nets](ARCHITECTURE.md#safety-nets) was traced through the code:

| Net | Verdict | Evidence |
|---|---|---|
| The watchdog stops the motors 1 s after the last good frame | Kept, from reading | `loop()` of `main.cpp` calls `stopAllMotors()` once `millis() - lastMessageReceivedTime` passes `TIMEOUT_MS`; `processBuffer()` resets the time for any frame with a good CRC, an unknown command included. It guards against a dead host only: see below. |
| A released joint is stopped | Kept, tested | `perform_command_mode_switch()` clears the claim, then raises `stop_pending`; `write()` takes the flag before it reads the claims. 7 tests, one of a release in the middle of `write()`. |
| Deactivation stops every motor | Kept, tested | `on_deactivate()` sends a velocity of 0 to every motor. `ros2_control` does not call it after a failed cycle: [After a Failed Cycle](#after-a-failed-cycle). |
| A rejected Move or Speed stops every motor | Kept, from reading | `moveCommand()` and `speedCommand()` parse every entry into local arrays, and call `stopAllMotors()` on each of their 4 rejections. |
| A rejected Config changes nothing | Kept, from reading | `configureCommand()` applies in a second pass. |
| A stop decelerates | Kept, from reading | A goal speed of 0 makes `run()` call `AccelStepper::stop()`, and the motor stays at rest: `isDecelerating()` holds until a goal asks for more speed. A Speed of 0 holds the position at the time of the command and does not come back to it. |
| A new session does not resume an old goal | Kept, with two prices | `hold_motors()` and Config both set the goal to the current position. On a motor that still turns, which takes a host that reconnects within the watchdog's second, that is the stop the rig's `CLAUDE.md` warns against: the motor brakes past the position and comes back. And `hold_motors()` moves one motor in eight by one step: finding 3. |
| The description cannot ask more than the board | Kept, tested | `joints_are_within_limits()`, then the firmware's own check. |
| The handshake refuses the wrong device and version | Kept for a quiet link | With a reply in the way it refuses the right controller, or accepts it one reply behind: finding 1. |

**What the watchdog does not see.** Every `read()` keeps it quiet, so it guards against a host that died, not against a client that did. A velocity controller keeps its last command for as long as nothing replaces it: if the node that publishes velocities dies with a stick held, the motors turn until someone switches the controller. The rig covers its own teleoperation with the 0.5 s timeout of `gamepad_teleop`, inside that node; nothing in this module bounds how long a velocity command lives. It is a choice, and a sound one for a module that is a generic `ros2_control` hardware, but [Safety Nets](ARCHITECTURE.md#safety-nets) should name it among what is deliberately not there.

**No limit on where a joint goes.** Neither the firmware, the hardware interface nor the description bounds a position, and there are no limit switches. For a rail with a camera on it, the end of the rail and the subject are protected only by what the client sends. See finding 5.

## The Serial Link

### The Handshake

`DefaultDriver::connect()` (line 59) opens the port and asks `Info` up to 5 times. Each method of the driver writes its request and reads the next frame; nothing drops what is waiting on the port first, and `serial::Serial::open()` does not flush its input. So a frame that is already there, or that arrives after its request gave up, is taken for the answer to the next request, and every answer after it is one behind.

Two probes, with the real driver and reader on the fake Teensy:

- **A status reply is waiting when the port opens.** `get_info()` accepts any frame of 5 bytes or more: it reads the status reply as an `Info` with no motor and the rest as the name. `connect()` logs `Unexpected device on serial port` and gives up after one query (line 84), without the 4 tries left. The hardware stays unconfigured.
- **An acknowledge is waiting when the port opens.** The first `Info` fails as too short, the second reads the reply to the first and succeeds. `hold_motors()` then reads the second `Info` reply as a status. `get_status()` (line 251) loops while a byte is left and takes 13 bytes each time, so it reads 5 motors from a reply of 56 bytes, the last 10 bytes past the end of the vector. The driver sends them as a Move: `71 01 01010500 01 41490FDB DB 4196CBE4 E4 0441490F 50 49542101`, that is motor 1 twice, the second time to 12.57 rad, the acceleration limit read as a position, and motors 219, 228 and 80. The Teensy rejects the whole command for the three ids, and stops every motor. The driver reads the pending status reply as the acknowledge, `0x11`, so `connect()` returns `true`; `on_configure()` then reads the rejection as an `Info` and fails with `Info response is too short.`

The same follows a first `Info` reply that is only late: the retry reads it, and the handshake is one behind from there.

The rejection is not designed: it comes from the bytes `0xDB` and `0xE4`, the last bytes of 4π and 6π as floats, landing where ids are read. Another pair of limits, or a reply of another length, gives other ids. That is why this is the first finding, although nothing moves today.

How a reply comes to be waiting, from reading: `ros2_control_node` killed between a request and its reply, which is about 1.6 ms of every 10 ms cycle; the Teensy's USB stack then holds the reply until a host reads again. The Teensy core is not in this repository and no board was touched, so this side is not verified. StepIt Freezer's review found the same pairing in its driver, with the same shared reader.

While the robot runs, the pairing does no harm, for a blunt reason: the first failed cycle ends all traffic.

### After a Failed Cycle

The architecture document says that after an `ERROR` from `read()` or `write()`, "`ros2_control` deactivates the hardware". Measured on `hardware_interface::System` of `ros2_control` 4.48.1, it is not so:

| | After `read()` threw, e.g. a timeout | After a rejected command |
|---|---|---|
| State of the hardware | `unconfigured` | `unconfigured` |
| `on_deactivate()` called | No | No |
| `on_error()` called | No | Yes |
| Commands sent to stop | 0 | 0 |
| `Driver::disconnect()` called | 0 | 0 |
| Status queries in the 10 cycles after | 0 | 0 |

`HardwareComponent::read()` calls `error()` on an `ERROR`, which calls `on_error()` and moves the component to `unconfigured`, or to `finalized` if `on_error()` fails. It never calls `on_deactivate()`. The controller manager then deactivates the controllers of the hardware, in the realtime loop.

Three consequences:

- **The catch blocks hide the error from `on_error()`.** `read()`, `write()` and `on_configure()` set the state to `unconfigured` themselves before returning `ERROR` (lines 252, 414 and 499). `error()` skips `on_error()` for a component that is already unconfigured, so a lost link never reaches it, while a rejected command does. An `on_error()` written tomorrow to stop the motors or close the port would run in one case and not the other.
- **The motors are stopped by the firmware alone.** No stop is sent and no query follows, so the watchdog fires 1 s later. A motor on a velocity command turns for that second: at the limits, 3 turns, then 2.25 turns of braking. A motor on a position goal ends its move. For a rejected command the firmware has already stopped them.
- **Only a restart recovers.** Configuring the hardware again, e.g. `ros2 control set_hardware_component_state motors inactive`, calls `connect()` a second time with no `disconnect()` between; verified. `serial::Serial::open()` throws `Serial port already open.` on an open port (`unix.cc` line 135), from reading, so the second `on_configure()` fails. The document's "It stays disconnected until `ros2_control_node` starts again" is true, for this reason. One reply that takes more than 0.2 s costs the rig a restart.

### Which Thread Switches the Controllers

The architecture document says a switch requested through a service runs on that service's thread while `write()` may be running. Measured with `robot.launch.py` on the fake motors, with the thread id logged in `write()` and in `perform_command_mode_switch()`:

| Request | Thread of `perform_command_mode_switch()` |
|---|---|
| `write()`, for reference | 153, the realtime loop |
| The spawners at start | 153 |
| `switch_controller` with `activate_asap: true` | 153 |
| `switch_controller` with `activate_asap: false` | 122, with `Requested controller switch from non-realtime loop` in the log |

So the document is right for the rig, and only for it: StepIt Macro's `SwitchController` sends `activate_asap = false` (`switch_controller.cpp` line 116), and the controller manager then runs `manage_switch()` on the thread of the service, while the loop goes on reading and writing. With `activate_asap: true` the switch runs inside `update()`, between `read()` and `write()`. The atomic flags are needed, and correct. Two remarks:

- The commands that a claim resets to NaN are plain `double`, written on thread 122 while `write()` reads them on thread 153: a data race by the letter of the language, harmless on the processors of the rig, as [Threads](ARCHITECTURE.md#threads) admits.
- When the switch does run in the realtime loop, `perform_command_mode_switch()` builds two `std::string` per interface there.

## The Firmware

From reading, except the conversions.

**A Move truncates its target.** `moveCommand()` stores `long positions[i] = radiansToSteps(...)` (line 516): the conversion from `float` to `long` drops the fraction. A position crosses the link as a 32-bit float of radians, and the way back to steps lands a hair under the whole number for many positions, so the target is one step short, towards zero. Compiled on the host side over the positions from -200,000 to 200,000 steps:

| Case | Positions that end one step short |
|---|---|
| The host asks for exactly N steps, as the float of N × 2π / 3200 | 674 of 400,001 |
| The host sends back the position the status reported | 50,194 of 400,001, one in eight; steps 1 and 2 among them |
| The same sent back 100 times | up to 7 steps of creep |
| With `lround()` instead of the truncation | 0 |

Sending back the reported position is not rare. `hold_motors()` does it at every connection: with the fake Teensy, 36 of 200 motors moved one step by connecting. A trajectory controller does it when it is activated and holds. And StepIt Macro's `CommandJointPositions` sends every other joint of the controller the position it reads on `/joint_states`, at every move: the axis that is not moved can lose a step per move, up to 7 over a stack, then it settles. Its arrival tolerance, 0.01 rad or 5 steps, hides it. The probe takes `PI` as a `double`, as the Teensy core defines it; with a single precision `PI` the numbers are 44,314 and 4 steps.

**A Move or a Speed is not checked for a number.** Config rejects a NaN; Move and Speed check only the length and the ids. A NaN speed holds the motor, by the way the comparisons fall. A NaN position is converted to `long`, which C++ leaves undefined; on the Teensy's processor it gives 0, so the motor goes to step 0. An infinite position, or any beyond 2³¹ steps, becomes `LONG_MAX`, the value the comment above line 463 says AccelStepper does not handle. See finding 5.

**Config writes the steppers while the interrupt runs them.** `configureCommand()` calls `setAcceleration()` and `setMaxSpeed()` from the main loop (lines 406 and 407). The `writingMotorGoals` flag keeps the interrupt from taking goals, not from calling `AccelStepper::run()`, and `setAcceleration()` updates three fields that `run()` reads. Harmless on a motor at rest, which is the usual case at configuration; a motor still turning, after a host that reconnects within a second, can take one wrong step interval.

**The watchdog stops the motors on every pass of `loop()` while it is timed out.** Judged harmless: it writes a speed of 0 that is already 0, and a goal sent later raises it. Once would do.

**Small points.** `DataBuffer::removeInt()` fills 2 of the 4 bytes of an `int` and returns it; its one caller drops the value. `sgn()` of `main.cpp`, `SerialPort::flush()` and `isBusyWriting()` are called nowhere. `Set motors enabled` with an empty payload reads a 0 and stops the interrupt. An `Echo` long enough to pass 200 bytes once escaped is cut without a sign by `Buffer::add()`. `MotorGoal` and `MotorState` are shared with the interrupt through plain members; only the two flags are `volatile`, which holds as long as their accessors stay in another file.

## The Fake Motors

`FakeDriver` enforces the limits and validates a whole configuration as the firmware does, and the closed form of `FakeMotor` is a good idea: a test moves the motors by passing a later time. But it departs from the board in five ways, each verified with a probe:

| What | The board | The fake |
|---|---|---|
| A robot of 3 joints | `on_configure()` fails: the status reports 5 motors | `on_configure()` succeeds |
| A Move naming an unknown motor after a known one | Nothing applied, every motor stopped | The known motor takes the goal, and a motor that was turning keeps turning |
| The hardware is deactivated while a joint turns | The joint decelerates and stays | The joint jumps: from 1.83 rad to -1000 rad in the probe |
| The hardware is configured again | The motors keep their positions | Every motor is back at 0 |
| The firmware version | 1.1.1 | 1.1.0 |

The architecture document names the first two, and that the distance to go is always 0. The third comes from `on_deactivate()` stamping its stop with `rclcpp::Time{}` (line 309): `FakeMotor::set_target_velocity()` takes it for the time 0, computes where the motor was at a time far before its last goal, and starts again from there. The fourth is `motors_.clear()` in `FakeDriver::configure()` (line 72).

## Coupling and Duplication

- **The firmware's protocol layer is copied in StepIt Freezer.** 8 files. After the licence header, 4 are identical; `DataBuffer` takes an `int16_t` there and an `int` here, and `SerialPort` has a `reset()` there that this one lacks, while `loop()` of `main.cpp` here carries `// TODO: clean input and output buffers` and a commented call to it. The Freezer fixed two things in its copy that this one still has. Weight: medium.
- **`framed-serial` and `serial` are shared, not copied**, at the same commits in `stepit.repos` here, in `freezer.repos` and in the rig's `stepit.repos` (`d386c91` and `d8d1606`). The reader does not find a frame again once it is out of step, as the Freezer's review shows; here the first failure ends the session, so it does not matter until finding 2 is fixed.
- **The limits are written in four places**: `MAX_ACCELERATION` and `MAX_SPEED` of `main.cpp`, `hardware_limits.hpp`, the 10 joint parameters of `stepit.ros2_control.xacro`, and, in the rig, the `scale: 18.8496` of `ui_teleop` in `rig.yaml`, "the motors' limit, 3 turns/s". `get_info` checks the first two against each other on the board; the firmware clamps a speed, so a stale `rig.yaml` only slows the slider. Weight: low.
- **The steps per turn are written in two repositories**: `STEPS_IN_ONE_ROTATION` here, and the 0.01 rad "5 motor steps" of `controllers.yaml` and of `CommandJointPositions`. Weight: low.
- **`rig.yaml` sets `baud_rate: 9600`**, an argument that changes nothing: finding 8.
- **The command ids are written on both sides** by hand, as the major version is; the handshake covers a change of layout, not a new id. Expected of a protocol without a shared header.

## Testability and Tests

| What | Tested | How |
|---|---|---|
| The frames of each command, the handshake | Yes, 11 tests | `test_default_driver`, on a mock of `FramedSerial`: whole frames, each the answer to its request. |
| The fake driver and the fake motor | Yes, 7 and 3 tests | The limits and one rejection; 3 tests for 345 lines of kinematics, none with a time that goes back. |
| The hardware interface | Yes, 36 tests | On a mock of `Driver`, calling `on_init()`, `on_configure()`, `read()` and `write()` directly. |
| The hardware under `ros2_control`'s own wrapper | Once | `load_urdf` loads the plugin through the `ResourceManager` and stops there. What `ros2_control` does after an `ERROR`, and a second `on_configure()`, are not tested: finding 2 lives there. |
| A byte stream with a reply late, stale or cut | No | Finding 1 has no test that could see it. |
| `FakeDriver` behind `StepitHardware` | No | As [Tests](ARCHITECTURE.md#tests) says; the jump at deactivation lives there. |
| The factories, the launch file, the xacro | No | `baud_rate` against `baudrate` would have failed the first test of `DefaultSerialFactory`. |
| The firmware | No | By design. Its conversions and its checks are pure and could run on the host. |

The missing seams: a fake `framed_serial::Serial` that plays the Teensy byte by byte under the real reader and driver, which the probes of finding 1 are, about 130 lines; the firmware's parsing, checks and conversions in a file with no Arduino include; and a `FakeDriver` that can be told to lose or reject a reply.

One trap in the test helpers: `FakeHardwareInfo` does not set `is_async`, which `HardwareInfo` leaves uninitialised. The 36 tests call `on_init()` directly and never read it. A test that wraps the hardware in `hardware_interface::System` gets an asynchronous component or not by chance: the first lifecycle probe did, and its `read()` returned `OK` for a cycle that had failed.

## Implementation Issues

Ordered by severity. **Safety** says whether the issue can move a motor that was not commanded, or leave one turning.

1. **A reply left on the port puts the handshake one reply behind, and `get_status()` reads past its response.** Verified. See [The Handshake](#the-handshake). Two probes on a byte-level fake Teensy: a stale status reply makes `connect()` refuse the controller after one query; a stale acknowledge makes it send a Move built from the `Info` reply, 10 bytes of it from beyond the vector, and return `true` with the Teensy's rejection unread. `get_status()` checks no length: a probe with 19 bytes got 2 motors and no exception. `get_info()` accepts a reply of another command. The stale reply itself, kept by the Teensy across a reopening, is from reading. **Safety: no today**: the Teensy rejects the Move and stops the motors, for the ids `0xDB` and `0xE4` that the limits' bytes happen to give. With other limits the same Move could be accepted, and it sends motor 1 to 12.57 rad.
2. **One failed cycle ends the session, sends no stop, and leaves the port open.** Verified. See [After a Failed Cycle](#after-a-failed-cycle). `Driver::disconnect()` is called by nothing, there is no `on_cleanup()`, `on_shutdown()` or `on_error()`, and the catch blocks keep `ros2_control` from calling `on_error()` for a lost link. A second `on_configure()` calls `connect()` on the open port. **Safety: no**: the firmware's watchdog stops the motors, up to 1 s and 5.25 turns later at the limits.
3. **A Move truncates its target, so a position sent back moves the motor.** Verified by compiling the firmware's conversions on the host side, and with the fake Teensy. See [The Firmware](#the-firmware). One position in eight loses a step when echoed, up to 7 steps over repeated echoes; `hold_motors()` and the rig's `CommandJointPositions` both echo. **Safety: no.** It is an accuracy issue for a focus stack: the axis that is not moved drifts by microsteps.
4. **The fake motors differ from the board.** Verified, 5 probes. See [The Fake Motors](#the-fake-motors). The jump at deactivation also shows on `/joint_states` of a simulated rig whose hardware is deactivated by hand. **Safety: no**, but a test on the fake cannot see a rejection stop the motors.
5. **Nothing refuses a position that is not finite or is out of range.** The host half is verified: `write()` drops a NaN command and forwards an infinite one, sent as `inf`. The firmware half is from reading: see [The Firmware](#the-firmware). `hold_motors()` cannot produce one; a client that publishes such a value on `/position_controller/commands` can. **Safety: yes, in principle**: it is the one input that moves a motor to a place nobody chose, step 0 or the end of the range.
6. **Two packages name the wrong dependencies.** Verified with colcon. `colcon list --packages-up-to robot_bringup` gives `robot_bringup` alone, and launching then fails with `package 'robot_description' not found`. `--packages-up-to robot_description` gives `serial`, `framed_serial`, `stepit_driver` and no `stepit_hardware`, the package of the plugin its xacro loads. The rig and CI build every package, so neither meets it. `stepit_driver` declares `rclcpp_lifecycle` and `lifecycle_msgs`, and `stepit_hardware_tests` declares `launch`, `launch_ros`, `ros2_control` and `ros2_controllers`, none of which they use. **Safety: no.**
7. **The board programs.** Verified without a port. `get_status --timeout 0.5` prints `read/write timeout: 0s`: both programs parse the timeout with `std::stoi()` (line 67 of `get_info.cpp`), so every value under 1 s is no timeout at all, and the connection fails. `ros2 run stepit_hardware_tests get_status` answers `No executable found`: `CMakeLists.txt` builds the two programs and installs neither. From reading: the help of `get_status` names `./set_relative_pressure`; both catch only `serial::IOException`, so a timeout after the handshake ends the program with an uncaught exception; and both connect as the driver does, so they send a Move, with finding 3. **Safety: no**, beyond that Move.
8. **The launch argument `baud_rate` is dead.** From reading. The xacro writes `<param name="baud_rate">`, `DefaultSerialFactory` reads `baudrate` (line 34): the port always opens at 9600. The architecture document says so already; `rig.yaml`, the README's table and the launch file's description still present it as a setting. Harmless on USB. **Safety: no.**
9. **The realtime loop allocates and can block for 0.2 s a byte.** From reading. A cycle builds about ten vectors, in `get_status()`, `StatusResponse`, `motor_states()` and the two lists of `write()`, and logs with `RCLCPP_ERROR` on a failure; `framed_serial` writes a byte and waits for the port to drain, 5 times for a status query and 30 or more for a Move of 5 motors. A reply that does not come holds the loop 0.2 s, 20 cycles, before the `ERROR`. The measured 2.7 ms at most leaves room, and blocking on the board is the design; the allocations are not needed. `set_status()`, suspected of taking its mutex every cycle, is only called in a cycle that fails. **Safety: no.**
10. **`on_init()` fails without a word, and wraps an id.** From reading. Its catch (line 170) returns `ERROR` and logs nothing, so a joint with no `id` or a `max_velocity` that is not a number gives no message, although the document says such a joint is rejected. `static_cast<uint8_t>(std::stoi(...))` turns an id of 256 into 0. `on_configure()` takes any answer to `configure()` that is not `0x12` for a success. **Safety: no**: the firmware checks every id.
11. **Config writes the steppers outside the interrupt's guard.** From reading. See [The Firmware](#the-firmware). **Safety: no**: one wrong step interval on a motor that turns during a configuration.
12. **Documents and comments that say another thing than the code.** From reading, and measured where it says so. The module's `CLAUDE.md` says C++20; the three `CMakeLists.txt` set 23. [Read and Write](ARCHITECTURE.md#read-and-write) and the fourth row of [Safety Nets](ARCHITECTURE.md#safety-nets) say `ros2_control` deactivates the hardware after an `ERROR`; it moves it to `unconfigured` and never calls `on_deactivate()`, measured. [The Lifecycle](ARCHITECTURE.md#the-lifecycle) says `on_init` rejects a joint that lacks a parameter; it does, silently. [Tests](ARCHITECTURE.md#tests) says the firmware is checked with `get_info` and `get_status`; they must be run from `build/`. The comment of `motor_states_are_valid()`, "this runs without allocating on the read cycle", sits two calls after three copies of a vector. `main.cpp` line 40 computes "= 200" for 3200 steps. **Safety: no.**

No security issue was found: the module opens only the configured port, and offers no service of its own.

## Recommendations

Ordered by value against effort; each fits one pull request.

1. **Check every reply, and start each session clean** (finding 1). In `DefaultDriver`, the exact length of each reply: 1 byte for an acknowledge, 1 + 13 × n for a status, and an `Info` whose length matches its motor count and whose name follows; anything else throws. In `connect()`, drop what is waiting before the first `Info` and before each retry: a `flush_input()` on `framed_serial::Serial`, over `serial::Serial::flushInput()`. Add the byte-level fake Teensy of the probes to the tests, with a stale and a late reply. Trade-off: `flush_input()` changes the shared library, so StepIt Freezer, whose review asks for the same call, and the rig's `stepit.repos` move to the same commit. The length checks need nothing shared and can go first.
2. **Round the target of a Move** (finding 3). `lround(radiansToSteps(...))` in `moveCommand()`: 0 of 400,001 positions then move. Firmware 1.1.2, no change of protocol. Trade-off: one line to flash and check on the board. A host-only fix does not exist: the host cannot know which floats the firmware truncates.
3. **Refuse what is not a position** (finding 5). In the firmware, reject a Move or a Speed whose value is not finite or whose target is beyond a stated range, with the stop that a rejection already gives; in `write()`, test `std::isfinite()` where it tests `std::isnan()`. Trade-off: goes with recommendation 2 in the same flash; a range must be chosen, e.g. the 2²⁴ steps a float counts exactly.
4. **Close the session when it fails** (finding 2). Add `on_error()` and `on_cleanup()` that try a velocity of 0, ignore its failure, and call `disconnect()`; stop setting the lifecycle state in the catch blocks, so that `ros2_control` calls `on_error()` for a lost link too. A second `on_configure()` then opens a closed port, and the rig recovers from a lost reply with one service call instead of a restart. Trade-off: recommendation 1 must come first, or the reconnection meets the stale reply of the session that failed; the status tests that expect `unconfigured` after an exception need a look.
5. **Make the fake fail like the board** (finding 4). Report `kMotorCount` motors in the status, apply a motion command only once every id is checked and stop the motors on a rejection, keep the positions across `configure()`, report 1.1.1, and give `on_deactivate()` the time of the last cycle instead of `rclcpp::Time{}`, or let `FakeMotor` refuse a time before its last goal. Add a test of `FakeDriver` behind `StepitHardware`. Trade-off: a robot of fewer than 5 joints then fails in simulation as on the board, which is the point; whether such a robot should work at all is a separate decision, for the firmware.
6. **Name the dependencies** (finding 6). `stepit_hardware` as an `exec_depend` of `robot_description`, `robot_description` as one of `robot_bringup`, and drop the unused ones. Trade-off: none.
7. **Remove or wire `baud_rate`** (finding 8). Remove it from the launch file, the xacro, the README and `rig.yaml`: the firmware is fixed at 9600 and USB ignores it. Trade-off: `rig.launch.py` refuses an argument a module does not declare, so `rig.yaml` changes in the same step as the pin in `stepit.repos`.
8. **Take the firmware's pure parts out of `main.cpp`** (testability, findings 3, 5 and 11). The conversions, the parsing and the checks of the three commands in a header with no Arduino include, compiled by the firmware and by a gmock test of `stepit_driver`. Share the protocol layer with StepIt Freezer, or at least take its `int16_t` and its `reset()`. Trade-off: the firmware's first host-side test needs a small build rule; sharing files across two repositories needs a third place for them.
9. **Smaller fixes in one pass** (findings 7, 9, 10, 11, 12). `std::stod()` for `--timeout`, an `install()` rule and a corrected help for the two programs; a log in the catch of `on_init()` and a range check of the id; `!= Success` in `on_configure()`; `is_async = false` in `FakeHardwareInfo`; the steppers' configuration with interrupts off; reserved vectors, or none, in `read()` and `write()`; C++23 in `CLAUDE.md`; and the sentences of the architecture document listed in finding 12. Trade-off: none worth a design decision.
