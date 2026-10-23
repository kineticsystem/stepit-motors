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

#include <stepit_driver/fake/position_kinematics.hpp>

#include <algorithm>
#include <cmath>

namespace stepit_driver::position_kinematics
{

/**
 * @brief Compute the sign of the given number.
 * @param val The number to calculate the sign of.
 * @return The sign of the given number.
 */
double sgn(double val)
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

/**
 * @brief Compute the time to stop for a motor with acceleration a and rotating
 * speed v0.
 * @param a The motor acceleration (rad/s^2).
 * @param v0 The current speed     (rad/s).
 * @return The time to stop        (s).
 */
double time_to_stop(double a, double v0)
{
  return std::abs(v0 / a);
}

/**
 * @brief Compute the time for a motor, with acceleration a and zero initial
 * velocity, to move from position x0 to position x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param x0    The initial position (rad).
 * @param x1    The target position  (rad).
 * @return      The motor velocity   (rad/s).
 */
double time_to_go(double v_max, double a, double x0, double x1)
{
  double dx = x1 - x0;
  double total_time = 0;
  if (std::abs(dx) >= pow(v_max, 2) / a)
  {
    double t1 = v_max / a;
    double t2 = t1 + std::max(0.0, (std::abs(dx) - pow(v_max, 2) / a)) / v_max;
    total_time = t1 + t2;
  }
  else
  {
    double t1 = std::sqrt(std::abs(dx) / a);
    total_time = 2 * t1;
  }
  return total_time;
}

/**
 * @brief Compute the position of a motor with acceleration a and zero initial
 * velocity, moving from position x0 to x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param x0    The initial position (rad).
 * @param x1    The target position  (rad).
 * @param t     The time             (s).
 * @return      The motor position   (rad).
 */
double position(double v_max, double a, double x0, double x1, double t)
{
  double dx = x1 - x0;
  double x = 0;
  if (std::abs(dx) >= pow(v_max, 2) / a)
  {
    double t1 = v_max / a;
    double t2 = t1 + std::max(0.0, (std::abs(dx) - pow(v_max, 2) / a)) / v_max;
    double t3 = t1 + t2;
    if (t <= t1)
    {
      x = 0.5 * a * pow(t, 2);
    }
    else if (t <= t2)
    {
      x = 0.5 * pow(v_max, 2) / a + v_max * (t - t1);
    }
    else if (t <= t3)
    {
      x = 0.5 * pow(v_max, 2) / a + v_max * (t - t1) - 0.5 * a * pow(t - t2, 2);
    }
    else
    {
      x = pow(v_max, 2) / a + v_max * (t2 - t1);
    }
  }
  else
  {
    double t1 = std::sqrt(std::abs(dx) / a);
    double t2 = 2.0 * t1;
    if (t <= t1)
    {
      x = 0.5 * a * pow(t, 2);
    }
    else if (t <= t2)
    {
      x = 0.5 * std::abs(dx) + a * t1 * (t - t1) - 0.5 * a * pow(t - t1, 2);
    }
    else
    {
      x = std::abs(dx);
    }
  }
  return x * sgn(dx);
}

/**
 * @brief Compute the velocity of a motor with acceleration a and zero initial
 * velocity, moving from position x0 to x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param x0    The initial position (rad).
 * @param x1    The target position  (rad).
 * @param t     The time             (s).
 * @return      The motor velocity   (rad/s).
 */
double velocity(double v_max, double a, double x0, double x1, double t)
{
  double v = 0.0;
  double dx = x1 - x0;
  if (std::abs(dx) >= pow(v_max, 2) / a)
  {
    double t1 = v_max / a;
    double t2 = t1 + std::max(0.0, (std::abs(dx) - pow(v_max, 2) / a)) / v_max;
    double t3 = t1 + t2;
    if (t <= t1)
    {
      v = a * t;
    }
    else if (t <= t2)
    {
      v = a * t1;
    }
    else if (t <= t3)
    {
      v = a * t1 - a * (t - t2);
    }
  }
  else
  {
    double t1 = std::sqrt(std::abs(dx) / a);
    double t2 = 2.0 * t1;
    if (t <= t1)
    {
      v = a * t;
    }
    else if (t <= t2)
    {
      v = a * t1 - a * (t - t1);
    }
  }
  return v * sgn(dx);
}

double distance_to_stop(double a, double v0)
{
  return 0.5 * pow(v0, 2) / a;
}

double time_to_go(double v_max, double a, double v0, double x0, double x1)
{
  v0 = std::clamp(v0, -v_max, v_max);
  double dx = x1 - x0;
  double d_stop = distance_to_stop(a, v0);
  double t_stop = time_to_stop(a, v0);
  double total_time = 0;
  if ((v0 >= 0 && dx >= d_stop) || (v0 <= 0 && dx <= -d_stop))  // Keep accelerating.
  {
    total_time = time_to_go(v_max, a, x0, x1 + sgn(v0) * d_stop) - t_stop;
  }
  else  // Decelerate and reverse.
  {
    total_time = time_to_go(v_max, a, x0, x1 - sgn(v0) * d_stop) + t_stop;
  }
  return total_time;
}

double position(double v_max, double a, double v0, double x0, double x1, double t)
{
  double x = 0.0;
  v0 = std::clamp(v0, -v_max, v_max);
  double dx = x1 - x0;
  double d_stop = distance_to_stop(a, v0);
  double t_stop = time_to_stop(a, v0);
  if ((v0 >= 0 && dx >= d_stop) || (v0 <= 0 && dx <= -d_stop))  // Keep accelerating.
  {
    x = position(v_max, a, x0, x1 + sgn(v0) * d_stop, t + t_stop) - sgn(v0) * d_stop;
  }
  else
  {
    if (t <= t_stop)  // Decelerate.
    {
      x = v0 * (t - t_stop) + sgn(v0) * (0.5 * a * (pow(t_stop, 2) - pow(t, 2)) + d_stop);
    }
    else  // Reverse.
    {
      x = position(v_max, a, x0, x1 - sgn(v0) * d_stop, t - t_stop) + sgn(v0) * d_stop;
    }
  }
  return x + x0;
}

double velocity(double v_max, double a, double v0, double x0, double x1, double t)
{
  double v = 0.0;
  v0 = std::clamp(v0, -v_max, v_max);
  double dx = x1 - x0;
  double d_stop = distance_to_stop(a, v0);
  double t_stop = time_to_stop(a, v0);
  if ((v0 >= 0 && dx >= d_stop) || (v0 <= 0 && dx <= -d_stop))  // Keep accelerating.
  {
    v = velocity(v_max, a, x0, x1 + sgn(v0) * d_stop, t + t_stop);
  }
  else
  {
    if (t <= t_stop)  // Decelerate.
    {
      v = v0 - sgn(v0) * a * t;
    }
    else  // Reverse.
    {
      v = velocity(v_max, a, x0, x1 - sgn(v0) * d_stop, t - t_stop);
    }
  }
  return v;
}

}  // namespace stepit_driver::position_kinematics
