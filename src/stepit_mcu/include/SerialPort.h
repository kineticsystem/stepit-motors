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

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <Arduino.h>
#include "DataBuffer.h"

// This class reads and writes PPP frames received from the serial port.
// A callback method is used to process incoming frames.
// CRC is calculated on all incoming frames and in case of error the frame is
// dumped.
// As a general rule, when int, long or float are sent, the Most Significant
// Byte (MSB) is sent first.

class SerialPort
{
public:
  explicit SerialPort(unsigned int inBuffersize, unsigned int outBufferSize);
  ~SerialPort();

  boolean isBusyWriting();
  void setCallback(void (*callback)(DataBuffer*));

  /**
   * Initialize the serial connection communication speed.
   */
  void init(int baudRate);

  /**
   * This must be called in each arduino loop iteration. It simulatenously
   * read and write from and to the serial port.
   */
  void update();

  /**
   * Write the given data buffer.
   * @param buffer The buffer contains a sequence of bytes representing
   * a message to the client.
   */
  void write(DataBuffer* buffer);

  /** Write the data buffer. */
  void flush();

private:
  // This is the CRC-16 calculated on incoming data.
  unsigned short crc;

  // Buffer to read requests.
  DataBuffer* m_readBuffer;

  // Buffer to write responses.
  DataBuffer* m_writeBuffer;

  // Function to be called when a command is received.
  void (*callback)(DataBuffer*);

  // Escape bytes that are equal to the DELIMITER_FLAG or ESCAPE_FLAG.
  static void addByte(DataBuffer* buffer, byte ch);

  // Receiver current state.

  enum ReadingState
  {
    Waiting,
    ReadingMessage,
    ReadingEscapedByte
  };

  ReadingState m_state;
};

#endif  // SERIAL_PORT_H
