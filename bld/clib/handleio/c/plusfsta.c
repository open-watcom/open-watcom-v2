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


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(__NT__)
#include <windows.h>
#endif
#include "iomode.h"
#include "handleio.h"

// these must be the same as is defined in iostream.h

#undef __in
#undef __out
#define __in          0x0001    // - open for input
#define __out         0x0002    // - open for output
#define __atend       0x0004    // - seek to end after opening
#define __append      0x0008    // - open for output, append to the end
#define __truncate    0x0010    // - discard contents after opening
#define __nocreate    0x0020    // - open only an existing file
#define __noreplace   0x0040    // - open only a new file
#define __text        0x0080    // - open as text file
#define __binary      0x0100    // - open as binary file

_WCRTLINK int __plusplus_fstat( int handle, int *pios_mode )
{
    int flags;
    int ios_mode = 0;

#if defined(__NETWARE__)
    handle=handle;
    flags=flags;
    // make a guess
    ios_mode |= __in | __out | __text;
#elif defined(__UNIX__)
    if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
        return( -1 );
    }
    if( flags & O_APPEND ) {
        ios_mode |= __append;
    }
    if( (flags & O_ACCMODE) == O_RDONLY ) {
        ios_mode |= __in;
    } else if( (flags & O_ACCMODE) == O_WRONLY ) {
        ios_mode |= __out;
    } else {
        ios_mode |= __in|__out;
    }
#elif __RDOS__
    flags = 0;
    ios_mode |= __in | __out | __text;
#else
    flags = __GetIOMode( handle );
    if( flags & _APPEND ) {
        ios_mode |= __append;
    }
    if( flags & _READ ) {
        ios_mode |= __in;
    }
    if( flags & _WRITE ) {
        ios_mode |= __out;
    }
    if( flags & _BINARY ) {
        ios_mode |= __binary;
    } else {
        ios_mode |= __text;
    }
#endif
    *pios_mode = ios_mode;
    return( 0 );
}
