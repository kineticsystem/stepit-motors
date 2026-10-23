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

#ifndef MOTOR_STATE_H
#define MOTOR_STATE_H

/**
 * This class is used to exchange motor states between the ISR (Interrupt
 * Service Routine) and the main thread.
 * States are written by the ISR and read by the main thread.
 */
class MotorState
{
public:
  MotorState();

  void setSpeed(float speed);
  float getSpeed() const;
  void setPosition(long position);
  long getPosition() const;
  void setDistanceToGo(long distanceToGo);
  long getDistanceToGo() const;

  void setDecelerating(bool isDecelerating);
  bool isDecelerating() const;

private:
  float m_speed = 0.0;
  long m_currentPosition = 0;
  long m_distanceToGo = 0;

  // Only the ISR uses this.
  bool m_isDecelerating = false;
};

#endif  // MOTOR_STATE_H
