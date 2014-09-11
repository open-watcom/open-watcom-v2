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
* Description:  Implementation of spawnve() for RDOS.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <process.h>
#include <errno.h>
#include <rdos.h>
#include "liballoc.h"
#include "filestr.h"
#include "rtdata.h"
#include "seterrno.h"
#include "_process.h"

#define SPVE_NEAR _WCI86NEAR
#define LIB_ALLOC   lib_nmalloc
#define LIB_FREE    lib_nfree

/* P_OVERLAY macro expands to a variable, not a constant! */
#define OLD_P_OVERLAY   2

#define FALSE   0

static int file_exists( const CHAR_TYPE *filename )                     /* 05-apr-91 */
{
    if( __F_NAME(access,_waccess)( filename, 0 ) == 0 )
        return( 1 );
    else
        return( 0 );
}

#pragma on(check_stack);

_WCRTLINK int spawnve( int mode, const CHAR_TYPE * path,
                       const CHAR_TYPE * const argv[], const CHAR_TYPE * const in_envp[] )
{
    const CHAR_TYPE * const *envp = (const CHAR_TYPE * const *)in_envp;
    CHAR_TYPE               *envmem;
    CHAR_TYPE               *envstrings;
    unsigned                envseg;
    int                     len;
    CHAR_TYPE SPVE_NEAR     *np;
    CHAR_TYPE SPVE_NEAR     *p;
    CHAR_TYPE SPVE_NEAR     *end_of_p;
    int                     rc;
    int                     retval;
    int                     num_of_paras;       /* for environment */
    size_t                  cmdline_len;
    CHAR_TYPE SPVE_NEAR     *cmdline_mem;
    CHAR_TYPE SPVE_NEAR     *cmdline;
    CHAR_TYPE               *drive;
    CHAR_TYPE               *dir;
    CHAR_TYPE               *fname;
    CHAR_TYPE               *ext;
    
    if( mode == OLD_P_OVERLAY ) {
        rc = __F_NAME(execve,_wexecve)(path, argv, envp);
        return( rc );
    }

    retval = __F_NAME(__cenvarg,__wcenvarg)( argv, envp, &envmem,
        &envstrings, &envseg,
        &cmdline_len, FALSE );
    if( retval == -1 ) {
        return( -1 );
    }
    num_of_paras = retval;
    len = __F_NAME(strlen,wcslen)( path ) + 7 + _MAX_PATH2;
    np = LIB_ALLOC( len * sizeof( CHAR_TYPE ) );
    if( np == NULL ) {
        p = (CHAR_TYPE SPVE_NEAR *)alloca( len*sizeof(CHAR_TYPE) );
        if( p == NULL ) {
            lib_free( envmem );
            return( -1 );
        }
    } else {
        p = np;
    }
    __F_NAME(_splitpath2,_wsplitpath2)( path, p + (len-_MAX_PATH2),
                                        &drive, &dir, &fname, &ext );

    /* allocate the cmdline buffer */
    cmdline_mem = LIB_ALLOC( cmdline_len * sizeof( CHAR_TYPE ) );
    if( cmdline_mem == NULL ) {
        cmdline = (CHAR_TYPE SPVE_NEAR *)alloca( cmdline_len*sizeof(CHAR_TYPE) );
        if( cmdline == NULL ) {
            retval = -1;
            __set_errno( E2BIG );
            goto cleanup;
        }
    } else {
        cmdline = cmdline_mem;
    }
    
    __F_NAME(_makepath,_wmakepath)( p, drive, dir, fname, ext );
    __set_errno( ENOENT );
    if( ext[0] != '\0' ) {
        if( __F_NAME(stricmp,wcscmp)( ext, __F_NAME(".bat",L".bat") ) == 0 )
        {
            retval = -1; /* assume file doesn't exist */
            if( file_exists( p ) ) goto spawn_command_com;
        } else {
            __set_errno( 0 );
            /* user specified an extension, so try it */
            retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envmem, argv );
        }
    }
    else {
        end_of_p = p + __F_NAME(strlen,wcslen)( p );
        __F_NAME(strcpy,wcscpy)( end_of_p, __F_NAME(".com",L".com") );
        __set_errno( 0 );
        retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envmem, argv );
        if( _RWD_errno == ENOENT || _RWD_errno == EINVAL ) {
            __set_errno( 0 );
            __F_NAME(strcpy,wcscpy)( end_of_p, __F_NAME(".exe",L".exe") );
            retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envmem, argv );
            if( _RWD_errno == ENOENT || _RWD_errno == EINVAL ) {
                /* try for a .BAT file */
                __set_errno( 0 );
                __F_NAME(strcpy,wcscpy)( end_of_p, __F_NAME(".bat",L".bat") );
                if( file_exists( p ) ) {
spawn_command_com:
                /* the environment will have to be reconstructed */
                lib_free( envmem );
                envmem = NULL;
                __F_NAME(__ccmdline,__wccmdline)( p, argv, cmdline, 1 );
                retval = spawnl( mode, getenv("COMSPEC"),
                    "COMMAND",
                    "/c ",
                    p, cmdline, NULL );
                }
            }
        }
    }
cleanup:
    LIB_FREE( cmdline_mem );
    LIB_FREE( np );
    lib_free( envmem );
    return( retval );
}
