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

#include <stepit_driver/fake/velocity_kinematics.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace stepit_driver::velocity_kinematics
{

/**
 * @brief Compute the sign of the given number.
 * @param val The number to calculate the sign of.
 * @return The sign of the given number.
 */
float sgn(double val)
{
  if (val > 0)
  {
    return 1.0;
  }
  if (val < 0)
  {
    return -1.0;
  }
  return 0.0;
}

double position(double v_max, double a, double x0, double v0, double v1, double t)
{
  double x;
  v0 = std::clamp(v0, -v_max, v_max);
  v1 = std::clamp(v1, -v_max, v_max);
  double t1 = std::abs(v1 - v0) / a;
  if (t <= t1)
  {
    x = x0 + v0 * t + 0.5 * a * pow(t, 2) * sgn(v1 - v0);
  }
  else
  {
    x = x0 + (v0 - v1) * t1 + 0.5 * a * pow(t1, 2) * sgn(v1 - v0) + v1 * t;
  }
  return x;
}

double velocity(double v_max, double a, double v0, double v1, double t)
{
  double v;
  v0 = std::clamp(v0, -v_max, v_max);
  v1 = std::clamp(v1, -v_max, v_max);
  double t1 = std::abs(v1 - v0) / a;
  if (t <= t1)
  {
    v = v0 + a * t * sgn(v1 - v0);
  }
  else
  {
    v = v1;
  }
  return v;
}

}  // namespace stepit_driver::velocity_kinematics
