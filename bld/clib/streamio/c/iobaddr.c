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
#include "rtdata.h"

_WCRTLINK FILE *__get_std_stream( unsigned handle ) {
    if( handle > NUM_STD_STREAMS ) {
        return NULL;
    } else {
        return &_RWD_iob[handle];
    }
}


#ifdef __NETWARE__

#include <io.h>

FILE **__get_stdin( void )
{
    static FILE         *stdin_ptr;

    stdin_ptr = __get_std_stream( STDIN_HANDLE );
    return( &stdin_ptr );
}

FILE **__get_stdout( void )
{
    static FILE         *stdout_ptr;

    stdout_ptr = __get_std_stream( STDOUT_HANDLE );
    return( &stdout_ptr );
}

FILE **__get_stderr( void )
{
    static FILE         *stderr_ptr;

    stderr_ptr = __get_std_stream( STDERR_HANDLE );
    return( &stderr_ptr );
}

#endif
