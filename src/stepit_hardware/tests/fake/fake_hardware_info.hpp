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

#include <string>

#include <hardware_interface/hardware_info.hpp>

namespace stepit_driver::test
{
/**
 * Simplified Hardware Interface configuration for testing.
 */
class FakeHardwareInfo : public hardware_interface::HardwareInfo
{
public:
  FakeHardwareInfo()
  {
    name = "StepitHardware";
    type = "system";
    hardware_plugin_name = "stepit_driver/StepitHardware";
    hardware_parameters = {
      { "usb_port", "/dev/ttyUSB0" },
      { "baud_rate", "9600" },
      { "timeout", "0.5" },
      { "use_dummy", "true" },
    };

    auto make_joint = [](const std::string& joint_name, const std::string& id) {
      hardware_interface::ComponentInfo joint;
      joint.name = joint_name;
      joint.type = "joint";
      hardware_interface::InterfaceInfo pos{ .name = "position", .size = 0, .parameters = {}, .enable_limits = false };
      hardware_interface::InterfaceInfo vel{ .name = "velocity", .size = 0, .parameters = {}, .enable_limits = false };
      joint.command_interfaces = { pos, vel };
      joint.state_interfaces = { pos, vel };
      joint.parameters = { { "id", id }, { "acceleration", "3.14159" }, { "max_velocity", "6.28319" } };
      return joint;
    };

    joints = {
      make_joint("joint1", "0"), make_joint("joint2", "1"), make_joint("joint3", "2"),
      make_joint("joint4", "3"), make_joint("joint5", "4"),
    };
  }
};
}  // namespace stepit_driver::test
