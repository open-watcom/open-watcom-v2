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


#define CR      0x0d
#define LF      0x0a
#define NC      0x00

#include "wfile.hpp"

extern "C" {
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include "wio.h"
};

#define MAX_FORMATTED 500
#define MAX_BUFFER 1000

const Handle FILE_ERROR = -1;

WEXPORT WFile::WFile()
    : _handle( FILE_ERROR )
    , _eof( true )
    , _ok( false )
    , _chsaved( false )
    , _buffer( NULL )
    , _bOffset( 0 )
    , _bLength( 0 )
    , _lineCount( 0 )
{
}

WEXPORT WFile::~WFile()
{
    freeBuffer();
}

bool WEXPORT WFile::open( const char* name, OpenStyle style )
{
    _handle = ::open( name, style, PMODE_RW );
    _style = style;
    _ok = ( _handle != FILE_ERROR );
    _eof = !_ok;
    _chsaved = false;
    _lineCount = 0;
    if( _ok ) {
        _filename = name;
        _filename.absoluteTo();
    }
    return( _ok );
}

bool WEXPORT WFile::open( const char* name, const char* env, OpenStyle style )
{
    char buff[_MAX_PATH];
    _searchenv( name, env, buff );
    return( open( buff, style ) );
}

bool WEXPORT WFile::close()
{
    _filename = "";
    freeBuffer();
    _eof = true;
    Handle code = ::close( _handle );
    if( code == FILE_ERROR ) {
        _ok = false;
        return( false );
    }
    return( true );
}

int WEXPORT WFile::read( char* buffer, int length )
{
    return( ::read( _handle, buffer, length ) );
}

int WEXPORT WFile:: write( const char* buffer, int length )
{
    return( ::write( _handle, (void*)buffer, length ) );
}

long WEXPORT WFile::lseek( long offset, int org )
{
    return( ::lseek( _handle, offset, org ) );
}

long WEXPORT WFile::getl()
{
    char temp[30];
    int len = 0;
    for( unsigned i = 0; !_eof && i <= sizeof( temp ); i++ ) {
        temp[i] = getch();
        len = i;
        if( !( isdigit( temp[i] ) || temp[i] == '-' ) ) {
            ungetch( temp[i] );
            break;
        }
    }
    temp[len] = '\0';
    return( atol( temp ) );
}

WString& WEXPORT WFile::getLine( WString& str )
{
    for(;!_eof;) {
        char ch = getch();
        if( ch == CR || ch == LF ) {
            break;
        }
        str.concat( ch );
    }
    return( str );
}

WString& WEXPORT WFile::gets( WString& str )
{
    for(;!_eof;) {
        char ch = getch();
        if( ch == CR || ch == LF || ch == NC ) {
            ungetch( ch );
            break;
        }
        str.concat( ch );
    }
    return( str );
}

void WEXPORT WFile::gets( char* str, size_t len )
{
    size_t  i;

    for( i=0; !_eof && i<len; i++ ) {
        str[i] = getch();
        if( str[i] == CR || str[i] == LF || str[i] == NC ) {
            ungetch( str[i] );
            break;
        }
    }
    str[i] = '\0';
}

void WEXPORT WFile::gets_exact( char* str, size_t len )
{
    size_t  i;

    // read exactly len bytes -- don't stop for separator characters ( CR/LF )
    for( i=0; !_eof && i<len; i++ ) {
        str[i] = getch();
        if( str[i] == NC ) {
            ungetch( str[i] );
            break;
        }
    }
    str[i] = '\0';
}

char WEXPORT WFile::getch()
{
    char chr;
    if( _chsaved ) {
        chr = _chsave;
        _chsaved = false;
    } else {
        chr = getByte();
    }
    if( chr == LF ) {
        _lineCount++;
    }
    return( chr );
}

void WEXPORT WFile::ungetch( char chr )
{
    _chsaved = true;
    _chsave = chr;
    if( chr == LF ) {
        _lineCount--;
    }
}

bool WEXPORT WFile::putl( long n )
{
    char temp[11];
    ltoa( n, temp, 10 );
    return( puts( temp ) );
}

bool WEXPORT WFile::puts( const char* str )
{
    if( str != NULL ) {
        return( putBytes( str, strlen( str ) ) );
    }
    return( true );
}

bool WEXPORT WFile::printf( const char* parms... )
{
    char* buffer = new char[MAX_FORMATTED+1];
    va_list args;
    va_start( args, parms );
    vsprintf( buffer, parms, args );
    bool ok = putBytes( buffer, strlen( buffer ) );
    delete [] buffer;
    return( ok );
}

bool WEXPORT WFile::putch( char ch )
{
    return( putByte( ch ) );
}

void WFile::freeBuffer()
{
    // flush buffer if writing
    if( _buffer != NULL ) {
        if( _bOffset > 0 && (_style & OStyleWrite) ) {
            write( _buffer, _bOffset );
        }
        delete [] _buffer;
        _buffer = NULL;
    }
}

char WFile::getByte()
{
    if( !_ok ) {
        return( '\0' );
    }
    if( _buffer == NULL ) {
        _buffer = new char[MAX_BUFFER];
        _bOffset = _bLength = 0;
    }
    if( _bOffset >= _bLength ) {
        _bLength = read( _buffer, MAX_BUFFER );
        _bOffset = 0;
        if( _bLength <= 0 ) {
            _eof = true;
            return( '\0' );
        }
    }
    return( _buffer[_bOffset++] );
}

bool WFile::putBytes( const char* str, size_t len )
{
    if( str != NULL ) {
        for( size_t i=0; i<len; i++ ) {
            if( !putByte( str[i] ) ) {
                return( false );
            }
        }
    }
    return( true );
}

bool WFile::putByte( char ch )
{
    if( _ok ) {
        if( _buffer == NULL ) {
            _buffer = new char[MAX_BUFFER];
            _bOffset = 0;
        }
        _buffer[_bOffset++] = ch;
        if( _bOffset >= MAX_BUFFER ) {
            unsigned len = write( _buffer, _bOffset );
            if( len != _bOffset ) {
                _ok = false;
            }
            _bOffset = 0;
        }
    }
    return( _ok );
}
