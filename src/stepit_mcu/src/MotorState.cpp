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

#include "MotorState.h"

MotorState::MotorState()
{
}

void MotorState::setSpeed(float speed)
{
  m_speed = speed;
}

float MotorState::getSpeed() const
{
  return m_speed;
}

void MotorState::setPosition(long position)
{
  m_currentPosition = position;
}

long MotorState::getPosition() const
{
  return m_currentPosition;
}

void MotorState::setDistanceToGo(long distanceToGo)
{
  m_distanceToGo = distanceToGo;
}

long MotorState::getDistanceToGo() const
{
  return m_distanceToGo;
}

void MotorState::setDecelerating(bool isDecelerating)
{
  m_isDecelerating = isDecelerating;
}

bool MotorState::isDecelerating() const
{
  return m_isDecelerating;
}
