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

#include <stepit_driver/default_serial_factory.hpp>

#include <framed_serial/default_serial.hpp>

#include <rclcpp/logging.hpp>

namespace stepit_driver
{
const auto kLogger = rclcpp::get_logger("DefaultSerialFactory");

constexpr auto kUsbPortParamName = "usb_port";
constexpr auto kUsbPortParamDefault = "/dev/ttyACM0";

constexpr auto kBaudrateParamName = "baudrate";
constexpr auto kBaudrateAddressParamDefault = 9600;

constexpr auto kTimeoutParamName = "timeout";
constexpr auto kTimeoutParamDefault = 0.2;

std::unique_ptr<framed_serial::Serial> DefaultSerialFactory::create(const hardware_interface::HardwareInfo& info) const
{
  RCLCPP_INFO(kLogger, "Reading usb_port...");
  std::string usb_port = info.hardware_parameters.count(kUsbPortParamName) ?
                             info.hardware_parameters.at(kUsbPortParamName) :
                             kUsbPortParamDefault;
  RCLCPP_INFO(kLogger, "usb_port: %s", usb_port.c_str());

  RCLCPP_INFO(kLogger, "Reading baudrate...");
  uint32_t baudrate = info.hardware_parameters.count(kBaudrateParamName) ?
                          static_cast<uint32_t>(std::stoul(info.hardware_parameters.at(kBaudrateParamName))) :
                          kBaudrateAddressParamDefault;
  RCLCPP_INFO(kLogger, "baudrate: %dbps", baudrate);

  RCLCPP_INFO(kLogger, "Reading timeout...");
  double timeout = info.hardware_parameters.count(kTimeoutParamName) ?
                       std::stod(info.hardware_parameters.at(kTimeoutParamName)) :
                       kTimeoutParamDefault;
  RCLCPP_INFO(kLogger, "timeout: %fs", timeout);

  auto serial = create_objects();
  serial->set_port(usb_port);
  serial->set_baudrate(baudrate);
  serial->set_timeout(std::chrono::duration<double>{ timeout });
  return serial;
}

std::unique_ptr<framed_serial::Serial> DefaultSerialFactory::create_objects() const
{
  return std::make_unique<framed_serial::DefaultSerial>();
}
}  // namespace stepit_driver
