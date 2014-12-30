/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. 
*    Portions Copyright (c) 2014 Open Watcom contributors. 
*    All Rights Reserved.
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
    char    *p     = NULL;
    char    **argv = &_argv[0];
    char    *space;

    --len; // reserve space for NULL byte

    if( buffer && (len > 0) ) {
        p  = buffer;
        *p = '\0';
    }
    
    /* create approximation of original command line */
    for( word = *argv++, i = 0, total = 0; word; word = *argv++ ) {
        i      = strlen( word );
        total += i;
        
        /* Check for spaces.  If found, this argument should
         * be quoted.  This solution is a bit hacky and might
         * possibly be fooled under very odd circumstances.
         */
        space = strchr(word, ' ');
        if(space != NULL)
            total +=2;

        if( p ) {
        
            if(space != NULL && len > 0) {
                *p++ = '"';
                --len;
            }
        
            if( i >= len ) {
                strncpy( p, word, len );
                p[len] = '\0';
                p      = NULL;
                len    = 0;
            } else {
                strcpy( p, word );
                p   += i;
                len -= i;
            }
            
            if(space != NULL && len > 0) {
                *p++ = '"';
                --len;
            }
        }

        /* account for at least one space separating arguments */
        if( *argv ) {
            if( p != NULL && len > 0 ) {
                *p++ = ' ';
                --len;
            }
            ++total;
        }
    }

    return( total );
}


_WCRTLINK char *(getcmd)( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}
