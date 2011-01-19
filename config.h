/*
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef config_h
#define config_h

#include <stdio.h>
#include <stdlib.h>
#include <wtf/Assertions.h>
#include <wtf/Platform.h>

// We do not support Unicode
namespace WTF {
namespace Unicode {

enum {
    Mark_NonSpacing,
    Mark_SpacingCombining,
    Number_DecimalDigit,
    Letter_Uppercase,
    Letter_Lowercase,
    Letter_Titlecase,
    Letter_Modifier,
    Letter_Other,
    Punctuation_Connector,
};

int category(int ch);
bool isSeparatorSpace(int ch);

inline int category(int ch)
{
    return 0;
}

inline bool isSeparatorSpace(int ch)
{
    return false;
}

} // namespace Unicode
} // namespace WTF

#endif
