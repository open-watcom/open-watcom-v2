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
* Description:  Multi-platform implementation of getcmd() and _bgetcmd().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <limits.h>
#include <ctype.h>
#include <string.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #define INCL_DOSMISC
    #include <wos2.h>
#endif
#include <process.h>
#include "initarg.h"

#ifdef __NT__

    static char *OS_GET_CMD_LINE( void )
    {
        char    *cmd = GetCommandLine();

        if( *cmd == '"' ) {
            cmd++;
            while( *cmd && *cmd != '"' ) {
                cmd++;
            }
            if( *cmd ) cmd++;
        } else {
            while( *cmd && *cmd != ' ' && *cmd != '\t' ) {
                ++cmd;
            }
        }
        return cmd;
    }

#elif defined(_NETWARE_CLIB) || defined(__OSI__)

    #define OS_GET_CMD_LINE()    _LpCmdLine

#elif defined(_NETWARE_LIBC)

    #define OS_GET_CMD_LINE()    getcmd(NULL)

#elif defined(__WINDOWS_386__)

    #define OS_GET_CMD_LINE()    _wincmdptr

#elif defined(__OS2_286__)

    #include <dos.h>
    static char _WCFAR *OS_GET_CMD_LINE( void )
    {
        unsigned short      envseg, cmdoff;
        char _WCFAR         *cmd;

        if( DosGetEnv( &envseg, &cmdoff) )
            return( NULL );

        cmd = MK_FP( envseg, 0 );
        if( !*cmd ) ++cmd;      /* adjust if null environment space */
        while( *cmd ) {         /* skip over environment space */
            while( *cmd ) ++cmd;
            ++cmd;
        }
        ++cmd;
        while( *cmd ) ++cmd;    /* skip over first copy of program name */
        ++cmd;

        while( *cmd ) ++cmd;    /* skip over second copy of program name */
        ++cmd;

        return( cmd );
    }

#elif defined(__WARP__)

    static char *OS_GET_CMD_LINE( void )
    {
        PTIB    tib;
        PPIB    pib;
        char    *cmd;

        if( DosGetInfoBlocks(&tib, &pib) )
            return( NULL );

        cmd = pib->pib_pchcmd;
        while( *cmd ) ++cmd;    /* skip over second copy of program name */
        ++cmd;
        return( cmd );
    }

#else

    #error *** This Module Does Not Support That Platform ***

#endif

_WCRTLINK int _bgetcmd( char *buffer, int len )
{
    int         cmdlen;
#if defined(__WINDOWS_386__) || defined(__OS2_286__)
    char _WCFAR *cmd;
    char _WCFAR *tmp;
#else
    char        *cmd;
    char        *tmp;
#endif

    if( buffer && (len > 0) )
        *buffer = '\0';

    cmd = OS_GET_CMD_LINE();
    if( !cmd )
        return( 0 );

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

/* Netware LibC has a special startup file (libc prelude) so getcmd is pulled from the LIBC imports */
#if !defined(_NETWARE_LIBC)

_WCRTLINK char *getcmd( char *buffer )
{
    _bgetcmd( buffer, INT_MAX );
    return( buffer );
} /* getcmd() */

#endif
