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

#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <Arduino.h>
#include "Buffer.h"

/**
 * This implements a circular byte buffer to be used during a serial port
 * communication.
 *
 * IMPORTANT NOTE
 * Arduino follows the Little Endian convention to store numbers: a pointer to
 * an int or long gives the address of the LSB (Less Significant Byte).
 * Numbers are sent and received to and from the network following IEEE 754
 * specification, with Most Significant Byte first.
 * We use the above specifications when converting numbers from bytes to
 * primitive types, and back.
 */
class DataBuffer
{
public:
  explicit DataBuffer(unsigned int bufferSize);

  // Returns how much data is currently stored in the buffer.
  int getSize();

  // Returns the maximum capacity of the buffer
  int getCapacity();

  // Insert a byte at the given location.
  void addByte(byte in, BufferPosition location);

  // Remove a byte from the given location.
  byte removeByte(BufferPosition location);

  // Insert an int at the given location.
  void addInt(int in, BufferPosition location);

  // Remove an int from the given location.
  int removeInt(BufferPosition location);

  // Insert a long (4 bytes) at the given location.
  void addLong(long in, BufferPosition location);

  // Remove a long (4 bytes) from the given location.
  long removeLong(BufferPosition location);

  // Insert a float (4 bytes) at the given location.
  void addFloat(float in, BufferPosition location);

  // Remove a float (4 bytes) from the given location.
  float removeFloat(BufferPosition location);

  // This method resets the buffer into an original state (with no data).
  void clear();

private:
  Buffer<byte> m_buffer;
};

#endif  // DATA_BUFFER_H
