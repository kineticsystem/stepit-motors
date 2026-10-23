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

#include "command_line_utility.hpp"

#include <iostream>

void CommandLineUtility::registerHandler(const std::string& parameter, ParameterHandler handler, bool isMandatory)
{
  handlers[parameter] = handler;
  if (isMandatory)
  {
    mandatoryParams.insert(parameter);
  }
}

bool CommandLineUtility::parse(int argc, char* argv[])
{
  for (int i = 1; i < argc; i++)
  {
    auto it = handlers.find(argv[i]);
    if (it != handlers.end())
    {
      receivedParams.insert(it->first);

      if (std::holds_alternative<LambdaWithValue>(it->second))
      {
        auto& handler = std::get<LambdaWithValue>(it->second);
        i++;
        if (i < argc)
        {
          handler(argv[i]);
        }
        else
        {
          std::cerr << it->first << " requires a value.\n";
        }
      }
      else if (std::holds_alternative<LambdaWithoutValue>(it->second))
      {
        auto& handler = std::get<LambdaWithoutValue>(it->second);
        handler();
      }
    }
    else
    {
      std::cerr << "Unknown argument: " << argv[i] << "\n";
      return false;
    }
  }

  for (const auto& param : mandatoryParams)
  {
    if (receivedParams.find(param) == receivedParams.end())
    {
      std::cerr << "Missing mandatory argument: " << param << "\n";
      return false;
    }
  }

  return true;
}
