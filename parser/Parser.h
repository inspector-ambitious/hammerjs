/*
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
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

#ifndef Parser_h
#define Parser_h

#include "Nodes.h"
#include "Lexer.h"
#include "ParserArena.h"
#include "SourceProvider.h"
#include "UString.h"
#include <wtf/Noncopyable.h>

namespace JSC {

    class FunctionBodyNode;
    
    class ProgramNode;
    class ScopeNode;
    class SourceElements;

    class Parser : public Noncopyable {
    public:

        UString createSyntaxTree(JSGlobalData* globalData, const SourceCode& m_source, int* errLine = 0, UString* errMsg = 0);

        ParserArena& arena() { return m_arena; }

    private:

        // Used to determine type of error to report.
        bool isFunctionBodyNode(ScopeNode*) { return false; }
        bool isFunctionBodyNode(FunctionBodyNode*) { return true; }

        ParserArena m_arena;
        const SourceCode* m_source;
        SourceElements* m_sourceElements;
        int m_lastLine;
        int m_numConstants;
    };

    class PropertyNode {
    public:
        enum Type { Constant = 1, Getter = 2, Setter = 4 };
    };

} // namespace JSC

#endif // Parser_h
