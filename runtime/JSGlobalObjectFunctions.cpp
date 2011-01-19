/*
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Cameron Zwarich (cwzwarich@uwaterloo.ca)
 *  Copyright (C) 2007 Maks Orlovich
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "JSGlobalObjectFunctions.h"

#include "dtoa.h"
#include <stdio.h>
#include <stdlib.h>
#include <wtf/ASCIICType.h>
#include <wtf/Assertions.h>
#include <wtf/Vector.h>

using namespace WTF;
using namespace Unicode;

namespace JSC {

// The trick is to define the NaN and Inf globals with a different type than the declaration.
// This trick works because the mangled name of the globals does not include the type, although
// I'm not sure that's guaranteed. There could be alignment issues with this, since arrays of
// characters don't necessarily need the same alignment doubles do, but for now it seems to work.
// It would be good to figure out a 100% clean way that still avoids code that runs at init time.

// Note, we have to use union to ensure alignment. Otherwise, NaN_Bytes can start anywhere,
// while NaN_double has to be 4-byte aligned for 32-bits.
// With -fstrict-aliasing enabled, unions are the only safe way to do type masquerading.
static const union {
    struct {
        unsigned char NaN_Bytes[8];
        unsigned char Inf_Bytes[8];
    } bytes;

    struct {
        double NaN_Double;
        double Inf_Double;
    } doubles;

} NaNInf = { {
#if CPU(BIG_ENDIAN)
    { 0x7f, 0xf8, 0, 0, 0, 0, 0, 0 },
    { 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 }
#elif CPU(MIDDLE_ENDIAN)
    { 0, 0, 0xf8, 0x7f, 0, 0, 0, 0 },
    { 0, 0, 0xf0, 0x7f, 0, 0, 0, 0 }
#else
    { 0, 0, 0, 0, 0, 0, 0xf8, 0x7f },
    { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f }
#endif
} } ;

const double NaN = NaNInf.doubles.NaN_Double;
const double Inf = NaNInf.doubles.Inf_Double;

bool isStrWhiteSpace(UChar c)
{
    switch (c) {
        // ECMA-262-5th 7.2 & 7.3
        case 0x0009:
        case 0x000A:
        case 0x000B:
        case 0x000C:
        case 0x000D:
        case 0x0020:
        case 0x00A0:
        case 0x2028:
        case 0x2029:
        case 0xFEFF:
            return true;
        default:
            return c > 0xff && isSeparatorSpace(c);
    }
}

static int parseDigit(unsigned short c, int radix)
{
    int digit = -1;

    if (c >= '0' && c <= '9')
        digit = c - '0';
    else if (c >= 'A' && c <= 'Z')
        digit = c - 'A' + 10;
    else if (c >= 'a' && c <= 'z')
        digit = c - 'a' + 10;

    if (digit >= radix)
        return -1;
    return digit;
}

double parseIntOverflow(const char* s, int length, int radix)
{
    double number = 0.0;
    double radixMultiplier = 1.0;

    for (const char* p = s + length - 1; p >= s; p--) {
        if (radixMultiplier == Inf) {
            if (*p != '0') {
                number = Inf;
                break;
            }
        } else {
            int digit = parseDigit(*p, radix);
            number += digit * radixMultiplier;
        }

        radixMultiplier *= radix;
    }

    return number;
}

double parseIntOverflow(const UChar* s, int length, int radix)
{
    double number = 0.0;
    double radixMultiplier = 1.0;

    for (const UChar* p = s + length - 1; p >= s; p--) {
        if (radixMultiplier == Inf) {
            if (*p != '0') {
                number = Inf;
                break;
            }
        } else {
            int digit = parseDigit(*p, radix);
            number += digit * radixMultiplier;
        }

        radixMultiplier *= radix;
    }

    return number;
}

static const int SizeOfInfinity = 8;

static bool isInfinity(const UChar* data, const UChar* end)
{
    return (end - data) >= SizeOfInfinity
        && data[0] == 'I'
        && data[1] == 'n'
        && data[2] == 'f'
        && data[3] == 'i'
        && data[4] == 'n'
        && data[5] == 'i'
        && data[6] == 't'
        && data[7] == 'y';
}

// See ecma-262 9.3.1
static double jsHexIntegerLiteral(const UChar*& data, const UChar* end)
{
    // Hex number.
    data += 2;
    const UChar* firstDigitPosition = data;
    double number = 0;
    while (true) {
        number = number * 16 + toASCIIHexValue(*data);
        ++data;
        if (data == end)
            break;
        if (!isASCIIHexDigit(*data))
            break;
    }
    if (number >= mantissaOverflowLowerBound)
        number = parseIntOverflow(firstDigitPosition, data - firstDigitPosition, 16);

    return number;
}

// See ecma-262 9.3.1
static double jsStrDecimalLiteral(const UChar*& data, const UChar* end)
{
    ASSERT(data < end);

    // Copy the sting into a null-terminated byte buffer, and call strtod.
    Vector<char, 32> byteBuffer;
    for (const UChar* characters = data; characters < end; ++characters) {
        UChar character = *characters;
        byteBuffer.append(isASCII(character) ? character : 0);
    }
    byteBuffer.append(0);
    char* endOfNumber;
    double number = WTF::strtod(byteBuffer.data(), &endOfNumber);

    // Check if strtod found a number; if so return it.
    ptrdiff_t consumed = endOfNumber - byteBuffer.data();
    if (consumed) {
        data += consumed;
        return number;
    }

    // Check for [+-]?Infinity
    switch (*data) {
    case 'I':
        if (isInfinity(data, end)) {
            data += SizeOfInfinity;
            return Inf;
        }
        break;

    case '+':
        if (isInfinity(data + 1, end)) {
            data += SizeOfInfinity + 1;
            return Inf;
        }
        break;

    case '-':
        if (isInfinity(data + 1, end)) {
            data += SizeOfInfinity + 1;
            return -Inf;
        }
        break;
    }

    // Not a number.
    return NaN;
}

// See ecma-262 9.3.1
double jsToNumber(const UString& s)
{
    unsigned size = s.length();

    if (size == 1) {
        UChar c = s.characters()[0];
        if (isASCIIDigit(c))
            return c - '0';
        if (isStrWhiteSpace(c))
            return 0;
        return NaN;
    }

    const UChar* data = s.characters();
    const UChar* end = data + size;

    // Skip leading white space.
    for (; data < end; ++data) {
        if (!isStrWhiteSpace(*data))
            break;
    }

    // Empty string.
    if (data == end)
        return 0.0;

    double number;
    if (data[0] == '0' && data + 2 < end && (data[1] | 0x20) == 'x' && isASCIIHexDigit(data[2]))
        number = jsHexIntegerLiteral(data, end);
    else
        number = jsStrDecimalLiteral(data, end);

    // Allow trailing white space.
    for (; data < end; ++data) {
        if (!isStrWhiteSpace(*data))
            break;
    }
    if (data != end)
        return NaN;

    return number;
}

} // namespace JSC
