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

#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <direct.h>
#include "stdrdos.h"
#include "debug.h"
#include "trperr.h"
#include "srvcdbg.h"
#include "pathgrp2.h"


/*
 * AccLoadProg - create a new process for debugging
 */
trap_retval TRAP_CORE( Prog_load )( void )
{
    char            *parm;
    prog_load_req   *acc;
    prog_load_ret   *ret;
    struct TDebug   *obj;
    char            name[256];
    pgroup2         pg;
    char            curdir[256];
    char            argstr[256];
    char            *src;
    char            *dst;
    int             len;
    char            ch;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    parm = GetInPtr( sizeof( *acc ) );

    _splitpath2( parm, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] != '\0' ) {
        _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
    } else {
        _makepath( name, pg.drive, pg.dir, pg.fname, "com" );
        if( access( name, 0 ) != 0 ) {
            _makepath( name, pg.drive, pg.dir, pg.fname, "exe" );
        }
    }

    ret->err = 0;
    ret->task_id = 0;
    ret->mod_handle = 0;
    ret->flags = 0;

    dst = argstr;
    src = parm;
    while( *src != 0 ) {
        ++src;
    }
    src++;

    // parm layout
    // <--parameters-->0<--program_name-->0<--arguments-->0
    //
    for( len = GetTotalSizeIn() - sizeof( *acc ) - ( src - parm ) - 1; len > 0; --len ) {
        ch = *src;
        *dst = ch;
        ++dst;
        ++src;
    }
    *dst = 0;

    if( access( name, 0 ) == 0 ) {
        obj = (struct TDebug *)malloc( sizeof( struct TDebug ) );
        InitDebug( obj, name, argstr, getcwd( curdir, 255 ) );
        WaitForLoad( obj );

        if( obj->ThreadList && obj->ModuleList ) {
            ret->task_id = obj->ThreadList->ThreadID;
            ret->mod_handle = obj->ModuleList->Handle;
            ret->flags = LD_FLAG_HAVE_RUNTIME_DLLS;
            SetCurrentDebug( obj );
        } else {
            FreeDebug( obj );
            free( obj );
            ret->err = MSG_LOAD_FAIL;
        }
    } else {
        ret->err = MSG_LOAD_FAIL;
    }
    return( sizeof( *ret ) );

}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;

    SetCurrentDebug( 0 );

    return( sizeof( *ret ) );
}
