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


#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include <wstring.hpp>
#include "assure.h"
#include "opstream.h"
#include "util.h"

const char * PathDelim = "\\/";
const char * PathSeperator = ";";

StringList ScanStreamBase::_searchPaths;

ScanStreamBase::ScanStreamBase( void )
            : _currPos( 0 )
            , _endPos( 0 )
            , _usePrev( FALSE )
            , _eof( FALSE )
            , _error( FALSE )
            , _line( 1 )
//------------------------------------
{
    _buffer[ 0 ] = '\0';
    _buffer[ SECTOR_SIZE ] = '\0';
}

char * ScanStreamBase::name( void )
//---------------------------------
{
    return NULL;
}

int ScanStreamBase::get( void )
//-----------------------------
{
    REQUIRE( !_error, "ScanStreamBase::get with _error ! " );
    if( _eof ) {
        return EOF;
    }
    if( _usePrev ) {
        _usePrev = FALSE;
        _eof = _prevEOF;
        return _prevChar;
    }

    int ret = _buffer[ _currPos ];

    _currPos += 1;
    if( _currPos > _endPos ) {
        refill();
    }

    if( ret == '\n' ) {
        _line += 1;
    }

    return ret;
}

void ScanStreamBase::unget( int c )
//---------------------------------
{
    REQUIRE( !_error, "ScanStreamBase::unget with _error ! " );
    _prevEOF = _eof;
    if( c != EOF ) {
        _eof = FALSE;
    }

    if( c == '\n' ) {
        _line -= 1;
    }

    _prevChar = c;
    _usePrev = TRUE;
};

void ScanStreamBase::refill( void )
//---------------------------------
{
    REQUIRE( !_error, "ScanStreamBase::refill with _error ! " );
}

bool ScanStreamBase::error( void )
//--------------------------------
{
    return _error;
}

void ScanStreamBase::streamInit( const char * srchPath )
//------------------------------------------------------
{
    if( srchPath != NULL ) {
        char * buffer = strdup( srchPath );
        char * path;

        path = strtok( buffer, PathSeperator );
        do {
            _searchPaths.add( path );
            path = strtok( NULL, PathSeperator );
        } while( path != NULL );
    }
}

ScanStreamFile::ScanStreamFile( const char * fname )
                : ScanStreamBase()
                , _fileName( fname )
//--------------------------------------------------
{
    char buffer[ _MAX_PATH ];

    _handle = open( fname, O_TEXT | O_RDONLY );
    for( int i = 0; i < _searchPaths.count() &&
                    _handle < 0 && errno == ENOENT; i += 1 ) {

        strcpy( buffer, _searchPaths[ i ] );

        char c = buffer[ strlen( buffer ) - 1 ];
        if( !strchr( PathDelim, c ) ) {
            strcat( buffer, "/" );
        }

        strcat( buffer, fname );
        _handle = open( buffer, O_TEXT | O_RDONLY );
    }

    if( _handle < 0 ) {
        if( errno != ENOENT ) {
            errMessage( "ScanStream::Streamfile - [%s]: %s", fname, strerror( errno ) );
        } else {
            _error = TRUE;
        }
    } else {
        _endPos = SECTOR_SIZE;      // so refill won't think eof.
        refill();
    }
}

ScanStreamFile::~ScanStreamFile()
//-------------------------------
{
    if( _handle >= 0 ) {
        if( close( _handle ) < 0 ) {
            errMessage( "ScanStream::~Streamfile: %s", strerror( errno ) );
            throw DEATH_BY_FILE_READING;
        }
    }
}

char * ScanStreamFile::name( void )
//---------------------------------
{
    WString val;

    val.printf( "file \"%s\" (line %d)", _fileName, _line );
    return WBRStrDup( val );
}

void ScanStreamFile::refill( void )
//---------------------------------
{
    if( _handle >= 0 ) {
        if( _eof ) return;
        if( _endPos + 1 < SECTOR_SIZE ) {
            _eof = TRUE;
            return;
        }

        _currPos = 0;

        _endPos = read( _handle, _buffer, SECTOR_SIZE );
        if( _endPos < 0 ) {
            errMessage( "ScanStream::~Streamfile: %s", strerror( errno ) );
            throw DEATH_BY_BAD_FILE;
        }

        _endPos -= 1;
    }
}

ScanStreamMem::ScanStreamMem( char * mem, int len )
                : ScanStreamBase()
                , _offset( 0 )
                , _end( len )
                , _memPtr( mem )
//-------------------------------------------------
{
    if( mem == NULL ) {
        _error = TRUE;
    } else {
        refill();
    }
}

void ScanStreamMem::refill( void )
//--------------------------------
{
    if( _eof || _offset >= _end ) {
        _eof = TRUE;
        return;
    }

    strncpy( _buffer, _memPtr + _offset, SECTOR_SIZE );
    _offset += SECTOR_SIZE;
    if( _offset > _end ) {
        _endPos = strlen( _buffer );
    } else {
        _endPos = SECTOR_SIZE;
    }
    _currPos = 0;
}

char * ScanStreamMem::name( void )
//--------------------------------
{
    return WBRStrDup( "command line" );
}
