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

#include <cstdint>
#include <vector>

#include <stepit_driver/msgs/response.hpp>

namespace stepit_driver
{
// Internal structure to store joint states and targets.
class MotorState
{
public:
  explicit MotorState(uint8_t id, double position, double velocity, double distance_to_go)
    : id_{ id }, position_{ position }, velocity_{ velocity }, distance_to_go_{ distance_to_go }
  {
  }
  uint8_t id() const
  {
    return id_;
  }
  double position() const
  {
    return position_;
  }
  double velocity() const
  {
    return velocity_;
  }
  double distance_to_go() const
  {
    return distance_to_go_;
  }

private:
  uint8_t id_;
  double position_;
  double velocity_;
  double distance_to_go_;
};

class StatusResponse : public Response
{
public:
  explicit StatusResponse(Status status, std::vector<MotorState> motor_states);
  std::vector<MotorState> motor_states() const;

private:
  std::vector<MotorState> motor_states_;
};
}  // namespace stepit_driver
