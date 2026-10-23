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

namespace stepit_driver::position_kinematics
{

/**
 * @brief Compute the absolute distance to stop for a motor with acceleration a
 * and rotating speed v0.
 * @param a  The motor acceleration (rad/s^2).
 * @param v0 The current speed      (rad/s).
 * @return   The distance to stop   (rad).
 */
double distance_to_stop(double a, double v0);

/**
 * @brief Compute the time for a motor, with acceleration a and initial velocity
 * v0, to move from position x0 to position x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param v0    The initial velocity (rad/s).
 * @param x0    The initial position (rad/s).
 * @param x1    The target position  (rad).
 * @return      The time to go       (s).
 */
double time_to_go(double v_max, double a, double v0, double x0, double x1);

/**
 * @brief Compute the position of a motor with acceleration a and initial
 * velocity v0, moving from position x0 to x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param v0    The initial velocity (rad/s).
 * @param x0    The initial position (rad).
 * @param x1    The target position  (rad).
 * @param t     The time             (s).
 * @return      The motor position   (rad).
 */
double position(double v_max, double a, double v0, double x0, double x1, double t);

/**
 * @brief Compute the velocity of a motor with acceleration a and initial
 * velocity v0, moving from position x0 to x1.
 * @param v_max The maximum velocity (rad/s).
 * @param a     The acceleration     (rad/s^2).
 * @param v0    The initial velocity (rad/s).
 * @param x0    The initial position (rad).
 * @param x1    The target position  (rad).
 * @param t     The time             (s).
 * @return      The motor velocity   (rad/s).
 */
double velocity(double v_max, double a, double v0, double x0, double x1, double t);

}  // namespace stepit_driver::position_kinematics
