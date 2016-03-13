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
* Description:  Internal helper routines __MkTmpFile() and __RmTmpFile().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <direct.h>
#include <string.h>
#include <process.h>
#include "rtdata.h"
#include "tmpfname.h"
#include "pathmac.h"


static unsigned __GetTmpPath( char *buf )
{
#ifndef __NETWARE__
    static char *evars[] = { "TMP", "TEMP", "TMPDIR", "TEMPDIR", "" };
    char        **evar;
    char        *tmp;
#endif
    unsigned    i;

    buf[0] = NULLCHAR;  // initialize path
#ifdef __NETWARE__
    getcwd( buf, PATH_MAX );    // No environment vars on Netware
#else
    for( evar = evars; **evar; ++evar ) {
        tmp = getenv( *evar );
        if( (tmp != NULL) && (strlen( tmp ) <= PATH_MAX) ) {
            tmp = _fullpath( buf, tmp, PATH_MAX );
            break;
        }
    }
    /*
     * If we didn't match on any environment vars, get current working dir
     */
    if( buf[0] == NULLCHAR ) {
        getcwd( buf, PATH_MAX );
    }
#endif

    // if last char is not a path delimiter then append one
    i = strlen( buf );
    if( i > 0 )
        i--;
    if( !IS_DIR_SEP( buf[i] ) ) {
        // if buf[i] is a null char then the following has no effect as planned
        i++;
        buf[i] = DIR_SEP;
        i++;
        buf[i] = NULLCHAR;
    }
    return( i );
}

static char __hex( int num )
{
    num += '0';
    if( num > '9' ) {
        num += 'a' - '0' - 10;
    }
    return( num );
}

#if defined( __NETWARE__ )
    extern int              GetThreadID( void );
    #define getuniqueid()   GetThreadID()
#elif defined( __NT__ )
    #define getuniqueid()   ((getpid() << 12) + *_threadid)
#elif defined( __OS2__ )
    #define getuniqueid()   ((getpid() << 12) + *_threadid)
#else
    #define getuniqueid()   (getpid())
#endif

void __MkTmpFile( char *buf, int num )
{
    unsigned    pid;
    unsigned    i;
    char        *ptr;

    pid = getuniqueid();
//  JBS on Win32 pid's range from 0 to n where n is not very large for
//  most systems (e.g. 500 would indicate many, many processes are active).
//  #if defined(__386__) || defined(__AXP__) || defined(__PPC__)
//      // try to use more of the 32bit pid bits
//      pid |= pid >> 16;
//  #endif

    i = __GetTmpPath( buf );
    ptr = &buf[ i ];
    ptr[0] = 't';
    for( i = 7; i != 0; i-- ) {     // JBS use 7 hex digits instead of 4
        ptr[i] = __hex( pid & 0x000F );
        pid = pid >> 4;
    }
    ptr[8] = '.';
    ptr[9] = 't';
    ptr[10] = __hex( (num >> 4) & 0x000F );
    ptr[11] = __hex( num & 0x000F );
    ptr[12] = NULLCHAR;
}

void __RmTmpFile( FILE *fp )
{
    char    name[PATH_MAX + _TMPFNAME_LENGTH + 1]; /* 02-aug-90 */

    __MkTmpFile( name, _FP_TMPFCHAR(fp) );
    remove( name );
}
