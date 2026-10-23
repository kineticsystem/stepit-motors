// Copyright 2023 Giovanni Remigi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <stepit_driver/driver.hpp>
#include <stepit_driver/driver_factory.hpp>
#include <stepit_hardware/visibility_control.hpp>

#include <hardware_interface/handle.hpp>
#include <hardware_interface/hardware_info.hpp>
#include <hardware_interface/system_interface.hpp>
#include <hardware_interface/types/hardware_component_interface_params.hpp>
#include <hardware_interface/types/hardware_interface_return_values.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <rclcpp/macros.hpp>
#include <rclcpp/rclcpp.hpp>
#include <stepit_motors_msgs/msg/motors_status.hpp>

namespace stepit_driver
{
/**
 * https://control.ros.org/master/doc/ros2_control/hardware_interface/doc/writing_new_hardware_interface.html
 */
class StepitHardware : public hardware_interface::SystemInterface
{
public:
  /**
   * Default constructor.
   */
  StepitHardware();

  /**
   * Constructor with driver factory. This method is used for testing.
   * @param driver_factory The interface to send commands and queries to
   * the hardware.
   */
  explicit StepitHardware(std::unique_ptr<DriverFactory> driver_factory);

  /**
   * Defines aliases and static functions for using the Class with shared_ptrs.
   * With such definitions, the control manager can instantiate the class with
   * auto hardware_interface = StepitHardware::make_shared();
   */
  RCLCPP_SHARED_PTR_DEFINITIONS(StepitHardware)

  /**
   * Initialization of the hardware interface from data parsed from the
   * robot's URDF.
   * @param hardware_info Structure with data from URDF.
   * @returns CallbackReturn::SUCCESS if required data are provided and can be
   * parsed or CallbackReturn::ERROR if any error happens or data are missing.
   */
  STEPIT_HARDWARE_PUBLIC CallbackReturn
  on_init(const hardware_interface::HardwareComponentInterfaceParams& params) override;

  /**
   * Connect to the hardware.
   * @param previous_state The previous state.
   * @returns CallbackReturn::SUCCESS if required data are provided and can be
   * parsed or CallbackReturn::ERROR if any error happens or data are missing.
   */
  STEPIT_HARDWARE_PUBLIC CallbackReturn on_configure(const rclcpp_lifecycle::State& previous_state) override;

  /**
   * This method exposes position and velocity of joints for reading.
   */
  STEPIT_HARDWARE_PUBLIC std::vector<hardware_interface::StateInterface::ConstSharedPtr>
  on_export_state_interfaces() override;

  /**
   * This method exposes the joints targets for writing.
   */
  STEPIT_HARDWARE_PUBLIC std::vector<hardware_interface::CommandInterface::SharedPtr>
  on_export_command_interfaces() override;

  /**
   * This method is invoked when the hardware is connected.
   * @param previous_state Unconfigured, Inactive, Active or Finalized.
   * @returns CallbackReturn::SUCCESS or CallbackReturn::ERROR.
   */
  STEPIT_HARDWARE_PUBLIC CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;

  /**
   * This method is invoked when the hardware is deactivated. It brings every
   * motor to rest: the firmware keeps executing the last goal it received,
   * and its watchdog does not fire while read() keeps polling the status.
   * @param previous_state Unconfigured, Inactive, Active or Finalized.
   * @returns CallbackReturn::SUCCESS or CallbackReturn::ERROR.
   */
  STEPIT_HARDWARE_PUBLIC CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;

  /**
   * Read data from the hardware.
   */
  STEPIT_HARDWARE_PUBLIC hardware_interface::return_type read(const rclcpp::Time& time,
                                                              const rclcpp::Duration& period) override;

  /**
   * Write data from to hardware.
   */
  STEPIT_HARDWARE_PUBLIC hardware_interface::return_type write(const rclcpp::Time& time,
                                                               const rclcpp::Duration& period) override;

