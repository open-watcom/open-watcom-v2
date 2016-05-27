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
#include <string.h>
#include <errno.h>
#include <stdio.h>      // need for seek-whence defines
#include <algorithm>

#include "chfile.h"

const char  *BadWhenceMessage = "Invalid \"whence\" argument.";
const unsigned MaxOpenReadFiles = 20;      // maximum # of files open for reading

FileExcept::FileExcept( Action act, int error, const char * fn,
                            const char * msg )
                : _action( act )
                , _error( error )
//---------------------------------------------------------------
// have to make a copy of the messages as they may be destroyed
// during stack unwinding
{
    size_t len;
    const char * storeMsg = (msg) ? msg : strerror( error );

    len = strlen( fn );
    _fileName = new char[ len + 1 ];
    memcpy( _fileName, fn, len + 1 );

    len = strlen( storeMsg );
    _message = new char[ len + 1 ];
    memcpy( _message, storeMsg, len + 1 );
}

FileExcept::~FileExcept()
//-----------------------
{
    delete [] _fileName;
    delete [] _message;
}

// OpenFiles is a list of all of the opened CheckedFiles.  It is statically
// initialized, but to avoid including <wcvector.h> in "chfile.h", the
// class only stores a pointer to the static instance.

static std::vector<CheckedFile *> OpenFiles;

std::vector<CheckedFile *> *CheckedFile::_openFiles = &OpenFiles;

CheckedFile::CheckedFile( const char * fileName )
                : _openAccess( 0 )
                , _openPermission( 0 )
                , _handle( -1 )
                , _currOffset( -1 )
                , _isOpen( false )
                , _logOpen( false )
//---------------------------------------------------------------------------
{
    size_t len;

    len = strlen( fileName ) + 1;
    _fileName = new char[ len ];

    memcpy( _fileName, fileName, len );
}


CheckedFile::~CheckedFile()
//-------------------------
{
    if( _logOpen ) {
        close();
    }

    delete [] _fileName;
}

void CheckedFile::setFileName( const char * fileName )
//----------------------------------------------------
{
    size_t len;

    assert( !_isOpen && !_logOpen );

    delete [] _fileName;

    len = strlen( fileName ) + 1;
    _fileName = new char[ len ];

    memcpy( _fileName, fileName, len );
}

void CheckedFile::removeOpenFile( CheckedFile *ent )
//---------------------------------------------------
{
    std::vector<CheckedFile *>::iterator it;

    it = std::find( _openFiles->begin(), _openFiles->end(), ent );
    if( it != _openFiles->end() ) {
        _openFiles->erase( it );
    }
}

void CheckedFile::addOpenFile( CheckedFile *ent )
//------------------------------------------------
{
    _openFiles->push_back( ent );
}


void CheckedFile::open( int access, int permission )
//--------------------------------------------------
{
    CheckedFile * closeIt;    // file to be closed when out of handles

    if( !_isOpen ) {
        _openAccess = access;
        _openPermission = permission;

        while( _openFiles->size() >= MaxOpenReadFiles ) {
            closeIt = _openFiles->back();
            closeIt->privClose();
            removeOpenFile( closeIt );
        }

        _handle = ::open( _fileName, _openAccess, _openPermission );

        while( _handle == -1 && errno == EMFILE ) {
            if( _openFiles->size() == 0 ) {
                FileExcept oops( FileExcept::Open, errno, _fileName );
                throw( oops );
            }

            closeIt = _openFiles->back();
            closeIt->privClose();
            removeOpenFile( closeIt );

            _handle = ::open( _fileName, _openAccess, _openPermission );
        }

        if( _handle == -1 ) {   // error other than out of handles
            FileExcept oops( FileExcept::Open, errno, _fileName );
            throw( oops );
        }

        _isOpen = true;
        _logOpen = true;
        _currOffset = 0;
        if( !( _openAccess & O_WRONLY ) ) {
            addOpenFile( this );
        }
    }
}

void CheckedFile::reOpen()
//------------------------
// re-open a file and seek to the position it
// was closed at.
{
#ifndef NDEBUG
    long newOff;
#endif

    CheckedFile::open( _openAccess, _openPermission );
#ifndef NDEBUG
    newOff = ::lseek( _handle, _currOffset, SEEK_SET );
    assert( newOff == _currOffset );
#else
    ::lseek( _handle, _currOffset, SEEK_SET );
#endif
}


