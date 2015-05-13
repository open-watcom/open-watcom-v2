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
* Description:  Implements POSIX filelength() function and Watcom
*               _filelength64().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#if defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "errorno.h"
#include "lseek.h"
#include "thread.h"


#if defined(__INT64__)

_WCRTLINK __int64 _filelengthi64( int handle )
{
#if defined(__NT__) || defined( __WARP__ ) || defined( __LINUX__ )
    __int64         file_len;
    __int64         current_posn;

    __handle_check( handle, -1 );
    _AccessFileH( handle );

    current_posn = __lseeki64( handle, 0, SEEK_CUR );
    if( current_posn == -1LL ) {
        _ReleaseFileH( handle );
        return( -1LL );
    }
    file_len = __lseeki64( handle, 0, SEEK_END );
    __lseeki64( handle, current_posn, SEEK_SET );

    _ReleaseFileH( handle );
    return( file_len );
#else
    long            file_len;

    file_len = filelength( handle );
    if( file_len == -1L ) {
        return( -1LL );
    }
    return( (unsigned long)file_len );
#endif
}

#else

_WCRTLINK long filelength( int handle )
{
    long            current_posn;
    long            file_len;

    __handle_check( handle, -1 );
    _AccessFileH( handle );

    current_posn = __lseek( handle, 0L, SEEK_CUR );
    if( current_posn == -1L )
    {
        _ReleaseFileH( handle );
        return( -1L );
    }
    file_len = __lseek( handle, 0L, SEEK_END );
    __lseek( handle, current_posn, SEEK_SET );

    _ReleaseFileH( handle );
    return( file_len );
}

#endif