  /**
   * Called when the set of claimed command interfaces changes (e.g. a
   * controller activates, deactivates, or is switched out for another).
   * Updates each joint's position_claimed/velocity_claimed flags so write()
   * knows which command interfaces are currently owned by an active
   * controller. This is what lets write() stop honoring a joint's command
   * once its owning controller has released the interface, even though its
   * last value is left sitting in joint.command.
   * A joint whose interface is released is also marked for a stop, which
   * the next write() sends unless another controller is commanding it.
   * A newly claimed interface has its command reset to NaN, so a controller
   * never starts from the value a previous one left behind.
   * @param start_interfaces Names of the command interfaces newly claimed.
   * @param stop_interfaces Names of the command interfaces being released.
   * @returns hardware_interface::return_type::OK.
   */
  STEPIT_HARDWARE_PUBLIC hardware_interface::return_type
  perform_command_mode_switch(const std::vector<std::string>& start_interfaces,
                              const std::vector<std::string>& stop_interfaces) override;

private:
  // Internal structure to store joint states or targets.
  struct JointValue
  {
    double position = 0.0;
    double velocity = 0.0;
  };

  // Internal structure to store joint states and targets.
  struct Joint
  {
    uint8_t id = 0;
    double acceleration = 0.0;
    double max_velocity = 0.0;
    JointValue state{};
    JointValue command{};

    // Whether the position/velocity command interface is currently claimed
    // by an active controller, set only by perform_command_mode_switch.
    // write() requires this (in addition to the command value not being
    // NaN) before honoring a joint's command, so a controller that has
    // released an interface is ignored even though its last written value is
    // still sitting in `command` unchanged.
    //
    // These flags are atomic because the controller manager runs a switch
    // requested from a service on that service's thread, while the realtime
    // loop may be inside write().
    std::atomic<bool> position_claimed{ false };
    std::atomic<bool> velocity_claimed{ false };

    // Set by perform_command_mode_switch when a controller releases one of
    // this joint's command interfaces. The firmware keeps executing the last
    // goal it received, so write() sends a zero velocity for the joint unless
    // another controller is already commanding it. write() takes the flag
    // with an exchange, so a release arriving while it runs is kept for the
    // next cycle rather than cleared unseen.
    std::atomic<bool> stop_pending{ false };
  };

  // Store information about current joint states and targets.
  std::vector<Joint> joints_;

  // Maps a configured motor id to the index of the matching entry in joints_.
  // Built in on_init(); read() uses it to route each reported motor state to
  // the right joint instead of trusting the reported id as a vector index.
  std::unordered_map<uint8_t, std::size_t> joint_index_by_id_;

  // True when every joint declares an acceleration and a maximum velocity
  // within the limits the controller reports for its motor. Logs the first
  // joint that exceeds them.
  bool joints_are_within_limits(const std::vector<MotorLimits>& limits) const;

  // True when motor_states reports exactly the configured set of joint ids,
  // each exactly once. A matching count alone cannot guarantee this: an
  // out-of-range id or a duplicate id would keep the count correct while
  // indexing out of bounds or leaving another joint stale. Logs the first
  // problem found. Tracks the joints already reported in a bitmask, hence the
  // limit on the number of joints checked in on_init().
  bool motor_states_are_valid(const std::vector<MotorState>& motor_states) const;

  // The status of the connection to the controller, published on ~/status of
  // the node ros2_control gives the hardware, e.g. /motors/status: latched,
  // when it changes and every second. Without that node, e.g. in a test that
  // passes no executor, nothing is published.
  //
  // set_status() records a change, also from read() and write(): the realtime
  // loop never publishes, the status timer does, on the node's executor.
  void set_status(bool connected, const std::string& message);
  void publish_status_if_due();
  void publish_status();

  rclcpp::Publisher<stepit_motors_msgs::msg::MotorsStatus>::SharedPtr status_publisher_;
  rclcpp::TimerBase::SharedPtr status_timer_;
  std::chrono::steady_clock::time_point last_status_publish_{};

  // The serial port of the controller, or "fake" for the fake driver.
  std::string device_;

  // Guards connected_ and status_message_, written from the realtime loop and
  // read by the status timer.
  std::mutex status_mutex_;
  bool connected_ = false;
  std::string status_message_;

  // Raised by set_status() on a change, cleared by publish_status().
  std::atomic<bool> status_changed_{ false };

  // Interface to send binary data to the hardware using the serial port.
  std::unique_ptr<Driver> driver_;

  // Factory to create the driver during the initialization step.
  std::unique_ptr<DriverFactory> driver_factory_;
};
}  // namespace stepit_driver
