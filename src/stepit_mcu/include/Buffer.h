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

#ifndef BUFFER_H
#define BUFFER_H

enum class BufferPosition
{
  Head,
  Tail
};

/**
 * This template implements a circular buffer. Items can be added and removed
 * to and from both sides of the buffer.
 */
template <typename T>
class Buffer
{
public:
  explicit Buffer(size_t capacity);
  ~Buffer();

  // Return the maximum capacity of the buffer.
  size_t capacity() const;

  // Return how much data is in the buffer.
  size_t size() const;

  // Insert an item at the given position.
  void add(T in, BufferPosition position);

  // Remove an item from the given position.
  // Removing an item from an empty buffer leads to undefined behavior.
  T remove(BufferPosition position);

  // Reset the buffer.
  void clear();

private:
  T* m_data;

  // The maximum capacity of the buffer.
  size_t m_capacity = 0;

  // "m_position" is the position of the first element of the buffer.
  size_t m_position = 0;

  // This records how much data is in the buffer: it is always greater
  // than zero and less than or equal to the capacity.
  // "m_position + m_size - 1" is the position of the last element of the
  // buffer.
  size_t m_size = 0;
};

template <typename T>
Buffer<T>::Buffer(size_t capacity) : m_data{ new T[capacity] }, m_capacity{ capacity }, m_position{ 0 }, m_size{ 0 }
{
}

template <typename T>
Buffer<T>::~Buffer()
{
  delete[] m_data;
}

template <typename T>
size_t Buffer<T>::size() const
{
  return m_size;
}

template <typename T>
size_t Buffer<T>::capacity() const
{
  return m_capacity;
}

template <typename T>
void Buffer<T>::clear()
{
  m_position = 0;
  m_size = 0;
}

template <typename T>
void Buffer<T>::add(T in, BufferPosition position)
{
  // Please note that there is no exception thrown in integer arithmetic
  // overflow or underflow.

  if (m_size < m_capacity)
  {
    if (position == BufferPosition::Tail)
    {
      // Add an item to the end of the buffer.
      m_data[(m_position + m_size) % m_capacity] = in;
    }
    else
    {
      // Add an item to the front of the buffer.
      if (m_position == 0)
      {
        m_position = m_capacity - 1;
      }
      else
      {
        m_position--;
      }
      m_data[m_position] = in;
    }
    m_size++;
  }
}

template <typename T>
T Buffer<T>::remove(BufferPosition position)
{
  // Please note that there is no exception thrown in integer arithmetic
  // overflow or underflow.

  T out;
  if (m_size > 0)
  {
    if (position == BufferPosition::Tail)
    {
      // Remove an item from the end of the buffer.
      out = m_data[(m_position + m_size - 1) % m_capacity];
    }
    else
    {
      // Remove an item from the front of the buffer.
      out = m_data[m_position];
      m_position = (m_position + 1) % m_capacity;
    }
    m_size--;
  }
  else
  {
    out = T{};  // This never happens.
  }
  return out;
}

#endif  // BUFFER_H
