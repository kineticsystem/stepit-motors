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

#pragma once

#include <cstddef>
// cpplint's list of C++ headers predates C++20, so it takes <numbers> for a C
// system header and wants it before <cstddef>.
#include <numbers>  // NOLINT(build/include_order)

#include <stepit_driver/msgs/info_response.hpp>

namespace stepit_driver::hardware_limits
{
/**
 * Limits of the StepIt controller. The firmware is the authority: these
 * mirror NUMBER_OF_MOTORS, MAX_ACCELERATION, MAX_SPEED and CONFIG_TOLERANCE
 * in src/stepit_mcu/src/main.cpp, converted from steps to radians.
 *
 * FakeDriver enforces them so that a configuration accepted in simulation is
 * one the real controller accepts too. Without this, a robot description that
 * works in simulation can fail to activate the moment it is run on hardware.
 */

// Number of motors the controller drives.
constexpr std::size_t kMotorCount = 5;

// Maximum acceleration: 2 rotations per square second.
constexpr double kMaxAcceleration = 4.0 * std::numbers::pi;

// Maximum velocity: 3 rotations per second.
constexpr double kMaxVelocity = 6.0 * std::numbers::pi;

// Tolerance applied when comparing against the limits above, so that a value
// stated as the limit itself is not rejected by rounding.
constexpr double kTolerance = 1.001;

// The firmware version the simulated controller reports. Its major number is
// the protocol version this workspace speaks: see VERSION_MAJOR in
// src/stepit_mcu/src/main.cpp.
const Version kFirmwareVersion{ 1, 1, 0 };

}  // namespace stepit_driver::hardware_limits
