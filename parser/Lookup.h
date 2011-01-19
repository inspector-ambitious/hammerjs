/*
  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef Lookup_h
#define Lookup_h

#include "Identifier.h"
#include "JSParser.h"
#include "UString.h"
#include <wtf/Assertions.h>

namespace JSC {

const unsigned mainTable = 0xcafebabe;

class HashEntry
{
public:
    int value;
    HashEntry() {}
    int lexerValue() const { return value; }
};

class HashTable
{
public:
    HashTable(unsigned id)
    {
        if (id != 0xcafebabe)
            CRASH();
    }

    void deleteTable() const {}

    HashEntry* entry(JSGlobalData* globalData, const Identifier& id) const
    {
        m_entry.value = testKeyword(id.ustring().length(), id.ustring().characters());
        return &m_entry;
    }

private:
    mutable HashEntry m_entry;
    int testKeyword(int len, const UChar *str) const;
};

inline int HashTable::testKeyword(int len, const UChar* str) const
{
    switch (len) {
    case 2:
        if (str[0] == 'd' && str[1] == 'o')
            return DO;
        else if (str[0] == 'i' && str[1] == 'f')
            return IF;
        else if (str[0] == 'i' && str[1] == 'n')
            return INTOKEN;
        break;

    case 3:
        if (str[0] == 'f' && str[1] == 'o' && str[2] == 'r')
            return FOR;
        else if (str[0] == 'n' && str[1] == 'e' && str[2] == 'w')
            return NEW;
        else if (str[0] == 't' && str[1] == 'r' && str[2] == 'y')
            return TRY;
        else if (str[0] == 'v' && str[1] == 'a' && str[2] == 'r')
            return VAR;
        break;

    case 4:
        if (str[0] == 'c' && str[1] == 'a' && str[2] == 's' && str[3] == 'e')
            return CASE;
        else if (str[0] == 'e' && str[1] == 'l' && str[2] == 's' && str[3] == 'e')
            return ELSE;
        else if (str[0] == 'e' && str[1] == 'n' && str[2] == 'u' && str[3] == 'm')
            return RESERVED;
        else if (str[0] == 'n' && str[1] == 'u' && str[2] == 'l' && str[3] == 'l')
            return NULLTOKEN;
        else if (str[0] == 't' && str[1] == 'h' && str[2] == 'i' && str[3] == 's')
            return THISTOKEN;
        else if (str[0] == 't' && str[1] == 'r' && str[2] == 'u' && str[3] == 'e')
            return TRUETOKEN;
        else if (str[0] == 'v' && str[1] == 'o' && str[2] == 'i' && str[3] == 'd')
            return VOIDTOKEN;
        else if (str[0] == 'w' && str[1] == 'i' && str[2] == 't' && str[3] == 'h')
            return WITH;
        break;

    case 5:
        if (str[0] == 'b' && str[1] == 'r' && str[2] == 'e' && str[3] == 'a' && str[4] == 'k')
            return BREAK;
        else if (str[0] == 'c' && str[1] == 'a' && str[2] == 't' && str[3] == 'c' && str[4] == 'h')
            return CATCH;
        else if (str[0] == 'c' && str[1] == 'o' && str[2] == 'n' && str[3] == 's' && str[4] == 't')
            return CONSTTOKEN;
        else if (str[0] == 'f' && str[1] == 'a' && str[2] == 'l' && str[3] == 's' && str[4] == 'e')
            return FALSETOKEN;
        else if (str[0] == 't' && str[1] == 'h' && str[2] == 'r' && str[3] == 'o' && str[4] == 'w')
            return THROW;
        else if (str[0] == 'w' && str[1] == 'h' && str[2] == 'i' && str[3] == 'l' && str[4] == 'e')
            return WHILE;
        else if (str[0] == 'c' && str[1] == 'l' && str[2] == 'a' && str[3] == 's' && str[4] == 's')
            return RESERVED;
        else if (str[0] == 's' && str[1] == 'u' && str[2] == 'p' && str[3] == 'e' && str[4] == 'r')
            return RESERVED;
        break;

    case 6:
        if (str[0] == 'd' && str[1] == 'e' && str[2] == 'l' && str[3] == 'e' && str[4] == 't' && str[5]=='e')
            return DELETETOKEN;
        else if (str[0] == 's' && str[1] == 'w' && str[2] == 'i' && str[3] == 't' && str[4] == 'c' && str[5]=='h')
            return SWITCH;
        else if (str[0] == 'r' && str[1] == 'e' && str[2] == 't' && str[3] == 'u' && str[4] == 'r' && str[5]=='n')
            return RETURN;
        else if (str[0] == 't' && str[1] == 'y' && str[2] == 'p' && str[3] == 'e' && str[4] == 'o' && str[5]=='f')
            return TYPEOF;
        else if (str[0] == 'e' && str[1] == 'x' && str[2] == 'p' && str[3] == 'o' && str[4] == 'r' && str[5]=='t')
            return RESERVED;
        else if (str[0] == 'i' && str[1] == 'm' && str[2] == 'p' && str[3] == 'o' && str[4] == 'r' && str[5]=='t')
            return RESERVED;
        break;

    case 7:
        if (str[0] == 'd') {
            if (str[1]=='e')
                if (str[2]=='f')
                    if (str[3]=='a')
                        if (str[4]=='u')
                            if (str[5]=='l')
                                if (str[6]=='t')
                                    return DEFAULT;
        } else if (str[0] == 'f') {
            if (str[1]=='i')
                if (str[2]=='n')
                    if (str[3]=='a')
                        if (str[4]=='l')
                            if (str[5]=='l')
                                if (str[6]=='y')
                                    return FINALLY;
        } else if (str[0] == 'e') {
            if (str[1]=='x')
                if (str[2]=='t')
                    if (str[3]=='e')
                        if (str[4]=='n')
                            if (str[5]=='d')
                                if (str[6]=='s')
                                    return RESERVED;
        }
        break;

    case 8:
        if (str[0] == 'f') {
            if (str[1]=='u')
                if (str[2]=='n')
                    if (str[3]=='c')
                        if (str[4]=='t')
                            if (str[5]=='i')
                                if (str[6]=='o')
                                    if (str[7]=='n')
                                        return FUNCTION;
        } else if (str[0] == 'c') {
            if (str[1]=='o')
                if (str[2]=='n')
                    if (str[3]=='t')
                        if (str[4]=='i')
                            if (str[5]=='n')
                                if (str[6]=='u')
                                    if (str[7]=='e')
                                        return CONTINUE;
        } else if (str[0] == 'd') {
            if (str[1]=='e')
                if (str[2]=='b')
                    if (str[3]=='u')
                        if (str[4]=='g')
                            if (str[5]=='g')
                                if (str[6]=='e')
                                    if (str[7]=='r')
                                        return DEBUGGER;
        }
        break;

    case 10:
        if (str[0]=='i')
            if (str[1]=='n')
                if (str[2]=='s')
                    if (str[3]=='t')
                        if (str[4]=='a')
                            if (str[5]=='n')
                                if (str[6]=='c')
                                    if (str[7]=='e')
                                        if (str[8]=='o')
                                            if (str[9]=='f')
                                                return INSTANCEOF;
        break;

    default:
        break;
    }

    return IDENT;
}


} // namespace JSC

#endif // Lookup_h

