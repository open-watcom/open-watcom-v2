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
#include <limits.h>
#include <ctype.h>
#include <dos.h>
#define INCL_DOSMISC
#include <wos2.h>


_WCRTLINK int _bgetcmd( char *buffer, int len )
{
    char _WCI86FAR *cmd;

    #if defined(__386__)
    {
        PTIB            tib;
        PPIB            pib;
        if( DosGetInfoBlocks( &tib, &pib ) != 0 ) {
            if( buffer && len ) {
                *buffer = '\0';
            }
            return( 0 );
        }
        cmd = pib->pib_pchcmd;
    }
    #else
    {
        unsigned short  envseg, cmdoff;
        if( DosGetEnv( &envseg, &cmdoff ) != 0 ) {
            if( buffer && len ) {
                *buffer = '\0';
            }
            return( 0 );
        }
        cmd = MK_FP( envseg, 0 );
        if( !*cmd ) ++cmd;      /* adjust if null environment space */
        while( *cmd ) {         /* skip over environment space */
            while( *cmd ) ++cmd;
            ++cmd;
        }
        ++cmd;
        while( *cmd ) ++cmd;/* skip over first copy of program name */
        ++cmd;
    }
    #endif
    while( *cmd ) ++cmd;        /* skip over second copy of program name */
    ++cmd;
    while( isspace( *cmd ) ) cmd++; /* trim leading blanks */

    if( buffer == 0 ) {
        int i;
        for( i = 0 ; *cmd ; cmd++, i++ );
        return( i );
    } else if( len == 0 ) {
        return( 0 );
    } else {
        char *p;
        p = buffer;
        for( --len ; len ; --len ) {
            if( !*cmd ) break;
            *p++ = *cmd++;
        }
        *p = '\0';
        return( p - buffer );
    }
}


_WCRTLINK char *getcmd( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}

