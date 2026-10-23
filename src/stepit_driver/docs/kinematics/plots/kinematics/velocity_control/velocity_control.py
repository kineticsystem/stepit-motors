# Copyright 2023 Giovanni Remigi
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


# Exported functions.
__all__ = ["position", "velocity"]


def sgn(number: float):
    """
    Return the sign of the given number.
    :param number: The number to calculate the sign of.
    :return: The sign of the given number.
    """
    return bool(number > 0) - bool(number < 0)


def position(v_max: float, a: float, x0: float, v0: float, v1: float, t: float):
    """
    Position of a motor with acceleration a while changing velocity from v0 to v1.
    :param v_max: The maximum velocity.
    :param a:     The acceleration.
    :param v0:    The initial velocity.
    :param v1:    The initial velocity.
    :param x0:    The initial position.
    :param t:     The time.
    :return:      The motor position.
    """
    v0 = max(-v_max, min(v0, v_max))
    v1 = max(-v_max, min(v1, v_max))
    t1 = abs(v1 - v0) / a
    if t <= t1:
        x = x0 + v0 * t + 0.5 * a * pow(t, 2) * sgn(v1 - v0)
    else:
        x = x0 + (v0 - v1) * t1 + 0.5 * a * pow(t1, 2) * sgn(v1 - v0) + v1 * t
    return x


def velocity(v_max: float, a: float, v0: float, v1: float, t: float):
    """
    Velocity of a motor with acceleration a while changing velocity from v0 to v1.
    :param v_max: The maximum velocity.
    :param a:     The acceleration.
    :param v0:    The initial velocity.
    :param v1:    The final velocity.
    :param t:     The time.
    :return:      The motor velocity.
    """
    v0 = max(-v_max, min(v0, v_max))
    v1 = max(-v_max, min(v1, v_max))
    t1 = abs(v1 - v0) / a
    if t <= t1:
        v = v0 + a * t * sgn(v1 - v0)
    else:
        v = v1
    return v
