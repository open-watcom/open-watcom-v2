/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implements _filelength() and _filelength64() functions.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <stdio.h>
#include <unistd.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "fileacc.h"
#include "iomode.h"
#include "rtcheck.h"
#include "lseek.h"
#include "thread.h"
#include "filei64.h"


_WCRTLINK __64_NAME(long,__int64) __64_NAME(_filelength,_filelengthi64)( int handle )
{
#if !defined( __INT64__ ) || defined( __NT__ ) || defined( __OS2_32BIT__ ) || defined( __LINUX__ )
    __64_NAME(long,__int64) file_len;
    __64_NAME(long,__int64) current_posn;

    __handle_check( handle, -1 );
    _AccessFileH( handle );

    current_posn = __64_NAME(__lseek,__lseeki64)( handle, 0, SEEK_CUR );
    if( current_posn == -1 ) {
        _ReleaseFileH( handle );
        return( -1 );
    }

    file_len = __64_NAME(__lseek,__lseeki64)( handle, 0, SEEK_END );
    __64_NAME(__lseek,__lseeki64)( handle, current_posn, SEEK_SET );

    _ReleaseFileH( handle );
    return( file_len );
#else
    long            file_len;

    file_len = _filelength( handle );
    if( file_len == -1 ) {
        return( -1 );
    }
    return( (unsigned long)file_len );
#endif
}
