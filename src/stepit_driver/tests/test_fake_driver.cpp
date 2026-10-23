// Copyright 2026 Giovanni Remigi
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <numbers>
#include <vector>

#include <stepit_driver/fake/fake_driver.hpp>
#include <stepit_driver/hardware_limits.hpp>

namespace stepit_driver::test
{
using hardware_limits::kMaxAcceleration;
using hardware_limits::kMaxVelocity;

/** A configuration for one motor, at the limits unless stated otherwise. */
ConfigCommand config(uint8_t motor_id, double acceleration = kMaxAcceleration, double max_velocity = kMaxVelocity)
{
  return ConfigCommand{ { ConfigParam{ motor_id, acceleration, max_velocity } } };
}

/**
 * A configuration within the limits of the controller is accepted.
 */
TEST(TestFakeDriver, configure_accepts_values_within_limits)
{
  FakeDriver driver;
  EXPECT_EQ(Response::Status::Success, driver.configure(config(0)).status());
  EXPECT_EQ(Response::Status::Success, driver.configure(config(0, kMaxAcceleration / 2, kMaxVelocity / 2)).status());
}

/**
 * The values the URDF declares are exactly the limits of the controller, so
 * they have to be accepted: were they not, simulation would refuse the very
 * configuration the robot ships with.
 */
TEST(TestFakeDriver, configure_accepts_the_declared_limits)
{
  // The literals used in stepit.ros2_control.xacro.
  const double urdf_acceleration = 12.5663706143592;
  const double urdf_max_velocity = 18.8495559215388;

  FakeDriver driver;
  EXPECT_EQ(Response::Status::Success, driver.configure(config(0, urdf_acceleration, urdf_max_velocity)).status());
}

/**
 * A configuration the real controller would refuse has to be refused here
 * too, otherwise a robot that runs in simulation fails to activate on
 * hardware.
 */
TEST(TestFakeDriver, configure_rejects_values_beyond_the_limits)
{
  FakeDriver driver;
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, kMaxAcceleration * 2, kMaxVelocity)).status());
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, kMaxAcceleration, kMaxVelocity * 2)).status());
}

/**
 * A non positive acceleration is refused: besides being meaningless, it
 * divides by zero in the position kinematics.
 */
TEST(TestFakeDriver, configure_rejects_non_positive_values)
{
  FakeDriver driver;
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, 0.0, kMaxVelocity)).status());
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, -kMaxAcceleration, kMaxVelocity)).status());
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, kMaxAcceleration, 0.0)).status());
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(0, kMaxAcceleration, -kMaxVelocity)).status());
}

/**
 * A motor id the controller does not drive is refused.
 */
TEST(TestFakeDriver, configure_rejects_an_unknown_motor_id)
{
  FakeDriver driver;
  EXPECT_EQ(Response::Status::Failure, driver.configure(config(99)).status());
}

/**
 * A batch whose last entry is invalid is refused as a whole: no motor is
 * configured, as on the firmware.
 */
TEST(TestFakeDriver, configure_rejects_a_batch_with_one_bad_entry)
{
  FakeDriver driver;
  const ConfigCommand command{ { ConfigParam{ 0, kMaxAcceleration, kMaxVelocity },
                                 ConfigParam{ 1, kMaxAcceleration, kMaxVelocity },
                                 ConfigParam{ 2, kMaxAcceleration * 99, kMaxVelocity } } };

  EXPECT_EQ(Response::Status::Failure, driver.configure(command).status());

  // Nothing was configured, so even the valid motors are unknown.
  EXPECT_EQ(Response::Status::Failure,
            driver.set_velocity(rclcpp::Time{}, VelocityCommand{ { VelocityGoal{ 0, 0.0 } } }).status());
}

/**
 * Commanding a motor that was never configured fails, as it does on the
 * firmware, instead of being silently ignored.
 */
TEST(TestFakeDriver, motion_commands_reject_an_unknown_motor_id)
{
  FakeDriver driver;
  ASSERT_EQ(Response::Status::Success, driver.configure(config(0)).status());

  EXPECT_EQ(Response::Status::Success,
            driver.set_velocity(rclcpp::Time{}, VelocityCommand{ { VelocityGoal{ 0, 0.0 } } }).status());
  EXPECT_EQ(Response::Status::Failure,
            driver.set_velocity(rclcpp::Time{}, VelocityCommand{ { VelocityGoal{ 3, 0.0 } } }).status());
  EXPECT_EQ(Response::Status::Failure,
            driver.set_position(rclcpp::Time{}, PositionCommand{ { PositionGoal{ 3, 0.0 } } }).status());
}

}  // namespace stepit_driver::test
