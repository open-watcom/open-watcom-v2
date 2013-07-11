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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
//  SCANNING.H      Class to scan .RTF files.
//          For the WATCOM .HLP compiler.
//

#ifndef SCANNING_H
#define SCANNING_H

#include "myfile.h"
#include "hcmem.h"


//  TokenTypes  --List of possible tokens.

enum TokenTypes {   TOK_PUSH_STATE,
                TOK_POP_STATE,
            TOK_COMMAND,
            TOK_SPEC_CHAR,
            TOK_TEXT,
            TOK_NONE,
            TOK_END
};


//
//  Token   --Class to represent a token from a .RTF file.
//

struct Token
{
    TokenTypes      _type;
    Buffer<char>    _text;
    int             _hasValue;
    int             _value;
    int             _lineNum;
    Token();
};


//
//  Scanner --Class to implement the scanner.
//

class Scanner
{
    InFile  *_source;
    int     _lineNum;

    // A buffer to provide a little extra lookahead.
    Buffer<uint_8> _buffer;
    int         _maxBuf;
    int         _curPos;

    int     nextch();
    void    putback( int c );


    // Various helper functions to handle specific cases.
    TokenTypes  handleSlash( Token * tok );
    int     isSpecial( int c );
    void    pullCommand( Token * tok );
    void    pullText( Token * tok );
    void    pullHex( Token * tok );
    void    getToken( Token * tok );

    // The "lookahead" buffer for tokens.
    Token   *tokens[3];

    // Assignment of Scanner's is not allowed.
    Scanner( Scanner const & ) : _buffer(0) {};
    Scanner &   operator=( Scanner const & ){ return *this; };

public:
    Scanner( InFile *src );
    ~Scanner();

    // Return the next token.
    Token   *next();

    // Look ahead to an upcoming token.
    Token   *look( int i ) { return i<3?tokens[i]:NULL; };

    // Check if a certain character can start a .HLP "footnote".
    int     isFootnoteChar( int c );

    // Access function.
    InFile  const *file() { return _source; };
};

#endif
