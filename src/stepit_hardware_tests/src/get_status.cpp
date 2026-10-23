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

#include <iostream>

#include <stepit_driver/default_driver.hpp>

#include <framed_serial/default_serial.hpp>
#include <framed_serial/default_framed_serial.hpp>

#include "command_line_utility.hpp"

constexpr auto kUsbPort = "/dev/ttyACM0";
constexpr auto kBaudRate = 9600;
constexpr auto kTimeout = 0.2;

using framed_serial::DefaultFramedSerial;
using framed_serial::DefaultSerial;
using stepit_driver::DefaultDriver;
using stepit_driver::MotorState;

/**
 * Main entry point for a command-line utility to interface and control
 * stepper motors driver via UART. This utility reads configuration parameters
 * such as the COM port, baud rate, and timeout from command-line arguments,
 * establishes a connection to the hardware, and retrieves the current status of
 * the motors.
 *
 * The primary purpose of this program is to serve as a diagnostic tool for testing
 * the hardware's response to status requests. It ensures that the hardware is
 * connected correctly and can be communicated with without issues.
 */
int main(int argc, char* argv[])
{
  CommandLineUtility cli;

  std::string port = kUsbPort;
  cli.registerHandler("--port", [&port](const char* value) { port = value; }, false);

  uint32_t baudrate = kBaudRate;
  cli.registerHandler(
      "--baudrate", [&baudrate](const char* value) { baudrate = static_cast<uint32_t>(std::stoul(value)); }, false);

  double timeout = kTimeout;
  cli.registerHandler("--timeout", [&timeout](const char* value) { timeout = std::stoi(value); }, false);

  cli.registerHandler("-h", [&]() {
    std::cout << "Usage: ./set_relative_pressure [OPTIONS]\n"
              << "Options:\n"
              << "  --port VALUE                 Set the com port (default " << kUsbPort << ")\n"
              << "  --baudrate VALUE             Set the baudrate (default " << kBaudRate << "bps)\n"
              << "  --timeout VALUE              Set the read/write timeout (default " << kTimeout << "s)\n"
              << "  -h                           Show this help message\n";
    exit(0);
  });

  if (!cli.parse(argc, argv))
  {
    return 1;
  }

  try
  {
    auto serial = std::make_unique<DefaultSerial>();
    serial->set_port(port);
    serial->set_baudrate(baudrate);
    serial->set_timeout(std::chrono::duration<double>{ timeout });

    auto framed_serial = std::make_unique<DefaultFramedSerial>(std::move(serial));

    auto driver = std::make_unique<DefaultDriver>(std::move(framed_serial));

    std::cout << "Using the following parameters: " << std::endl;
    std::cout << " - port: " << port << std::endl;
    std::cout << " - baudrate: " << baudrate << "bps" << std::endl;
    std::cout << " - read/write timeout: " << timeout << "s" << std::endl;

    std::cout << "Checking if the driver is connected..." << std::endl;
    bool connected = driver->connect();
    if (!connected)
    {
      std::cout << "The driver is not connected" << std::endl;
      return 1;
    }

    std::cout << "The driver is connected." << std::endl;
    std::cout << "Reading the driver status..." << std::endl;

    auto response = driver->get_status(rclcpp::Time{});

    std::cout << "Status retrieved:" << std::endl;

    for (const MotorState& state : response.motor_states())
    {
      // Cast to int: streaming a uint8_t would print it as a character.
      std::cout << " - motor id: " << static_cast<int>(state.id()) << std::endl;
      std::cout << " - motor position: " << state.position() << "rad" << std::endl;
      std::cout << " - motor velocity: " << state.velocity() << "rad/s" << std::endl;
      std::cout << " - distance to go: " << state.distance_to_go() << "rad" << std::endl;
    }
  }
  catch (const serial::IOException& e)
  {
    std::cout << "Failed to communicating with the hardware:" << e.what();
    return 1;
  }
}
