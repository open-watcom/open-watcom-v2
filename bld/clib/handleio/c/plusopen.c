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
* Description:  C++ handle oriented open.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#if defined(__UNIX__)
 #include <unistd.h>
#else
 #include <io.h>
#endif
#include <share.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "handleio.h"

// these must be the same as is defined in iostream.h

#define __in          0x0001    // - open for input
#define __out         0x0002    // - open for output
#define __atend       0x0004    // - seek to end after opening
#define __append      0x0008    // - open for output, append to the end
#define __truncate    0x0010    // - discard contents after opening
#define __nocreate    0x0020    // - open only an existing file
#define __noreplace   0x0040    // - open only a new file
#define __text        0x0080    // - open as text file
#define __binary      0x0100    // - open as binary file

// these must be the same as is defined in fstream.h

#define __sh_compat   0x0000    // - compatibility mode
#define __sh_read     0x1000    // - allow others to read
#define __sh_write    0x2000    // - allow others to write
#define __sh_none     0x4000    // - do not allow others to read or write

_WCRTLINK int __plusplus_open( const char *name, int *pios_mode, int prot )
{
    int ios_mode = *pios_mode;
    int mode;
    int share;

    // Figure out the POSIX "open" function parameters based on ios_mode:
    if( (ios_mode & (__in | __out)) == (__in | __out) ) {
        mode = O_RDWR|O_CREAT;
    } else if( ios_mode & __in ) {
        mode = O_RDONLY;
    } else if( ios_mode & __out ) {
        mode = O_WRONLY|O_CREAT;
    } else {
        return( -1 );
    }
    if( ios_mode & __append ) {
        mode |= O_APPEND;
    }
    if( ios_mode & __truncate ) {
        mode |= O_TRUNC;
    }
    if( ios_mode & __nocreate ) {
        mode &= ~O_CREAT;
    }
    #if defined(__UNIX__)
        *pios_mode &= ~(__binary|__text);
    #else
        if( ios_mode & __binary ) {
            mode |= O_BINARY;
        } else {
            mode |= O_TEXT;
            *pios_mode |= __text;
        }
    #endif

    // If "noreplace" is specified and O_CREAT is set,
    // then an error will occur if the file already exists:
    if( (ios_mode&__noreplace) && (mode|O_CREAT) ) {
        struct stat buf;

        if( stat( name, &buf) != -1) {
            return( -1 );
        }
    }

    // Figure out sharing mode
    share = SH_COMPAT;
    switch( prot & (__sh_read|__sh_write|__sh_none) ) {
    case __sh_read:
        share = SH_DENYWR;      /* deny write */
        break;
    case __sh_write:
        share = SH_DENYRD;      /* deny read */
        break;
    case __sh_read|__sh_write:
        share = SH_DENYNO;      /* deny none */
        break;
    case __sh_none:
        share = SH_DENYRW;      /* deny read/write */
        break;
    }
    prot &= ~(__sh_read|__sh_write|__sh_none);

    return( sopen( name, mode, share, prot ) );
}
