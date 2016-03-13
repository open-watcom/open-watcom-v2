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
* Description:  Platform independent tmpnam() implementation.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtdata.h"
#include "rterrno.h"
#include "fileacc.h"
#include "exitwmsg.h"
#include "liballoc.h"
#include "tmpnm.h"
#include "thread.h"
#include "pathmac.h"

/*
    U's     are unique filename letters for the process
    P's     are the pid
    N's     are the nid
    .tmp    is a literal string

    On QNX, the name is of the form (only use the 16 low order bits of the nid):

        _PPPPPPP.NNNN.UUU

    On other systems, the name looks like:

        _PPPPPPP.UUU
*/

#if defined(__NETWARE__)
 #define getpid()       GetThreadID()
 extern int             GetThreadID( void );
#elif defined(__UNIX__)
 extern char *__tmpdir( char * );
#endif

static CHAR_TYPE *__F_NAME(putbits,_wputbits)( CHAR_TYPE *p, unsigned val )
{
    if( val > 0x1f ) {
        p = __F_NAME(putbits,_wputbits)( p, val >> 5 );
        val &= 0x1f;
    }
    if( val >= 10 ) {
        *p = val - 10 + STRING( 'A' );
    } else {
        *p = val + STRING( '0' );
    }
    return( p + 1 );
}

static size_t init_name( void )
{
    CHAR_TYPE   *p;

    p = (CHAR_TYPE *)_RWD_tmpnambuf;
#if defined(__UNIX__)
    p = __tmpdir( p );
#endif
    *p++ = STRING( '_' );
    p = __F_NAME(putbits,_wputbits)( p, getpid() );
#if defined(__QNX__)
    *p++ = STRING( '.' );
    p = __F_NAME(putbits,_wputbits)( p, (unsigned short)getnid() );
#endif
    *p++ = EXT_SEP;
    *p++ = STRING( 'A' );
    *p++ = STRING( 'A' );
    *p++ = STRING( 'A' ) - 1;
    *p   = NULLCHAR;
    return( p - (CHAR_TYPE *)_RWD_tmpnambuf );
}

_WCRTLINK CHAR_TYPE *__F_NAME(tmpnam,_wtmpnam)( CHAR_TYPE *buf )
{
    int         err;
    int         iter;
    size_t      i;
    CHAR_TYPE   *tmpnmb;

    err = _RWD_errno;
                            // JBS 99/10/18 rewrote for thread safety
    _AccessIOB();           // prevent same name in multi-threaded apps
    tmpnmb = (CHAR_TYPE *)_RWD_tmpnambuf;
    if( (tmpnmb[0] == NULLCHAR) || (tmpnmb[0] != STRING( '_' )) ) {
        i = init_name();    // set to ?.AA@
    } else {
        i = __F_NAME(strlen,wcslen)( tmpnmb );
    }
    // let's go around the loop at most twice
    for( iter = 0; iter < 2; ) {
        //
        //  tmpnambuf now contains something like _PPPPPPP.AAA
        //
        for( ;; ) {
            --i;
            // if ?.ZZZ then start over with ?.AA@
            if( tmpnmb[i] == EXT_SEP ) {
                i = init_name() - 1;
                iter++;
            }
            // if current extension char is not 'Z' then we can exit loop and increment
            if( tmpnmb[i] != STRING( 'Z' ) )
                break;
            // ?.ABZ -> ?.ABA (which will become ?.ACA)
            tmpnmb[i] = STRING( 'A' );
        }
        tmpnmb[i]++;    // next name, e.g., ?.AAA -> ?.AAB
        if( __F_NAME(access,_waccess)( tmpnmb, 0 ) != 0 ) {
            break;
        }
    }
    // if iter is 2 then we failed to find a useable name
    if( iter == 2 )
        tmpnmb[0] = NULLCHAR;
    if( buf != NULL ) {
        __F_NAME(strcpy,wcscpy)( buf, tmpnmb );
    } else {
        buf = tmpnmb;
    }
    _ReleaseIOB();          // if it's been copied, we are thread-safe
    if( iter == 2 )
        buf = NULL;
    _RWD_errno = err;
    return( buf );
}
