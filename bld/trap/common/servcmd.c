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
#include "banner.h"
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "tcerr.h"

extern int              WantUsage( char * );
extern void             StartupErr(char *);
extern  char ServUsage[];
extern char  RWBuff[ 0x400 ];
extern trap_version     TrapVersion;

static char *SkipSpaces( char *ptr )
{
    while( *ptr == ' ' || *ptr == '\t' ) ++ptr;
    return( ptr );
}

/*
 * GetFilename -- get filename from command line
 */
static char *GetFilename( char *ptr, char *buff )
{
    ptr = SkipSpaces( ptr );
    for( ;; ) {
        if( *ptr == '\0' ) break;
        if( *ptr == ' ' ) break;
        if( *ptr == '\t' ) break;
#if !defined(__QNX__) && !defined(__LINUX__)
        if( *ptr == '/' ) break;
        if( *ptr == '-' ) break;
#endif
        if( *ptr == ';' ) break;
        if( *ptr == '{' ) break;
        *buff++ = *ptr++;
    }
    *buff = '\0';
    return( ptr );
}

static char *CollectTrapParm( char *ptr, char *start )
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
            *start++ = *ptr++;
        }
        *start = '\0';
    } else {
        ptr = GetFilename( ptr, start );
    }
    return( ptr );
}

#if defined(__QNX__) || defined(__LINUX__)
    #define IS_OPTION( c )      ((c) == '-')
#else
    #define IS_OPTION( c )      ((c) == '-' || (c) == '/')
#endif

bool ParseCommandLine( char *cmdline, char *trap, char *parm, bool *oneshot )
/***************************************************************************/
{
    char        *start;
    char        *ptr;

    *oneshot = FALSE;
    #if defined(__AXP__) && defined(__NT__)
        //NYI: temp until we can get all the unaligned stuff straightened out.
        SetErrorMode( SEM_NOALIGNMENTFAULTEXCEPT );
    #endif
    ptr = SkipSpaces( cmdline );
    if( WantUsage( ptr ) ) {
        StartupErr( ServUsage );
        return( FALSE );
    }
    trap[0] = '\0';
    while( IS_OPTION( *ptr ) ) {
        ptr = SkipSpaces( ptr + 1 );
        start = ptr;
        #undef isalpha
        while( isalpha( *ptr ) ) ++ptr;
        if( ptr == start ) {
            StartupErr( TRP_ERR_expect_option );
            return( FALSE );
        } else if( strnicmp( "trap", start, ptr - start ) == 0 ) {
            ptr = SkipSpaces( ptr );
            if( *ptr != '=' && *ptr != '#' ) {
                StartupErr( TRP_ERR_expect_equal );
                return( FALSE );
            }
            ptr = SkipSpaces( GetFilename( ptr + 1, trap ) );
            if( *ptr == ';' ) {
                start = &trap[ strlen( trap ) ];
                *start++ = ';';
                ptr = CollectTrapParm( SkipSpaces( ptr + 1 ), start );
                } else if( *ptr == '{'/*}*/ ) {
                start = &trap[ strlen( trap ) ];
                *start++ = ';';
                ptr = CollectTrapParm( ptr, start );
            }
        } else if( strnicmp( "once", start, ptr - start ) == 0 ) {
            *oneshot = TRUE;
        }
        ptr = SkipSpaces( ptr );
    }
    CollectTrapParm( ptr, parm );
    TrapVersion.remote = TRUE;
    return( TRUE );
}
