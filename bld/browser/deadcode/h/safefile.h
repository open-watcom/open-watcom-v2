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


#ifndef SAFEFILE_H_INCLUDED
#define SAFEFILE_H_INCLUDED

#include <wcvector.h>
#include <wstd.h>
#include <wobject.hpp>

extern "C" {
    #include <sys\types.h>
    #include <sys\stat.h>
    #include <fcntl.h>
};

#include "wtime.h"

#define SafeORead       ( O_RDONLY )
#define SafeOReadB      ( O_RDONLY | O_BINARY )
#define SafeOWrite      ( O_WRONLY | O_CREAT | O_TRUNC )
#define SafeOWriteB     ( O_WRONLY | O_CREAT | O_TRUNC | O_BINARY )

/*
 * Access rights which can be specified when the O_CREAT option is
 * used.  These are passed as the second parameter to safeOpen.
 */
#define SafeCreateDefault       ( S_IRUSR | S_IWUSR )

class WString;

class SafeFile : public WObject
{
public:
                SafeFile() : _isOpen(FALSE) {}
                SafeFile( const char * fname );
virtual         ~SafeFile();
virtual void    safeOpen( int access = SafeOReadB,
                          int createmode = SafeCreateDefault );
virtual void    safeClose();
    bool        exists();
virtual void    safeRead( void * buffer, int length );
virtual void    safeWrite( const void * buffer, int length );
virtual void    safeSeek( long pos );
virtual long    safeTell();
        void    safePuts( WString & str );
        void    safeAppend( char * path );
    time_t      getTime();
    long        fileSize();

    static long         fileSize( const char * path );
    static bool         getTime( const char * path, WTime & time );
    static time_t       getTime( const char * path );

    bool        operator== ( const SafeFile& other ) const {
                    return this == &other;
                };

    char *      getFileName() { return _fileName; };
    void        setFileName( const char * fn );

 protected:
    static WCPtrOrderedVector<SafeFile>  openFiles;
    char *              _fileName;
    int                 _handle;
    bool                _isOpen;
    char *              _buffer;
};
#endif
