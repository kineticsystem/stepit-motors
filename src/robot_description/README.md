# Robot Description

This package configures a sample robot that uses StepIt Motors. The robot is displayed in RViz when the launch argument `launch_rviz` is `true`.

To activate the robot execute the following command:

```
ros2 launch robot_description robot.launch.py
```

By default, the robot runs on fake hardware. To drive the microcontroller, set the launch arguments `use_dummy` and `usb_port`:

```
ros2 launch robot_description robot.launch.py use_dummy:=false usb_port:=/dev/ttyACM0
```

To make the robot move, we need to connect the hardware to a controller: `robot.launch.py` starts the controller manager and loads the controllers. The package `robot_bringup` includes this launch file; how to send commands to the controllers is in [Running the Application](../../README.md#running-the-application) of the README.
