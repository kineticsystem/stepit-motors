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

#include <gmock/gmock.h>

#include <stepit_driver/driver.hpp>

#include <rclcpp/rclcpp.hpp>

namespace stepit_driver::test
{
class MockDriver : public stepit_driver::Driver
{
public:
  MOCK_METHOD(bool, connect, (), (override));
  MOCK_METHOD(void, disconnect, (), (override));
  MOCK_METHOD(AcknowledgeResponse, configure, (const ConfigCommand& command), (override, const));
  MOCK_METHOD(AcknowledgeResponse, set_position, (const rclcpp::Time& time, const PositionCommand& command),
              (override, const));
  MOCK_METHOD(AcknowledgeResponse, set_velocity, (const rclcpp::Time& time, const VelocityCommand& command),
              (override, const));
  MOCK_METHOD(StatusResponse, get_status, (const rclcpp::Time& time), (override, const));
  MOCK_METHOD(InfoResponse, get_info, (const rclcpp::Time& time), (override, const));
};
}  // namespace stepit_driver::test
