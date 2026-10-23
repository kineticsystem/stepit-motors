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

#ifndef CRC_UTILS_H
#define CRC_UTILS_H

/**
 * This utility provides methods to compute CRC-16 (Cyclic Redundancy Check) on
 * a sequence of bytes.
 * It implements an algorithm called CCITT CRC-16 (Kermit).
 * Please note that the calculated CRC is given in Little Endian Form and the
 * two bytes produced must be inverted before sending the CRC-16 to the serial
 * port.
 *
 * A complete catalog of parametrized CRC algorithms with 16 bits is available at
 * http://reveng.sourceforge.net/crc-catalogue/16.htm
 *
 * A CRC online calculator useful for testing is available at
 * http://www.lammertbies.nl/comm/info/crc-calculation.html
 *
 * For a sample implementation of different CRC-16 see
 * http://www.lammertbies.nl/comm/software/
 */
namespace crc_utils
{
/**
 * This method updates the given CRC adding a new byte to the original
 * sequence of bytes where the given CRC was computed.
 * The CCITT CRC-16 (Kermit) requires to pass an initial CRC equal to 0x0000
 * the first time the method is invoked.
 * Please note that the calculated CRC is given in Little Endian Form.
 * @param crc The CRC value to update.
 * @param ch The byte to be added to the original sequence of bytes where
 *     the given CRC was computed.
 * @return The new CRC computed on the full sequence of bytes.
 */
unsigned short crc_ccitt_byte(unsigned short crc, unsigned char ch);

/**
 * This method calculates the CRC on the given sequence bytes and length.
 * Please note that the calculated CRC is given in Little Endian Form.
 * @param buffer The sequence of bytes to calculate the CRC.
 * @param length The sequence length in a number of bytes.
 * @return The CRC calculated on the given sequence of bytes.
 */
unsigned short crc_ccitt(const unsigned char* buffer, int length);
};  // namespace crc_utils

#endif  // CRC_UTILS_H
