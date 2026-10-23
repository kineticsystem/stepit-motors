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

#include "DataBuffer.h"

DataBuffer::DataBuffer(unsigned int bufferSize) : m_buffer{ Buffer<byte>{ bufferSize } }
{
}

int DataBuffer::getSize()
{
  return m_buffer.size();
}

int DataBuffer::getCapacity()
{
  return m_buffer.capacity();
}

void DataBuffer::clear()
{
  m_buffer.clear();
}

void DataBuffer::addByte(byte in, BufferPosition location)
{
  m_buffer.add(in, location);
}

byte DataBuffer::removeByte(BufferPosition location)
{
  return m_buffer.remove(location);
}

// Following IEEE 754 specifications a type int is always sent/received
// with Most Significant Byte (MSB) first.
// In Arduino a type int is stored in memory in Little Endian format,
// it means a pointer to an int gives the address of the LSB.
// [LSB][...][...][MSB].
void DataBuffer::addInt(int in, BufferPosition location)
{
  byte* pointer = (byte*)&in;
  if (location == BufferPosition::Tail)
  {
    m_buffer.add(pointer[1], BufferPosition::Tail);
    m_buffer.add(pointer[0], BufferPosition::Tail);
  }
  else
  {
    m_buffer.add(pointer[0], BufferPosition::Head);
    m_buffer.add(pointer[1], BufferPosition::Head);
  }
}

// Following IEEE 754 specifications, a type int is always sent/received
// with Most Significant Byte (MSB) first.
// In Arduino a type int is stored in memory in Little Endian format,
// it means a pointer to an int gives the address of the LSB.
// [LSB][...][...][MSB].
int DataBuffer::removeInt(BufferPosition location)
{
  int ret;
  byte* pointer = (byte*)&ret;
  if (location == BufferPosition::Head)
  {
    pointer[1] = m_buffer.remove(BufferPosition::Head);
    pointer[0] = m_buffer.remove(BufferPosition::Head);
  }
  else
  {
    pointer[0] = m_buffer.remove(BufferPosition::Tail);
    pointer[1] = m_buffer.remove(BufferPosition::Tail);
  }
  return ret;
}

// Following IEEE 754 specifications, a type long is always sent/received
// with Most Significant Byte (MSB) first.
// In Arduino a long is stored in memory in Little Endian format,
// it means a pointer to a long gives the address of the LSB.
// [LSB][...][...][MSB].
void DataBuffer::addLong(long in, BufferPosition location)
{
  byte* pointer = (byte*)&in;
  if (location == BufferPosition::Tail)
  {
    m_buffer.add(pointer[3], BufferPosition::Tail);
    m_buffer.add(pointer[2], BufferPosition::Tail);
    m_buffer.add(pointer[1], BufferPosition::Tail);
    m_buffer.add(pointer[0], BufferPosition::Tail);
  }
  else
  {
    m_buffer.add(pointer[0], BufferPosition::Head);
    m_buffer.add(pointer[1], BufferPosition::Head);
    m_buffer.add(pointer[2], BufferPosition::Head);
    m_buffer.add(pointer[3], BufferPosition::Head);
  }
}

// Following IEEE 754 specifications, a type long is always sent/received
// with Most Significant Byte (MSB) first.
// In Arduino, a long is stored in memory in Little Endian format,
// it means a pointer to a long gives the address of the LSB.
// [LSB][...][...][MSB].
long DataBuffer::removeLong(BufferPosition location)
{
  long ret;
  byte* pointer = (byte*)&ret;
  if (location == BufferPosition::Head)
  {
    pointer[3] = m_buffer.remove(BufferPosition::Head);
    pointer[2] = m_buffer.remove(BufferPosition::Head);
    pointer[1] = m_buffer.remove(BufferPosition::Head);
    pointer[0] = m_buffer.remove(BufferPosition::Head);
  }
  else
  {
    pointer[0] = m_buffer.remove(BufferPosition::Tail);
    pointer[1] = m_buffer.remove(BufferPosition::Tail);
    pointer[2] = m_buffer.remove(BufferPosition::Tail);
    pointer[3] = m_buffer.remove(BufferPosition::Tail);
  }
  return ret;
}

// Following IEEE 754 specifications, a type float is always sent/received
// with Sign and Exponent first followed by the Significand in
// Most Significant Byte (MSB) first.
void DataBuffer::addFloat(float in, BufferPosition location)
{
  byte* pointer = (byte*)&in;
  if (location == BufferPosition::Tail)
  {
    m_buffer.add(pointer[3], BufferPosition::Tail);
    m_buffer.add(pointer[2], BufferPosition::Tail);
    m_buffer.add(pointer[1], BufferPosition::Tail);
    m_buffer.add(pointer[0], BufferPosition::Tail);
  }
  else
  {
    m_buffer.add(pointer[0], BufferPosition::Head);
    m_buffer.add(pointer[1], BufferPosition::Head);
    m_buffer.add(pointer[2], BufferPosition::Head);
    m_buffer.add(pointer[3], BufferPosition::Head);
  }
}

// Following IEEE 754 specifications a type float is stored in the following way:
// - Sign: 1bit
// - Exponent: 8 bits
// - Significand: 23 bits.
float DataBuffer::removeFloat(BufferPosition location)
{
  float ret;
  byte* pointer = (byte*)&ret;
  if (location == BufferPosition::Head)
  {
    pointer[3] = m_buffer.remove(BufferPosition::Head);
    pointer[2] = m_buffer.remove(BufferPosition::Head);
    pointer[1] = m_buffer.remove(BufferPosition::Head);
    pointer[0] = m_buffer.remove(BufferPosition::Head);
  }
  else
  {
    pointer[0] = m_buffer.remove(BufferPosition::Tail);
    pointer[1] = m_buffer.remove(BufferPosition::Tail);
    pointer[2] = m_buffer.remove(BufferPosition::Tail);
    pointer[3] = m_buffer.remove(BufferPosition::Tail);
  }
  return ret;
}
