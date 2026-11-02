# Inverted Joints

Pull request [#26](https://github.com/kineticsystem/stepit-motors/pull/26), branch `inverted-joints`.

## Goal

On the rig of StepIt Macro, both motors turn the other way since their cables were connected again. A robot needs a way to turn a motor round without touching the firmware or the wires.

## Decisions

**A launch argument, `inverted_joints`.** It names the joints to turn round, separated by spaces, e.g. `joint1 joint2`. It is empty by default, so nothing changes for a robot that does not set it. StepIt Macro sets it in `rig.yaml`, as it sets the serial port.

**A parameter of each joint, `inverted`.** xacro writes it into each `<joint>` of the `ros2_control` block, `True` or `False`. `StepitHardware` reads it in `on_init()`; a joint without it is not inverted.

**The sign changes on the host, both ways.** `write()` sends an inverted joint's position and velocity negated, and `read()` negates what the motor reports. The joint, the controllers and the objectives see one direction, and the firmware's count of steps stays its own.

**A wrong name is an error.** `robot.launch.py` refuses a name that is not a joint of the robot. Ignored, a typo would leave the motor turning the wrong way.

## Left Out

**The firmware.** AccelStepper can invert the direction pin, but every robot would then need its own build, flashed again for each change.

**Swapping the wires of one coil.** It needs no software, but the box must be opened, and the next person to wire it would undo it.

## Tested

- Three tests of `StepitHardware`: an inverted joint reads negated states and writes negated positions and velocities, and a joint set to `False` is left as it is. All 36 tests pass.
- xacro writes `True` for the named joints and `False` for the others, and `robot.launch.py` stops on `inverted_joints:="joint1 jiont2"`.
