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
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include <limits.h>
#include "rtdata.h"

_WCRTLINK int _bgetcmd( char *buffer, int len )
{
    int i;
    int cmdlen;
    char *p;
    char _WCFAR *cmd;

    cmd    = MK_FP( _RWD_psp, 0x0080 );
    cmdlen = *cmd++;

    while ((cmdlen > 0) && isspace(*cmd))
    {
        ++cmd;
        --cmdlen;
    }

    if (cmdlen <= 0)
    {
        if (buffer && (len > 0))
            *buffer = 0x00;

        return 0;
    }

    i = cmdlen;

    if( buffer == 0 ) {
        return( cmdlen );
    } else if( len < 2 ) {
        *buffer = '\0';
        return( cmdlen );
    } else {
        p = buffer;
        if( i >= len ) i = len - 1;
        while( i-- ) {
            *p++ = *cmd++;
        }
        *p = '\0';
        return cmdlen;
        //return( p - buffer );
    }
}

_WCRTLINK char *getcmd( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
}