void CheckedFile::close()
//-----------------------
{
    assert( _logOpen );

    privClose();

    _logOpen = false;
    _currOffset = -1;
    _handle = -1;
}

void CheckedFile::privClose()
//---------------------------
// close the physical file, logical file stays open
// since privClose is called from a destructor, avoid
// doing a throw in it.
{
#ifndef NDEBUG
    int retVal;
#endif

    if( _isOpen ) {
#ifndef NDEBUG
        retVal = ::close( _handle );
        assert( retVal == 0 );
#else
        ::close( _handle );
#endif
        if( _openAccess & O_RDONLY ) {
            removeOpenFile( this );
        }
    }

    _isOpen = false;
}


int CheckedFile::read( void * buffer, int len )
//---------------------------------------------
{
    int numRead;

    if( !_isOpen ) {
        reOpen();
    }

    numRead = ::read( _handle, buffer, len );

    if( numRead == -1 ) {
        FileExcept oops( FileExcept::Read, errno, _fileName );
        throw( oops );
    }

    _currOffset += numRead;

    return numRead;
}

void CheckedFile::write( const void * buffer, int len )
//-----------------------------------------------------
{
    int numWritten;

    if( !_isOpen ) {
        reOpen();
    }

    numWritten = ::write( _handle, (void *) buffer, len );  // cast away const

    if( numWritten != len ) {
        FileExcept oops( FileExcept::Write, errno, _fileName );
        throw( oops );
    }

    _currOffset += numWritten;
}

int CheckedFile::readNString( std::string & str )
//------------------------------------------
// read in a string in the form <uint_16>{<byte>}*
{
    const int   BufLen = 255;
    char        buffer[ BufLen + 1 ];
    uint_16     strLen;
    uint_16     amtRead = 0;
    int         maxRead;

    str = "";       // clear string

    read( &strLen, sizeof( uint_16 ) );

    while( amtRead < strLen ) {
        maxRead = strLen - amtRead;
        if( BufLen < maxRead ) {
            maxRead = BufLen;
        }
        read( buffer, maxRead );
        buffer[ maxRead ] = '\0';
        str += buffer;
        amtRead += (uint_16)maxRead;
    }

    return strLen;
}

int CheckedFile::writeNString( std::string & str )
//-------------------------------------------
// write out a string in the form <uint_16>{<byte>}*
{
    uint_16 strLen;

    strLen = (uint_16) str.length();
    write( &strLen, sizeof( uint_16 ) );
    write( str.c_str(), strLen );

    return strLen;
}

void CheckedFile::puts( const char * str )
//----------------------------------------
{
    int len;

    if( str != NULL ) {
        len = (int)strlen( str );
        if( len > 0 ){
            write( str, len );
        }
    }

    write( "\n", 1 );
}


long CheckedFile::seek( long offset, int whence )
//-----------------------------------------------
{
    long newOff;

    if( !_isOpen ) {
        _currOffset = offset;
        reOpen();
    }

    switch( whence ) {
    case SEEK_SET:
        if( offset > _currOffset ) {
            newOff = ::lseek( _handle, offset - _currOffset, SEEK_CUR );
        } else {
            if( offset == _currOffset ) {
                return offset;              // already at right position
            } else {
                newOff = ::lseek( _handle, offset, SEEK_SET );
            }
        }
        break;
    case SEEK_CUR:
    case SEEK_END:
        newOff = ::lseek( _handle, offset, whence );
        break;
    default:
        FileExcept oops( FileExcept::Seek, EINVAL, _fileName, BadWhenceMessage );
        throw( oops );
    }

    if( newOff == -1 ) {
        FileExcept oops( FileExcept::Seek, errno, _fileName );
        throw( oops );
    }

    _currOffset = newOff;
    return newOff;
}

long CheckedFile::tell() const
//----------------------------
{
    return _currOffset;
}

int CheckedFile::fstat( struct stat * buf )
//-----------------------------------------
{
    int retVal;

    if( !_isOpen ) {
        reOpen();
    }

    retVal = ::fstat( _handle, buf );

    if( retVal != 0 ) {
        FileExcept oops( FileExcept::Stat, errno, _fileName );
        throw( oops );
    }

    return retVal;
}

time_t CheckedFile::get_st_mtime()
//----------------------------
{
    struct stat buf;

    fstat( &buf );

    return buf.st_mtime;
}

off_t CheckedFile::get_st_size()
//--------------------------
{
    struct stat buf;

    fstat( &buf );

    return buf.st_size;
}


