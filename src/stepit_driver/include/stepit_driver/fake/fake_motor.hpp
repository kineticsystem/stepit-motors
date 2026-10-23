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

#include <rclcpp/rclcpp.hpp>

namespace stepit_driver
{
/**
 * @brief The FakeMotor class simulates the kinematic of a real stepped motor
 * with given acceleration and absolute maximum velocity.
 */
class FakeMotor
{
public:
  FakeMotor();

  /**
   * @brief Set the motor acceleration.
   * @param acceleration The motor acceleration.
   */
  void set_acceleration(double acceleration);

  /**
   * @brief Set the motor absolute maximum velocity.
   * @param velocity The motor absolute maximum velocity.
   */
  void set_max_velocity(double velocity);

  /**
   * @brief Get the motor position at the given time.
   * @param time The time the position is computed.
   * @return The motor position at the given time.
   */
  double get_position(const rclcpp::Time& time) const;

  /**
   * @brief Set the motor target position.
   * @param time The time the command is received.
   * @param position The motor target position.
   */
  void set_target_position(const rclcpp::Time& time, double position);

  /**
   * @brief Get the motor velocity at the given time.
   * @param time The time the velocity is computed.
   * @return the motor velocity at the given time.
   */
  double get_velocity(const rclcpp::Time& time) const;

  /**
   * @brief Set the motor target velocity.
   * @param time The time the command is received.
   * @param velocity The motor target velocity.
   */
  void set_target_velocity(const rclcpp::Time& time, double velocity);

private:
  /* The motor acceleration (rad/s^2) */
  double acceleration_;

  /* The motor maximum velocity. */
  double max_velocity_;

  /* The motor position (rad) at the time a command is received. */
  double initial_position_;

  /* The motor velocity (rad/s) at the time a command is received. */
  double initial_velocity_;

  /* The motor target position. */
  double target_position_;

  /* The motor target velocity. */
  double target_velocity_;

  /* This store the time when a command is last received. */
  rclcpp::Time time_{ 0 };

  enum ControlType
  {
    Velocity,
    Position
  };

  /* Set the motor to be in velocity or position controlled mode depending on
   * the last received command.
   */
  ControlType control_type = ControlType::Position;
};
}  // namespace stepit_driver
