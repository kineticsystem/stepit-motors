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

#include <stepit_driver/fake/fake_motor.hpp>
#include <stepit_driver/fake/position_kinematics.hpp>
#include <stepit_driver/fake/velocity_kinematics.hpp>

#include <rclcpp/rclcpp.hpp>

namespace stepit_driver
{

FakeMotor::FakeMotor()
  : acceleration_{ 0.0 }
  , max_velocity_{ 0.0 }
  , initial_position_{ 0.0 }
  , initial_velocity_{ 0.0 }
  , target_position_{ 0.0 }
  , target_velocity_{ 0.0 }
{
}

void FakeMotor::set_acceleration(double acceleration)
{
  acceleration_ = acceleration;
}

void FakeMotor::set_max_velocity(double velocity)
{
  max_velocity_ = velocity;
}

double FakeMotor::get_position(const rclcpp::Time& time) const
{
  double t = time.seconds() - time_.seconds();
  if (control_type == ControlType::Position)
  {
    return position_kinematics::position(max_velocity_, acceleration_, initial_velocity_, initial_position_,
                                         target_position_, t);
  }
  else
  {
    return velocity_kinematics::position(max_velocity_, acceleration_, initial_position_, initial_velocity_,
                                         target_velocity_, t);
  }
}

void FakeMotor::set_target_position(const rclcpp::Time& time, double position)
{
  initial_position_ = get_position(time);
  initial_velocity_ = get_velocity(time);
  target_position_ = position;
  control_type = ControlType::Position;
  time_ = time;
}

double FakeMotor::get_velocity(const rclcpp::Time& time) const
{
  double t = time.seconds() - time_.seconds();
  if (control_type == ControlType::Position)
  {
    return position_kinematics::velocity(max_velocity_, acceleration_, initial_velocity_, initial_position_,
                                         target_position_, t);
  }
  else
  {
    return velocity_kinematics::velocity(max_velocity_, acceleration_, initial_velocity_, target_velocity_, t);
  }
}

void FakeMotor::set_target_velocity(const rclcpp::Time& time, double velocity)
{
  initial_position_ = get_position(time);
  initial_velocity_ = get_velocity(time);
  target_velocity_ = velocity;
  control_type = ControlType::Velocity;
  time_ = time;
}
}  // namespace stepit_driver
