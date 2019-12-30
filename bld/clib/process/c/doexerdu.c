/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of _doexec for RDOS.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#include <rdos.h>
#include "rtdata.h"
#include "liballoc.h"
#include "_process.h"
#include "_rdos.h"

int _doexec( CHAR_TYPE *pgmname, CHAR_TYPE *cmdline,
             CHAR_TYPE *envpar,
             const CHAR_TYPE * const argv[] )
{
    int len;
    char *p;
    int ok;
    char *fname;
    char *ext;
    char *envdata;
    char *envp;
    char *ep;
    int rc = -1;
    int fh;
    char *drive;
    char *dir;
    char null_repl;

    __F_NAME(__ccmdline,__wccmdline)( pgmname, argv, cmdline, 0 );

    ok = 0;

    len = strlen( pgmname ) + 7 + _MAX_PATH2;
    p = lib_malloc( len );

    _splitpath2( pgmname, p + (len-_MAX_PATH2), &drive, &dir, &fname, &ext );
    _makepath( p, drive, dir, fname, ext );
    fh = RdosOpenFile( p, 0 );
    if( fh == 0 ) {
        if( drive[0] == '\0' && dir[0] == '\0' ) {
            envdata = getenv( "PATH" );
            if( envdata ) {
                envp = envdata;
                while( envp && !ok) {
                    ep = strchr( envp, ';' );
                    if( ep ) {
                        null_repl = *ep;
                        *ep = 0;
                    }
                    _makepath( p, NULL, envp, fname, ext );
                    fh = RdosOpenFile( p, 0 );
                    if( fh ) {
                        ok = 1;
                        RdosCloseFile( fh );
                    }
                    if( ep ) {
                        *ep = null_repl;
                        ep++;
                        envp = ep;
                    } else {
                        envp = 0;
                    }
                }
            }
        }
    } else {
        RdosCloseFile( fh );
        ok = 1;
    }

    if( ok ) {
        RdosExec( p, cmdline, 0, envpar );
    }

    lib_free( p );

    return( rc );
}
