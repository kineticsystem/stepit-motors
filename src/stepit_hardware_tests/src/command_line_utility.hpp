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

#include <functional>
#include <map>
#include <set>
#include <string>
#include <variant>

/**
 *  Class to parse the command line.
 */
class CommandLineUtility
{
  using LambdaWithValue = std::function<void(const char*)>;
  using LambdaWithoutValue = std::function<void()>;
  using ParameterHandler = std::variant<LambdaWithValue, LambdaWithoutValue>;

public:
  /**
   * Assign to each command parameter a lambda function to handle it.
   * @param parameter The parameter to handle.
   * @param handler The lambda function to handle the parameter value.
   * @param isMandatory True if the parameter is mandatory, else otherwise.
   */
  void registerHandler(const std::string& parameter, ParameterHandler handler, bool isMandatory = false);

  /**
   * Parse the command line and read all parameters.
   * @param argc The number of tokens in the command line.
   * @param argv The list of tokens.
   * @return True if the parsing is successful.
   */
  bool parse(int argc, char* argv[]);

private:
  // Map that associates a lambda function to each parameter to process the expected value.
  std::map<std::string, ParameterHandler> handlers;

  // Store all mandatory parameters.
  std::set<std::string> mandatoryParams;

  // Store the parameters which are parsed in the command line. If a parameter is mandatory,
  // but cannot be found in the received  parameters, then print an error.
  std::set<std::string> receivedParams;
};
