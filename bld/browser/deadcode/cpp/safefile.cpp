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
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <wstring.hpp>

#include "death.h"
#include "safefile.h"
#include "util.h"

static WCPtrOrderedVector<SafeFile> SafeFile::openFiles;

SafeFile::SafeFile( const char * fname )
                : _isOpen(FALSE)
/**************************************/
{
    _fileName = new char[ strlen( fname ) + 1 ];
    strcpy( _fileName, fname );
}

SafeFile::~SafeFile()
/*******************/
{
    safeClose();

    delete [] _fileName;
};

void SafeFile::setFileName( const char * fn )
/*******************************************/
{
    delete [] _fileName;

    _fileName = new char[ strlen( fn ) + 1 ];
    strcpy( _fileName, fn );
}

void SafeFile::safeOpen( int access, int createmode )
/****************************************************/
{
    int         index;
    SafeFile *  file;

    if( !_isOpen ) {
        for(;;) {

            if( access & O_CREAT ) {
                _handle = open( _fileName, access, createmode );
            } else {
                _handle = open( _fileName, access );
            }

            if( _handle != -1 ) break;
            index = openFiles.entries();
            if( index == 0 ) {
                cantOpenFile( _fileName );
                throw DEATH_BY_NO_FILES;
            }
            index--;
            file = (SafeFile *) openFiles[index];
            file->safeClose();
        }
        _isOpen = TRUE;
        openFiles.append( this );
    }
}

void SafeFile::safeClose( void )
/******************************/
{
    if( _isOpen ) {
        openFiles.remove( this );
        close( _handle );
        _isOpen = FALSE;
    }
}

void SafeFile::safeRead( void * buffer, int len )
/***********************************************/
{
    int result;

    result = read( _handle, (char *) buffer, len );
    if( result != len ) {
        errMessage( "Unable to read file %s\n%s\n", _fileName, strerror( errno )  );
        throw DEATH_BY_FILE_READING;
    }
}

void SafeFile::safeWrite( const void * buffer, int len )
/******************************************************/
{
    int result;

    if( !_isOpen ) {
        errMessage( "Unable to write file %s: file is not open\n", _fileName );
        throw DEATH_BY_FILE_WRITING;
    }

    result = write( _handle, (char *) buffer, len );

    if( result != len ) {
        errMessage( "Unable to write file %s\n%s\n", _fileName, strerror( errno )  );
        throw DEATH_BY_FILE_WRITING;
    }
}

void SafeFile::safePuts( WString & str )
/**************************************/
{
    int length;

    length = str.size();
    if( length > 0 ) {
        safeWrite( (void *) str.gets(), length );
    }
    safeWrite( "\n", 1 );
}

void SafeFile::safeSeek( long pos )
/*********************************/
{
    if( lseek( _handle, pos, SEEK_SET ) < 0 ) {
        errMessage( "Unable to seek on file %s\n%s\n", _fileName, strerror( errno )  );
        throw DEATH_BY_FILE_SEEK;
    }
}

long SafeFile::safeTell()
/***********************/
{
    long res;

    res = tell( _handle );
    if( res < 0 ) {
        errMessage( "Unable to tell on file %s\n%s\n", _fileName, strerror( errno )  );
        throw DEATH_BY_FILE_SEEK;   // NYI - ITB - may want to change
    }

    return res;
}

bool SafeFile::exists()
/*********************/
{
    return access( _fileName, F_OK ) == 0;
}

long SafeFile::fileSize()
/***********************/
{
    safeOpen();                     // make sure we're open
    return filelength( _handle );
}

static long SafeFile::fileSize( const char * path )
/*************************************************/
{
    int rc;
    struct stat fileStat;

    rc = stat( path, &fileStat );
    if( rc == 0 ) {
        return fileStat.st_size;
    }

    return -1;
}

time_t SafeFile::getTime()
/************************/
{
    int rc;
    struct stat fileStat;

    safeOpen();
    rc = fstat( _handle, &fileStat );
    if( rc == 0 ) {
        return fileStat.st_mtime;
    }

    return 0;
}

static bool SafeFile::getTime( const char * path, WTime & time )
/**************************************************************/
// Returns TRUE if successful, FALSE if failed
{
    int rc;
    struct stat fileStat;

    rc = stat( path, &fileStat );
    if( rc == 0 ) {
        time.set( fileStat.st_mtime );
        return TRUE;
    }

    return FALSE;
}

static time_t SafeFile::getTime( const char * path )
/**************************************************/
// NYI -- should have a through for this.  in fact, probably all stat's
// could be collected into one area.
{
    int rc;
    struct stat fileStat;

    rc = stat( path, &fileStat );
    if( rc == 0 ) {
        return fileStat.st_mtime;
    }

    return 0;
}

void SafeFile::safeAppend( char * path )
/**************************************/
// assumes opened for writing
{
    char        buffer[ BUFSIZ ];
    int         size_read;

    int hOther;

    hOther = open( path, O_RDONLY | O_BINARY );
    if( hOther == -1 ) {        // NYI -- a throw here would be good
        errMessage( "Unable to open file %s to append\n%s\n",
                    path, strerror( errno )  );
        return;
    }

    for( ;; ) {
        size_read = read( hOther, buffer, BUFSIZ );

        if( size_read == -1 ) {
            errMessage( "Unable to read file %s to append\n%s\n",
                        path, strerror( errno )  );
            break;              // NYI -- a throw here would be good
        }

        safeWrite( buffer, size_read );

        if( size_read < BUFSIZ ) {
            break;              // done appending
        }
    }

    close( hOther );
}
