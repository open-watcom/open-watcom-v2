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


#include <assert.h>
#include <wcvector.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>  // error reporting via printf
#include <stdlib.h>

#include "chbffile.h"
#include "scanner.h"
#include "prsbnd.gh"
#include "prsdlg.gh"

static const char * const   Scanner::_SpecialCharacters = ",{}|:()@#;";

struct TokenStruct {
    const char * name;
    short        token; // token value associated with this
};

#define DEFTOKEN( n, val )  { n, val },

TokenStruct Tokens[] = {
    #include "deftoken.h"
};


Scanner::Scanner( const char * fileName )
//---------------------------------------
{
    _file = new CheckedBufferedFile( fileName );
    _strings = new WCPtrOrderedVector<char>;
    _identifiers = new WCPtrOrderedVector<char>;

    _file->open( CheckedFile::ReadText, CheckedFile::UserReadWrite );

    _current = ' ';
}

Scanner::~Scanner()
//-----------------
{
    delete _file;
}

const char * Scanner::getString( int idx )
//----------------------------------------
{
    return (*_strings)[ idx ];
}

const char * Scanner::getIdent( int idx )
//---------------------------------------
{
    return (*_identifiers)[ idx ];
}

bool Scanner::error( char const * errStr )
//----------------------------------------
{
    const int BufferSize = 80;
    char buffer[ BufferSize ];
    int  nRead;

    nRead = _file->read( buffer, BufferSize );

    buffer[ nRead - 1 ] = '\0';

    fprintf( stderr, "%s: %s\n", _file->getFileName(), errStr );
    fprintf( stderr, "near <%s>\n", buffer );

    return FALSE;       // don't continue
}

int Scanner::get()
//----------------
{
    int     numRead;
    char    c;

    numRead = _file->read( &c, sizeof( char ) );

    if( numRead != sizeof( char ) ) {
        _current = EOF;
    } else {
        _current = c;
    }

    return _current;
}

void Scanner::gobble()
//--------------------
// eat whitespaces. When done,
// _current contains a non-white space
{
    while( isspace( _current ) ) {
        get();
    }
}

bool Scanner::isSpecial()
//-----------------------
{
    return ( strchr( _SpecialCharacters, _current ) != NULL );
}

bool Scanner::isSpace()
//---------------------
{
    return (bool) isspace( _current );
}

bool Scanner::isQuote()
//---------------------
{
    return _current == '"';
}

bool Scanner::isEOF()
//-------------------
{
    return _current == EOF;
}

bool Scanner::isDigit()
//---------------------
{
    return (bool) isdigit( _current );
}

bool Scanner::isHexDigit()
//------------------------
{
    return (bool) isxdigit( _current );
}

void Scanner::readDecimal( YYSTYPE & lval )
//-----------------------------------------
{
    const int   MaxBufLen = 10;
    char        buffer[ MaxBufLen ];
    int         bufPos;         // position in buffer

    for( bufPos = 0; bufPos < MaxBufLen; bufPos += 1 ) {
        if( isEOF() || !isDigit() ) break;
        buffer[ bufPos ] = (char) _current;
        get();
    }

    assert( bufPos < MaxBufLen );

    buffer[ bufPos ] = '\0';
    lval = (YYSTYPE) atoi( buffer );
}

void Scanner::readHex( YYSTYPE & lval )
//-------------------------------------
{
    const int   MaxBufLen = 10;
    char        buffer[ MaxBufLen ];
    int         bufPos;         // position in buffer

    get();  // move past x (ie. 0x7f)

    for( bufPos = 0; bufPos < MaxBufLen; bufPos += 1 ) {
        if( isEOF() || !isHexDigit() ) break;
        buffer[ bufPos ] = (char) _current;
        get();
    }

    assert( bufPos < MaxBufLen );

    buffer[ bufPos ] = '\0';
    lval = (YYSTYPE) strtol( buffer, NULL, 16 );
}

void Scanner::readQuotedString( YYSTYPE & lval )
//----------------------------------------------
{
    const int   MaxBufLen = 512;
    char        buffer[ MaxBufLen ];
    int         bufPos;         // position in buffer
    char *      dupStr;

    get();  // move past "

    for( bufPos = 0; bufPos < MaxBufLen; bufPos += 1 ) {
        if( isQuote() || isEOF() ) break;
        buffer[ bufPos ] = (char) _current;
        get();
    }

    if( isQuote() ) {
        get();  // eat the quote
    }

    assert( bufPos < MaxBufLen );

    buffer[ bufPos ] = '\0';

    dupStr = new char[ strlen( buffer ) + 1 ];
    strcpy( dupStr, buffer );

    lval =  (YYSTYPE) _strings->entries();
    _strings->append( dupStr );
}

short Scanner::getToken( YYSTYPE & lval )
//---------------------------------------
// get the next token from the input stream.
// at each call, the next character should be
// in _current.
{
    const int   MaxBufLen = 512;
    char        buffer[ MaxBufLen ];
    int         bufPos;                 // position in buffer
    char        special;

    gobble();

    if( isEOF() ) {
        return 0;
    }

    if( isSpecial() ) {
        special = (char) _current;
        get();                      // set up for next call
        return special;   // <-------- early return
    }

    if( isQuote() ) {
        readQuotedString( lval );
        return T_String;   // <-------- early return
    }

    if( _current == '0' ) {
        get();
        if( toupper( _current ) == 'X' ) {
            readHex( lval );
        } else {
            if( isDigit() ) {
                readDecimal( lval );
            } else {
                lval = 0;
            }
        }
        return T_Number;   // <-------- early return
    }

    if( isDigit() ) {
        readDecimal( lval );
        return T_Number;   // <-------- early return
    }

    for( bufPos = 0; bufPos < MaxBufLen; bufPos += 1 ) {
        buffer[ bufPos ] = (char) _current;

        get();
        if( isEOF() || isSpace() || isSpecial() ) break;
    }

    bufPos += 1;

    assert( bufPos < MaxBufLen );

    buffer[ bufPos ] = '\0';

    return tokenValue( buffer, lval );
}

static int CompareTokens( const void * lhs, const void * rhs )
//------------------------------------------------------------
{
    return strcmp( (const char *) lhs, ((const TokenStruct *)rhs)->name );
}

short Scanner::tokenValue( const char * tok, YYSTYPE & lval )
//-----------------------------------------------------------
{
    TokenStruct * res;
    char *  dupStr;

    res = (TokenStruct *) bsearch( tok, Tokens, sizeof( Tokens ) / sizeof( TokenStruct ), sizeof( TokenStruct ), &CompareTokens );

    if( res ) {
        return res->token;
    } else {
        dupStr = new char[ strlen( tok ) + 1 ];
        strcpy( dupStr, tok );

        lval =  (YYSTYPE) _identifiers->entries();
        _identifiers->append( dupStr );

        return T_Ident;
    }
}
