#!/usr/bin/python3

# Copyright 2023 Giovanni Remigi
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import launch
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    OpaqueFunction,
    RegisterEventHandler,
)
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.substitutions import (
    LaunchConfiguration,
    PathJoinSubstitution,
)

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

import yaml
import xacro


def launch_setup(context, *_args, **_kwargs):
    # Declare all parameters.
    # Read now, not when RViz starts: RViz waits for an event, which comes after
    # the launch file that includes this one may have closed its scope, e.g.
    # StepIt Macro, which runs every module in a scoped group.
    launch_rviz = LaunchConfiguration("launch_rviz").perform(context)

    # Extract all parameters' values.
    description_pkg = FindPackageShare("robot_description")
    description_file = PathJoinSubstitution(
        [description_pkg, "urdf", "stepit.urdf.xacro"]
    ).perform(context)
    rviz_config_file = PathJoinSubstitution(
        [description_pkg, "rviz", "robot.rviz"]
    ).perform(context)
    controllers_config_file = PathJoinSubstitution(
        [
            description_pkg,
            "config",
            "controllers.yaml",
        ]
    ).perform(context)

    # The hardware, as the xacro arguments of the same names.
    hardware = {
        name: LaunchConfiguration(name).perform(context)
        for name in ("use_dummy", "usb_port", "baud_rate")
    }
    robot_description_content = xacro.process_file(
        description_file, mappings=hardware
    ).toxml()

    with open(controllers_config_file, "r") as f:
        controllers_config = yaml.safe_load(f)

    # NOTE: ros2_control 4.42.2 bug workaround.
    # When ros2_control_node spawns a controller it passes its own --params-file
    # arguments down to the controller sub-node. File paths are silently lost in
    # that propagation (the path becomes empty), which causes every controller to
    # crash on load:
    #   "Couldn't parse trailing --params-file flag. No file path provided."
    # Only parameters passed as Python dicts (written to a temp file by the
    # launch system) survive the propagation correctly.
    # Fix: pass the controller_manager section of the YAML as a dict, and
    # supply the controller-specific parameters (joints, etc.) to each spawner
    # via --param-file, which uses the parameter service and is unaffected.
    # TODO: revert to passing controllers_config_file directly once fixed.
    controller_manager_params = controllers_config.get("controller_manager", {}).get(
        "ros__parameters", {}
    )

    # The Controller Manager (CM) connects the controllers' and
    # hardware-abstraction sides of the ros2_control framework. It also serves
    # as the entry-point for users through ROS services.
    # https://control.ros.org/master/doc/getting_started/getting_started.html#architecture
    controller_manager = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[
            {"robot_description": robot_description_content},
            controller_manager_params,
        ],
        output={
            "stdout": "screen",
            "stderr": "screen",
        },
    )

    # The broadcaster reads all state interfaces and reports them on
    # /joint_states and /dynamic_joint_states.
    # https://control.ros.org/master/doc/ros2_controllers/joint_state_broadcaster/doc/userdoc.html
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
            "--param-file",
            controllers_config_file,
        ],
    )

    # This is a controller that work using the "velocity" joint command
    # interface.
    # https://control.ros.org/master/doc/ros2_controllers/velocity_controllers/doc/userdoc.html
    velocity_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "velocity_controller",
            "-c",
            "/controller_manager",
            "--param-file",
            controllers_config_file,
            "--inactive",
        ],
    )

    # This is a controller that work using the "position" joint command
    # interface.
    # https://control.ros.org/master/doc/ros2_controllers/position_controllers/doc/userdoc.html
    position_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "position_controller",
            "-c",
            "/controller_manager",
            "--param-file",
            controllers_config_file,
            "--inactive",
        ],
    )

    # Controller for executing joint-space trajectories on a group of joints.
    # Trajectories are specified as a set of waypoints to be reached at
    # specific time instants, which the controller attempts to execute as well
    # as the mechanism allows. Waypoints consist of positions, and optionally
    # velocities and accelerations.
    # https://control.ros.org/master/doc/ros2_controllers/joint_trajectory_controller/doc/userdoc.html
    joint_trajectory_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_trajectory_controller",
            "-c",
            "/controller_manager",
            "--param-file",
            controllers_config_file,
        ],
    )

    # robot_state_publisher uses the URDF specified by the parameter robot
    # description and the joint positions from the topic /joint_states to
    # calculate the forward kinematics of the robot and publish the results via
    # tf.
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        parameters=[{"robot_description": robot_description_content}],
        output="screen",
    )

    # joint_state_publisher_gui reads the robot description from the topic
    # /robot/description and create a GUI displaying a slider for each moving
    # joint. Remember to set non-zero lower and upper limits on each joint,
    # otherwise the slider will not be displayed. By moving the slider we
    # publish joint states to the topic /joint_states which are used by the
    # robot_state_publisher node. A non-graphical tool to do the same is the
    # joint_state_publisher node.
    # joint_state_publisher_gui_node = Node(
    #    package="joint_state_publisher_gui",
    #    executable="joint_state_publisher_gui",
    #    name="joint_state_publisher_gui",
    # )

    # Starts up the RViz2 application.
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        arguments=["-d", rviz_config_file],
        condition=IfCondition(launch_rviz),
    )

    # Delay RViz2 start after joint_state_broadcaster_spawner.
    delay_rviz_after_joint_state_broadcaster_spawner = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[rviz_node],
        )
    )

    nodes_to_start = [
        controller_manager,
        joint_state_broadcaster_spawner,
        delay_rviz_after_joint_state_broadcaster_spawner,
        velocity_controller_spawner,
        position_controller_spawner,
        joint_trajectory_controller_spawner,
        robot_state_publisher_node,
    ]
    return nodes_to_start


def generate_launch_description():
    """
    A Python launch file is meant to help implement the markup based frontends
    like YAML and XML, and so it is declarative in nature rather than
    imperative. For this reason, it is not possible to directly access the
    content of LaunchConfiguration parameters, which are asyncio futures. To
    access the content of a LaunchConfiguration, we must provide a context by
    wrapping the initialization method into an OpaqueFunction.
    https://answers.ros.org/question/397123/how-to-access-the-runtime-value-of-a-launchconfiguration-instance-within-custom-launch-code-injected-via-an-opaquefunction-in-ros2
    https://github.com/Serafadam/interbotix_ros_manipulators/blob/xsarm_control_galactic/interbotix_ros_xsarms/interbotix_xsarm_control/launch/xsarm_control.launch.py
    """
    declared_arguments = [
        DeclareLaunchArgument(
            "launch_rviz", default_value="false", description="Launch RViz?"
        ),
        DeclareLaunchArgument(
            "use_dummy",
            default_value="true",
            description="Use fake motors instead of the microcontroller.",
        ),
        DeclareLaunchArgument(
            "usb_port",
            default_value="/dev/ttyACM0",
            description="Serial port of the microcontroller.",
        ),
        DeclareLaunchArgument(
            "baud_rate",
            default_value="9600",
            description="Speed of the serial port, the one of the firmware.",
        ),
        OpaqueFunction(function=launch_setup),
    ]

    return LaunchDescription(declared_arguments)


def main():
    """
    This is used to execute the launch file as a normal python script.
    Simply type the following in a terminal window:
    ./robot.launch.py
    """
    ls = launch.LaunchService()
    ld = generate_launch_description()
    ls.include_launch_description(ld)
    return ls.run()


if __name__ == "__main__":
    main()
