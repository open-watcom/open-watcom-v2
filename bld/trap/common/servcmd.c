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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "trptypes.h"
#include "trpld.h"
#include "packet.h"
#include "trperr.h"
#include "tcerr.h"
#include "servio.h"

extern trap_version     TrapVersion;

static const char *SkipSpaces( const char *ptr )
{
    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    return( ptr );
}

/*
 * GetFilename -- get filename from command line
 */
static const char *GetFilename( const char *ptr, char *buff )
{
    ptr = SkipSpaces( ptr );
    for( ;; ) {
        if( *ptr == '\0' ) break;
        if( *ptr == ' ' ) break;
        if( *ptr == '\t' ) break;
#if !defined(__UNIX__)
        if( *ptr == '/' ) break;
        if( *ptr == '-' ) break;
#endif
        if( *ptr == TRAP_PARM_SEPARATOR ) break;
        if( *ptr == '{' ) break;
        *buff++ = *ptr++;
    }
    *buff = '\0';
    return( ptr );
}

static const char *CollectTrapParm( const char *ptr, char *buff )
{
    unsigned    num;

    if( *ptr == '{' ) {
        ++ptr;
        num = 1;
        for( ;; ) {
            if( *ptr == '\0' ) {
                StartupErr( TRP_ERR_expect_brace );
            } else if( *ptr == '{' ) {
                ++num;
            } else if( *ptr == '}' ) {
                if( --num == 0 ) {
                    ++ptr;
                    break;
                }
            }
            *buff++ = *ptr++;
        }
        *buff = '\0';
    } else {
        ptr = GetFilename( ptr, buff );
    }
    return( ptr );
}

#if defined( __UNIX__ )
    #define IS_OPTION( c )      ((c) == '-')
#else
    #define IS_OPTION( c )      ((c) == '-' || (c) == '/')
#endif

bool ParseCommandLine( const char *cmdline, char *trapparms, char *servparms, bool *oneshot )
/*******************************************************************************************/
{
    const char  *start;
    const char  *ptr;
    char        *buff;

    *oneshot = false;
    *trapparms = '\0';
    *servparms = '\0';
#if defined(__AXP__) && defined(__NT__)
    //NYI: temp until we can get all the unaligned stuff straightened out.
    SetErrorMode( SEM_NOALIGNMENTFAULTEXCEPT );
#endif
    ptr = SkipSpaces( cmdline );
    if( WantUsage( ptr ) ) {
        StartupErr( ServUsage );
        return( false );
    }
    while( IS_OPTION( *ptr ) ) {
        ptr = SkipSpaces( ptr + 1 );
        start = ptr;
        #undef isalpha
        while( isalpha( *ptr ) )
            ++ptr;
        if( ptr == start ) {
            StartupErr( TRP_ERR_expect_option );
            return( false );
        } else if( strnicmp( "trap", start, ptr - start ) == 0 ) {
            ptr = SkipSpaces( ptr );
            if( *ptr != '=' && *ptr != '#' ) {
                StartupErr( TRP_ERR_expect_equal );
                return( false );
            }
            ptr = SkipSpaces( GetFilename( ptr + 1, trapparms ) );
            if( *ptr == TRAP_PARM_SEPARATOR ) {
                buff = trapparms + strlen( trapparms );
                *buff++ = TRAP_PARM_SEPARATOR;
                ptr = CollectTrapParm( SkipSpaces( ptr + 1 ), buff );
            } else if( *ptr == '{'/*}*/ ) {
                buff = trapparms + strlen( trapparms );
                *buff++ = TRAP_PARM_SEPARATOR;
                ptr = CollectTrapParm( ptr, buff );
            }
        } else if( strnicmp( "once", start, ptr - start ) == 0 ) {
            *oneshot = true;
        }
        ptr = SkipSpaces( ptr );
    }
    CollectTrapParm( ptr, servparms );
    TrapVersion.remote = true;
    return( true );
}
