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
#include "widechar.h"
#include <io.h>
#include <malloc.h>
#include <mbstring.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#ifdef __NT__
    #include <windows.h>
#endif


_WCRTLINK CHAR_TYPE *__F_NAME(_mktemp,_wmktemp)( CHAR_TYPE *template )
{
    CHAR_TYPE           letter;
    unsigned            pid;
    CHAR_TYPE *         xs;

    /*** Ensure the template is valid ***/
    if( !is_valid_template( template, &xs ) ) {
        return( NULL );
    }

    /*** Get the process/thread ID ***/
    #ifdef __NT__
        #ifdef __SW_BM
            pid = GetCurrentThreadId();     /* thread ID for multi-thread */
        #else
            pid = GetCurrentProcessId();    /* process ID for single-thread */
        #endif
    #else
        pid = getpid();
    #endif
    pid %= 100000;      /* first few digits tend to be repeated under 95 */

    /*** Try to build a unique filename ***/
    for( letter='a'; letter<='z'; letter++ ) {
        #ifdef __WIDECHAR__
            _bwprintf( xs, wcslen(xs) + 1, L"%c%05u", letter, pid );
        #else
            _bprintf( xs, strlen(xs) + 1, "%c%05u", letter, pid );
        #endif
        if( __F_NAME(access,_waccess)( template, F_OK )  !=  0 ) {
            return( template );
        }
    }
    return( NULL );
}


static int is_valid_template( CHAR_TYPE *template, CHAR_TYPE **xs )
{
    int                 len;
    CHAR_TYPE *         p;

    /*** Ensure the last 6 characters form the string "XXXXXX" ***/
    len = __F_NAME(_mbslen,wcslen)( template );
    if( len < 6 )  return( 0 );         /* need room for "XXXXXX" */
    #ifdef __WIDECHAR__
        p = template + len - 6;
        if( wcscmp( p, L"XXXXXX" ) )  return( 0 );
    #else
        p = _mbsninc( template, len-6 );
        if( _mbscmp( p, "XXXXXX" ) )  return( 0 );
    #endif
    *xs = p;

    return( 1 );
}
