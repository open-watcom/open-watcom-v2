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


// System includes --------------------------------------------------------

#include <io.h>
#include <fcntl.h>
#include <string.h>

// Project includes -------------------------------------------------------

#include "mfile.h"
#include "errors.h"

MFile::MFile( char * fname )
/**************************/
    : _isOpen( FALSE )
    , _fileName( NULL )
{
    int len;

    len = strlen( fname );
    _fileName = new char[ len + 1 ];
    if( _fileName == NULL ) {
        throw ErrOutOfMemory;
    }
    strcpy( _fileName, fname );

}

MFile::~MFile()
/*************/
{
    close();
    if( _fileName != NULL ) {
        delete [] _fileName;
    }
}

char * MFile::name()
/******************/
{
    return _fileName;
}

bool MFile::open( int access, int createmode )
/********************************************/
{
    if( !_isOpen ) {
        if( access & O_CREAT ) {
            _handle = ::open( _fileName, access, createmode );
        } else {
            _handle = ::open( _fileName, access );
        }
        if( _handle != -1 ) {
            _isOpen = TRUE;
        }
    }

    return _isOpen;
}

void MFile::close( void )
/******************************/
{
    if( _isOpen ) {
        ::close( _handle );
        _isOpen = FALSE;
    }
}

int MFile::read( void * buffer, int len )
/***************************************/
{
    return( ::read( _handle, (char *) buffer, len ));
}

int MFile::write( void * buffer, int len )
/****************************************/
{
    return( ::write( _handle, (char *) buffer, len ));
}

int MFile::putch( unsigned char c )
/*********************************/
{
    return( ::write( _handle, &c, 1 ));
}

long MFile::seek( long pos )
/**************************/
{
    return( ::lseek( _handle, pos, SEEK_SET ));
}

long MFile::tell()
/****************/
{
    return ::tell( _handle );
}

unsigned char MFile::getch()
/**************************/
{
    unsigned char c;

    ::read( _handle, &c, 1 );
    return c;
}

long MFile::fileSize()
/********************/
{
    return filelength( _handle );
}

static long MFile::fileSize( char * path )
/****************************************/
{
    int rc;
    struct stat info;

    rc = stat( path, &info );
    if( rc == -1 ) {
        throw MFileErrGeneral;
    }

    return info.st_size;
}

uint_32 MFile::readULEB128()
/**************************/
// Returns the ULEB128 value
{
    uint_32     result;
    uint        shift;
    uint_8      b;

    result = 0;
    shift = 0;
    for(;;) {
        b = getch();
        result |= ( b & 0x7f ) << shift;
        if( ( b & 0x80 ) == 0 ) break;
        shift += 7;
    }
    return( result );
}

int MFile::writeULEB128( uint_32 value )
/**************************************/
// Returns the number of bytes written.
{
    uint_8                      b;
    int                         count;

    for( count = 1 ;; count++ ) {
        b = (uint_8) (value & 0x7f);
        value >>= 7;
        if( value == 0 ) break;
        putch( b | 0x80 );
    }
    putch( b );
    return( count );
}

void MFile::gets( char * str, int & length )
/******************************************/
// Allocate memory for and read in a string
// Returns string and length of string, including terminating null.
{
    const int increment = 32;
    int size;

    length = 0;
    size = increment;
    str = new char[ increment ];
    if( str == NULL ) {
        throw ErrOutOfMemory;
    }

    for( ;; ) {
        if( length == size ) {
            char * tmp;

            size += increment;
            tmp = new char[ size ];
            if( tmp == NULL ) {
                throw ErrOutOfMemory;
            }

            memcpy( tmp, str, length );
            delete [] str;
            str = tmp;
        }

        if( ::read( _handle, &str[ length ], 1 ) != 1 ) {
            delete [] str;
            str = NULL;
            length = 0;
            break;
        }

        if( str[ length ] == '\0' ) {
            length++;
            break;
        }

        length++;
    }
}

void MFile::append( char * path )
/*******************************/
{
    int hOther;
    unsigned char c;
    int rc;

    if( !_isOpen ) {
        return;
    }

    hOther = ::open( path, O_RDONLY | O_BINARY );
    if( hOther == -1 ) {
        return;
    }

    for( ;; ) {
        rc = ::read( hOther, &c, sizeof( c ));

        if( rc == 0 ) {
            break;
        }

        ::write( _handle, &c, sizeof( c ));
    }

    ::close( hOther );
}
