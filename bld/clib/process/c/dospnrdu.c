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
* Description:  Implementation of _dospawn for RDOS.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <process.h>
#include <rdos.h>
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#include "_process.h"

extern char * __CreateInheritString( void );

int _dospawn( int mode, CHAR_TYPE *pgmname, CHAR_TYPE *cmdline,
                                  CHAR_TYPE *envpar, 
                                  const CHAR_TYPE * const argv[] )
{
    int tid;
    int handle;
    int wait;
    int rc = -1;
    int fh;
    int len;
    char *p;
    char *drive;
    char *dir;
    char *fname;
    char *ext;
    char *envdata;
    char *envp;
    char *ep;
    char *options;
    int ok;

    options = __CreateInheritString();

    __F_NAME(__ccmdline,__wccmdline)( pgmname, argv, cmdline, 0 );

    ok = 0;

    len = strlen( pgmname ) + 7 + _MAX_PATH2;
    p = lib_malloc( len );

    _splitpath2( pgmname, p + (len-_MAX_PATH2),
                 &drive, &dir, &fname, &ext );

    _makepath( p, drive, dir, fname, ext );
    fh = RdosOpenFile( p, 0 );
    if( fh == 0 ) {
        if( strlen( drive ) == 0 && strlen( dir ) == 0 ) {
            envdata = getenv( "PATH" );
            if( envdata ) {
                envp = envdata;
                while( envp && !ok) {
                    ep = strchr( envp, ';' );
                    if( ep ) {
                        *ep = 0;
                        ep++;
                    }
                    _makepath( p, "", envp, fname, ext );
                    fh = RdosOpenFile( pgmname, 0 );
                    if( fh ) {
                        ok = 1;
                        RdosCloseFile( fh );
                    }
                    envp = ep;
                }                
            }
        }
    } else {
        RdosCloseFile( fh );    
        ok = 1;
    }

    if( ok ) {
        handle = RdosSpawn( pgmname, cmdline, 0, envpar, options, &tid );

        if( !handle )
            ok = 0;
    }

    if( ok ) {
        if( mode == P_WAIT ) {
            wait = RdosCreateWait();
            RdosAddWaitForProcessEnd( wait, handle, 0 );
            RdosWaitForever( wait );
            rc = RdosGetProcessExitCode( handle );
            RdosCloseWait( wait );
        } 
        else
            rc = tid;
            
        RdosFreeProcessHandle( handle );                        
    }

    lib_free( options );
    lib_free( p );

    return( rc );
}
