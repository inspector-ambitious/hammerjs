/*
 * Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef UString_h
#define UString_h

#include "config.h"
#include <dtoa.h>
#include <string.h>

#include <wtf/AlwaysInline.h>

namespace JSC {

class UString {
public:
    // Construct a null string, distinguishable from an empty string.
    UString()
        : m_length(0)
        , m_data(0)
    {
    }

    ~UString()
    {
        delete [] m_data;
    }

    UString(const UString &other)
        : m_length(other.m_length)
        , m_data(0)
    {
        m_length = other.m_length;
        if (m_length) {
            m_data = new UChar[m_length];
            memcpy(m_data, other.m_data, m_length * sizeof(UChar));
        }
    }

    UString& operator=(const UString &other)
    {
        if (&other != this) {
            delete [] m_data;
            m_data = 0;
            m_length = other.m_length;
            if (m_length) {
                m_data = new UChar[m_length];
                memcpy(m_data, other.m_data, m_length * sizeof(UChar));
            }
        }
        return *this;
    }

    // Construct a string with UTF-16 data.
    UString(const UChar* characters, unsigned length)
        : m_length(0)
        , m_data(0)
    {
        if (characters && length) {
            m_length = length;
            m_data = new UChar[length];
            memcpy(m_data, characters, length * sizeof(UChar));
        }
    }

    // Construct a string with latin1 data, from a null-terminated source.
    UString(const char* characters)
        : m_length(0)
        , m_data(0)
    {
        if (characters) {
            m_length = strlen(characters);
            m_data = new UChar[m_length];
            for (unsigned i = 0; i < m_length; ++i)
                m_data[i] = characters[i];
        }
    }

    // Construct a string with latin1 data.
    UString(const char* characters, unsigned length)
        : m_length(0)
        , m_data(0)
    {
        if (characters && length) {
            m_length = length;
            m_data = new UChar[m_length];
            for (unsigned i = 0; i < m_length; ++i)
                m_data[i] = characters[i];
        }
    }

    bool isEmpty() const { return !m_length || !m_data; }

    unsigned length() const { return m_length; }

    const UChar* characters() const { return m_data; }

    UChar operator[](unsigned index) const { return (!m_data || index >= m_length) ? 0 : m_data[index]; }

    static UString number(double d)
    {
        DtoaBuffer buffer;
        unsigned length;
        doubleToStringInJavaScriptFormat(d, buffer, &length);
        return UString(buffer, length);
    }

private:
    unsigned m_length;
    UChar* m_data;
};

ALWAYS_INLINE bool operator==(const UString& s1, const UString& s2)
{
    // If the lengths are not the same, we're done.
    if (s1.length() != s2.length())
        return false;

    // If they're the same rep, they're equal.
    if (s1.characters() == s2.characters())
        return true;

    // At this point we know
    //   (a) that the strings are the same length and
    //   (b) that they are greater than zero length.
    return memcmp(s1.characters(), s2.characters(), s1.length() * sizeof(UChar)) == 0;
}


inline bool operator!=(const UString& s1, const UString& s2)
{
    return !JSC::operator==(s1, s2);
}

bool operator==(const UString& s1, const char* s2);

inline bool operator!=(const UString& s1, const char* s2)
{
    return !JSC::operator==(s1, s2);
}

inline bool operator==(const char *s1, const UString& s2)
{
    return operator==(s2, s1);
}

inline bool operator!=(const char *s1, const UString& s2)
{
    return !JSC::operator==(s1, s2);
}

} // namespace JSC

#endif

