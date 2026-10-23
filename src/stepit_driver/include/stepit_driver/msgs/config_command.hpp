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

#include <vector>

#include <stepit_driver/msgs/request.hpp>

namespace stepit_driver
{

class ConfigParam
{
public:
  explicit ConfigParam(uint8_t motor_id, double acceleration, double max_velocity)
    : motor_id_{ motor_id }, acceleration_{ acceleration }, max_velocity_{ max_velocity } {};
  uint8_t motor_id() const
  {
    return motor_id_;
  }
  double acceleration() const
  {
    return acceleration_;
  }
  double max_velocity() const
  {
    return max_velocity_;
  }

private:
  uint8_t motor_id_;
  double acceleration_;
  double max_velocity_;
};

/**
 * @brief Command to configure a group of motors.
 */
class ConfigCommand : public Request
{
public:
  explicit ConfigCommand(const std::vector<ConfigParam>& params);
  std::vector<ConfigParam> params() const;

private:
  std::vector<ConfigParam> params_;
};
}  // namespace stepit_driver
