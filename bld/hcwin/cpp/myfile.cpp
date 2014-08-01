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


/*
MYFILE:  Special purpose file handling
*/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "myfile.h"
#include "clibext.h"

const uint_8 File::_isOpen = 0x10;

File::File( char const filename[], uint_8 type )
// Open the file and record the name.
{
    char    mode[3] = "rb";
    size_t  length = strlen( filename );

    _shortName = new char[length + 1];
    strncpy( _shortName, filename, length + 1 );

    if( type & WRITE ){
        mode[0] = 'w';
    }
    if( type & TEXT ){
        mode[1] = 't';
    }
    _fp = fopen( filename, mode );
    _badFile = ( _fp == NULL );
    if( !_badFile ){
        _flags = type;
        _fullName = new char[ _MAX_PATH ];
        _fullpath( _fullName, filename, _MAX_PATH );
        _flags |= _isOpen;
    } else {
        _fullName = NULL;
    }
}

File::File()
    : _flags( 0 ),
      _fp( NULL ),
      _badFile( 1 ),
      _fullName( NULL ),
      _shortName( NULL )
{
    // empty
}

int File::open( char const filename[], uint_8 type )
// Open the file and record the name.
{
    char mode[3] = "rb";
    size_t length = strlen( filename );

    if( _shortName != NULL ){
        renew( _shortName, length + 1 );
    } else {
        _shortName = new char[length + 1];
    }
    strncpy( _shortName, filename, length + 1 );

    if( type & WRITE ){
        mode[0] = 'w';
    }
    if( type & TEXT ){
        mode[1] = 't';
    }
    if( _fp )
        fclose( _fp );
    _fp = fopen( filename, mode );
    _badFile = ( _fp == NULL );
    if( !_badFile ){
        _flags = type;
        if( !_fullName ){
            _fullName = new char[ _MAX_PATH ];
        }
        _fullpath( _fullName, filename, _MAX_PATH );
        _flags |= _isOpen;
    } else {
        if( _fullName ){
            delete[] _fullName;
            _fullName = NULL;
        }
    }

    return _badFile;
}

File::~File()
// Free memory and close the file if necessary.
{
    if( _fullName ) delete[] _fullName;
    if( _shortName ) delete[] _shortName;
    if( _fp ) fclose( _fp );
}

int File::open()
// Reopen a file closed with the close() function.
{
    if( !( _flags & _isOpen ) ){
    char mode[3] = "rb";
    if( _flags & TEXT ){
        mode[1] = 't';
    }
    if( _flags & WRITE ){
        mode[0] = 'w';
    }
    _fp = fopen( _fullName, mode );
    if( _fp != NULL ){
        _flags |= _isOpen;
    }
    }
    return _fp != NULL;
}

void File::close()
// Shut down a file (to preserve file handles when necessary)
{
    if( _flags & _isOpen ){
    fclose( _fp );
    _fp = NULL;
    _flags ^= _isOpen;
    }
}

InFile::InFile( char const filename[], int is_binary )
    : File( filename, is_binary?(READ|BIN):(READ|TEXT) )
{
    // empty;
}

int InFile::open( char const filename[], int is_binary )
{
    return File::open( filename, is_binary?(READ|BIN):(READ|TEXT) );
}


//  Hash    --Implements the WinHelp context string hashing function.

extern uint_32 Hash( char const *str )
{
    uint_32 result = 0;
    char increment;
    for( int i=0; str[i] != '\0'; i++ ){
    if( isalpha( str[i] ) ){
        increment = (char) (tolower( str[i] ) - 'a' + 17);
    } else if( isdigit( str[i] ) ){
        increment = (char) ( str[i] - '1' + 1 );
        if( !increment ) increment = 10;
    } else if( str[i] == '.' ){
        increment = 12;
    } else if( str[i] == '_' ){
        increment = 13;
    }  else {
        increment = 0;
    }
    result *= 43;
    result += increment;
    }
    return result;
}
