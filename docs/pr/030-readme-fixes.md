# The README Files, Checked Against the Code

Pull request [#30](https://github.com/kineticsystem/stepit-motors/pull/30), branch `readme-fixes`.

## Goal

Every statement of the README files read against the repository: commands, package and file names, launch arguments, pins, the examples. What was wrong is corrected.

This pull request changes documents and one comment of `MotorsStatus.msg`. No code, no launch file and no configuration changes.

## Decisions

- **A README says what the code does today.** The launch argument `baud_rate` never reaches the serial port: the table says so and points to finding 8 of `docs/Review.md`. The fix of the argument itself is a pull request of its own.
- **After a failed read or write, the hardware is `unconfigured`, not deactivated**, as the architecture document already says. The README and the comment of `MotorsStatus.msg` now agree with it.
- **The long trajectory example keeps its positions and takes 10 s for each move.** Before, `joint5` had to turn 56.55 rad in 2 s, a mean of 28.3 rad/s, above the limit of 18.85 rad/s. With 10 s and a velocity of 0 at each waypoint, the fastest joint peaks at 1.5 × 56.55 / 10 = 8.5 rad/s, 45 % of the limit, and at 6 × 56.55 / 100 = 3.4 rad/s², 27 % of the limit of 12.57 rad/s².
- **The pins are 0 to 11 and 13 to 15.** The firmware does not drive pin 12.
- **The README of `robot_description` names the real packages**, `robot_description` and `robot_bringup`, and says that `robot.launch.py` itself loads the controllers.
- **The xacro example of `src/robot_description/docs/README.md` follows `robot.launch.py`**, and the graph is made with `urdf_to_graphviz`, a program of its own, not an argument of `xacro`.
- **The heading "Chekout the Git Repository" becomes "Check out the Git Repository"**, with its anchor. Nothing else in the repository, or in StepIt Macro, links to the old anchor.

## Left Out

- The PlatformIO boilerplate files `README` of `src/stepit_mcu/lib`, `include` and `test`.
- The statements on the hardware that the repository cannot prove, e.g. the heatsinks and the loader's messages.
- "Pins 0 to 15" in `docs/ARCHITECTURE.md`: there it names the pins a board must have, which is true.

## Open Items

- `get_info` and `get_status` of `stepit_hardware_tests` are built but not installed, so `ros2 run` does not find them: finding 7 of the review. No README names them.
- `check_urdf` and `urdf_to_graphviz` come from `liburdfdom-tools`, which no `package.xml` and not the `Dockerfile` installs.
- The last sentence of `src/robot_description/meshes/docs/README.md`, "not in the texture as required by Gazebo", lacks a word; which folder Gazebo requires is not checked.

## Tested

Nothing is built or run: the commands and the names are read against the scripts, the launch files and the `CMakeLists.txt`; the trajectory by arithmetic; the relative links and anchors by a script.
