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

extern char             *_LpCmdLine;

_WCRTLINK int _bgetcmd( char *buffer, int len )
{
    char *cmd;
    char *p;

    cmd = _LpCmdLine;
    if( cmd == 0 ) {
        if( buffer && len ) {
            *buffer = '\0';
        }
        return( 0 );
    }
    if( buffer == 0 ) {
        for( p = cmd ; *p ; p++ );
        return( p - cmd );
    } else if( len == 0 ) {
        return( 0 );
    } else {
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

