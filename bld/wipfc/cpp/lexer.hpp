/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  the IPF lexer
*
****************************************************************************/

#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include <map>
#include <string>

class IpfData;  //forward reference

class Lexer {
public:
    enum Token {
        END,            //end of file or buffer
        WHITESPACE,     //[[:blank:]]
        WORD,           //[[:alnum:]]+
        ENTITY,         //"&"[[:alpha:]]+"."
        PUNCTUATION,    //[^&\.:[:alnum:][:space:][:cntrl:]]
        COMMAND,        //(^".*") | (^"."[[:alpha:]]+)
        TAG,            //":"[[:alpha:]][[:alnum:]]+
        TAGEND,         //<INTAG>"."
        FLAG,           //<INTAG>[[:alpha:]]+
        ATTRIBUTE,      //(<INTAG>[[:alpha:]]+"="{word}) |
                        //<INTAG>[[:alpha:]]+"="{cpw}
                        //cpw {word}({punct}{word})+{punct}?
        ERROR_TAG,      //Tag inside another tag's definition
        ERROR_ENTITY    //malformed entity
    };
    enum CmdId {
        COMMENT,
        BREAK,
        CENTER,
        IMBED,
        NAMEIT,
        BADCMD
    };
    enum TagId {
        #undef PICK
        #define PICK(a,b) a,
        #include "tags.hpp"
        BADTAG
    };
    Lexer();
    ~Lexer() { };
    //get the next token from the input stream
    Token lex( IpfData* input );
    //get the actual text associated with the token
    const std::wstring& text() const { return buffer; };
    //parsing inside a tag
    bool isInTag() const { return inTag; };
    //which tag?
    TagId tagId() const { return tagCode; };
    //which command?
    CmdId cmdId() const { return cmdCode; };
    //get the position at the start of the current token
    unsigned int currentLine() const { return lineNum; };
    unsigned int currentCol() const { return charNum; };
private:
    Lexer( const Lexer& rhs );              //no copy
    Lexer& operator=( const Lexer& rhs );   //no assignment
    std::wstring buffer;
    std::map< std::wstring, TagId > tagIdMap;
    typedef std::map< std::wstring, TagId >::iterator TagIdMapIter;
    typedef std::map< std::wstring, TagId >::const_iterator ConstTagIdMapIter;
    unsigned int charNum;
    unsigned int lineNum;
    TagId tagCode;
    CmdId cmdCode;
    bool inTag;         //we are between ':' and '.' inside a tag
    //if the token is a tag, find the id
    void getTagId();
    //if the token is a command, find the id
    void getCmdId();
};

#endif //LEXER_INCLUDED
