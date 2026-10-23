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

#include <map>

#include <stepit_driver/driver.hpp>
#include <stepit_driver/fake/fake_motor.hpp>

#include <rclcpp/rclcpp.hpp>

namespace stepit_driver
{
/**
 * @brief The FakeCommandHandler class receives commands and queries from the
 * hardware interface and sends them to a fake hardware.
 */
class FakeDriver : public Driver
{
public:
  FakeDriver() = default;
  bool connect() override;
  void disconnect() override;
  AcknowledgeResponse configure(const ConfigCommand& command) const override;
  AcknowledgeResponse set_position(const rclcpp::Time& time, const PositionCommand& command) const override;
  AcknowledgeResponse set_velocity(const rclcpp::Time& time, const VelocityCommand& command) const override;
  StatusResponse get_status(const rclcpp::Time& time) const override;
  InfoResponse get_info(const rclcpp::Time& time) const override;

private:
  /* Virtual motors behaving like real stepper motors with given acceletation
   * and absolute maximum velocity.
   */
  mutable std::map<uint8_t, FakeMotor> motors_;
};
}  // namespace stepit_driver
