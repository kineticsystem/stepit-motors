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

#include <stepit_driver/default_driver.hpp>
#include <stepit_driver/default_driver_factory.hpp>
#include <stepit_driver/default_framed_serial_factory.hpp>
#include <stepit_driver/fake/fake_driver.hpp>

#include <framed_serial/data_utils.hpp>
#include <framed_serial/default_framed_serial.hpp>

#include <rclcpp/logging.hpp>

namespace stepit_driver
{
const auto kLogger = rclcpp::get_logger("stepit_default_driver_factory");

constexpr auto kUseDummyParamName = "use_dummy";
constexpr auto kUseDummyParamDefault = "false";

using framed_serial::data_utils::to_lower;

std::unique_ptr<stepit_driver::Driver>
stepit_driver::DefaultDriverFactory::create(const hardware_interface::HardwareInfo& info)
{
  if (info.hardware_parameters.count(kUseDummyParamName) &&
      to_lower(info.hardware_parameters.at(kUseDummyParamName)) != to_lower(kUseDummyParamDefault))
  {
    return std::make_unique<FakeDriver>();
  }
  else
  {
    auto framed_serial = DefaultFramedSerialFactory().create(info);
    return std::make_unique<DefaultDriver>(std::move(framed_serial));
  }
}
}  // namespace stepit_driver
