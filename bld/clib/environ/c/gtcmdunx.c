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
* Description:  Implementation of getcmd() and _bgetcmd() for Unix.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "initarg.h"
#ifdef __WATCOMC__  // Needs to be built with other tools for clibext
    #include <process.h>
#endif


_WCRTLINK int (_bgetcmd)( char *buffer, int len )
{
    int     total;
    int     i;
    char    *word;
    char    *p;
    char    **argv = &_argv[1];
    int     need_quotes;

    if( (buffer != NULL) && (len > 0) ) {
        p = buffer;
        --len;          // reserve space for terminating NULL byte
    } else {
        p = NULL;
        len = 0;
    }

    /* create approximation of original command line */
    total = 0;
    while( (word = *argv++) != NULL ) {
        /* account for at least one space separating arguments */
        if( total != 0 ) {
            ++total;
            if( len != 0 ) {
                --len;
                *p++ = ' ';
            }
        }

        i = strlen( word );
        total += i;
        need_quotes = ( strpbrk( word, " " ) != NULL || i == 0 );
        if( need_quotes )
            total += 2;

        if( need_quotes && len != 0 ) {
            --len;
            *p++ = '"';
        }
        if( len != 0 && i != 0 ) {
            if( i > len )
                i = len;
            memcpy( p, word, i );
            p += i;
            len -= i;
        }
        if( need_quotes && len != 0 ) {
            --len;
            *p++ = '"';
        }
    }
    if( p != NULL ) {
        *p = '\0';  // terminate string
    }

    return( total );
}


_WCRTLINK char *(getcmd)( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}
