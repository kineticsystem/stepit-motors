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
#include <string>
#include <vector>

#include <stepit_driver/msgs/response.hpp>

namespace stepit_driver
{
/**
 * The motion limits of one motor, as reported by the controller. The
 * controller is the authority on these: they depend on the board, the motors
 * and the mechanics, and exceeding them makes an open loop stepper lose steps
 * without anything noticing.
 */
class MotorLimits
{
public:
  explicit MotorLimits(uint8_t id, double max_acceleration, double max_velocity)
    : id_{ id }, max_acceleration_{ max_acceleration }, max_velocity_{ max_velocity }
  {
  }
  uint8_t id() const
  {
    return id_;
  }
  double max_acceleration() const
  {
    return max_acceleration_;
  }
  double max_velocity() const
  {
    return max_velocity_;
  }

private:
  uint8_t id_;
  double max_acceleration_;
  double max_velocity_;
};

/**
 * The firmware version reported by the controller.
 *
 * The major number is the wire compatibility of the protocol: a driver that
 * does not know a controller's major version cannot trust the bytes it reads
 * from it. The minor and the patch number identify the build.
 */
class Version
{
public:
  explicit Version(uint8_t major = 0, uint8_t minor = 0, uint8_t patch = 0)
    : major_{ major }, minor_{ minor }, patch_{ patch }
  {
  }
  uint8_t major() const
  {
    return major_;
  }
  uint8_t minor() const
  {
    return minor_;
  }
  uint8_t patch() const
  {
    return patch_;
  }
  std::string to_string() const
  {
    return std::to_string(major_) + "." + std::to_string(minor_) + "." + std::to_string(patch_);
  }

private:
  uint8_t major_;
  uint8_t minor_;
  uint8_t patch_;
};

class InfoResponse : public Response
{
public:
  explicit InfoResponse(Status status, const std::string& info, const Version& version = Version{},
                        const std::vector<MotorLimits>& limits = {});
  std::string info() const;
  Version version() const;
  std::vector<MotorLimits> limits() const;

private:
  std::string info_;
  Version version_;
  std::vector<MotorLimits> limits_;
};
}  // namespace stepit_driver
