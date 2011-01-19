/*
 *  Copyright (C) 2003, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef Identifier_h
#define Identifier_h

#include "UString.h"

namespace JSC {

    class JSGlobalData;
    class UString;

    class Identifier {
    public:

        Identifier() { }
        Identifier(JSGlobalData*, const UChar* chars, size_t len): m_string(chars, len) { }
        Identifier(JSGlobalData*, const UString &id): m_string(id) { }

        const UString& ustring() const { return m_string; }
        const UString& impl() const { return m_string; }

        const UChar* characters() const { return m_string.characters(); }
        int length() const { return m_string.length(); }

        static bool equal(const Identifier& a, const Identifier& b) { return a.m_string == b.m_string; }

    private:
        UString m_string;
    };

    inline bool operator==(const Identifier& a, const Identifier& b)
    {
        return Identifier::equal(a, b);
    }

    inline bool operator!=(const Identifier& a, const Identifier& b)
    {
        return !Identifier::equal(a, b);
    }

} // namespace JSC

#endif // Identifier_h
