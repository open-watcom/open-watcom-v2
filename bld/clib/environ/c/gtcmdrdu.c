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
* Description:  RDOS implementation of getcmd() and _bgetcmd().
*
****************************************************************************/


#include "variety.h"
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <rdos.h>

_WCRTLINK int _bgetcmd( char *buffer, int len )
{
    int         cmdlen;
    const char  *cmd;
    const char  *tmp;

    if( buffer && (len > 0) )
        *buffer = '\0';

    cmd = RdosGetCmdLine();
    if( !cmd )
        return( 0 );

    while( *cmd != ' ' && *cmd != '\t' )
        ++cmd;

    while( *cmd == ' ' || *cmd == '\t' )
        ++cmd;

    for( cmdlen = 0, tmp = cmd; *tmp; ++tmp, ++cmdlen )
        ;

    if( !buffer || (len <= 0) )
        return( cmdlen );

    len--;
    len = (len < cmdlen) ? len : cmdlen;

    while( len ) {
        *buffer++ = *cmd++;
        --len;
    }
    buffer[len] = '\0';

    return( cmdlen );
} /* _bgetcmd() */

_WCRTLINK char *getcmd( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
} /* getcmd() */
