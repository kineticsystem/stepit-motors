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

#include <stepit_driver/msgs/acknowledge_response.hpp>
#include <stepit_driver/msgs/config_command.hpp>
#include <stepit_driver/msgs/info_response.hpp>
#include <stepit_driver/msgs/position_command.hpp>
#include <stepit_driver/msgs/status_response.hpp>
#include <stepit_driver/msgs/velocity_command.hpp>

#include <rclcpp/time.hpp>

namespace stepit_driver
{
/**
 * @brief This class receives commands and queries from the
 * hardware interface and sends them to a fake or a real hardware.
 */
class Driver
{
public:
  virtual ~Driver() = default;

  /**
   * @brief Initialize the command interface.
   */
  virtual bool connect() = 0;

  /**
   * @brief Disconnect the interface..
   */
  virtual void disconnect() = 0;

  /**
   * @brief Configure the hardware.
   * @param command A command with all required configuration.
   * @return An aknowledgment response, success or failure.
   */
  virtual AcknowledgeResponse configure(const ConfigCommand& command) const = 0;

  /**
   * @brief Send motor target positions (rad) to the hardware.
   * @param time The time the command is sent.
   * @param command A command with all positions (rad).
   * @return An aknowledgment response, success or failure.
   */
  virtual AcknowledgeResponse set_position(const rclcpp::Time& time, const PositionCommand& command) const = 0;

  /**
   * @brief Send motor target velocities (rad/s) to the hardware.
   * @param time The time the command is sent.
   * @param command A command with all motor velocities (rad/s).
   * @return An aknowledgment response, success or failure.
   */
  virtual AcknowledgeResponse set_velocity(const rclcpp::Time& time, const VelocityCommand& command) const = 0;

  /**
   * @brief Request the status of each motor connected to the hardware.
   * @param time The time the command is sent.
   * @return Each motor status and a request status, success or failure.
   */
  virtual StatusResponse get_status(const rclcpp::Time& time) const = 0;

  /**
   * @brief Request information about the software installed in the
   * micro-controller.
   * @param time The time the command is sent.
   * @return Information about the installed software.
   */
  virtual InfoResponse get_info(const rclcpp::Time& time) const = 0;
};
}  // namespace stepit_driver
