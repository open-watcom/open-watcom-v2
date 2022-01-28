/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#ifdef __WINDOWS__
#include <dos.h>
#include "commonui.h"
#endif
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "pathgrp2.h"
#ifdef __WINDOWS__
#include "sampwin.h"
#endif

#include "clibext.h"


#if defined( __WINDOWS__ )
extern void CloseShop( void );
#endif

void GetProg( const char *cmd, size_t len )
{
    char        prog_name[_MAX_PATH];
    pgroup2     pg1;
    pgroup2     pg2;
#ifdef __WINDOWS__
    pgroup2     pg3;
    unsigned    a,b;
#endif

    memcpy( prog_name, cmd, len );
    prog_name[len] = '\0';
    _splitpath2( prog_name, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
#ifdef __NETWARE__
    if( pg1.ext[0] == '\0' )
        pg1.ext = "nlm";
#elif defined( __DOS__ ) && defined( __PHARLAP__ )
    if( pg1.ext[0] == '\0' )
        pg1.ext = "exp";
#elif !defined( __UNIX__ )
    if( pg1.ext[0] == '\0' )
        pg1.ext = "exe";
#endif
    _makepath( prog_name, pg1.drive, pg1.dir, pg1.fname, pg1.ext );

    if( pg1.drive[0] == '\0' && pg1.dir[0] == '\0' ) {
        _searchenv( prog_name, "PATH", ExeName );
    } else if( access( prog_name, R_OK ) == 0 ) {
        strcpy( ExeName, prog_name );
    }

    if( ExeName[0] == '\0' ) {
        OutputMsgParmNL( MSG_PROGRAM, prog_name );
        fatal();
    }

#ifdef __WINDOWS__
    /*
     * for windows, we need to make our current directory the
     * same as that for the EXE, since windows moves our current directory
     * to that of the sampler
     */
    _splitpath2( ExeName, pg3.buffer, &pg3.drive, &pg3.dir, NULL, NULL );
    a = tolower( pg3.drive[0] ) - 'a' + 1;
    _dos_setdrive( a, &b );
    pg3.dir[strlen( pg3.dir ) - 1] = '\0';
    chdir( pg3.dir );
#endif

    _splitpath2( SampName, pg2.buffer, &pg2.drive, &pg2.dir, &pg2.fname, &pg2.ext );

    if( pg2.fname[0] == '\0' )
        pg2.fname = pg1.fname;
    if( pg2.ext[0] == '\0' )
        pg2.ext = "smp";
    /*
     * for windows, we need to give the sample file an absolute
     * path name so that both threads of the sampler can write
     * to the sample file
     */
#ifdef __WINDOWS__
    if( pg2.drive[0] == '\0' )
        pg2.drive = pg3.drive;
    if( pg2.dir[0] == '\0' )
        pg2.dir = pg3.dir;
#endif

    _makepath( SampName, (char *)pg2.drive, (char *)pg2.dir, (char *)pg2.fname, (char *)pg2.ext );

#ifdef __WINDOWS__
    _fstrcpy( SharedMemory->SampName, SampName );
#endif
}

void fatal( void )
{
#if defined( __WINDOWS__ )
    WaitForFirst = FALSE;
    MessageLoop();
    CloseShop();
    MsgFini();
    exit( 0 );
#else
    MsgFini();
    _exit( 1 );
#endif
}
