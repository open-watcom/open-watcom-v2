/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <process.h>
#include <rdos.h>
#include "liballoc.h"
#include "filestr.h"
#include "_process.h"
#include "thread.h"

#define LIB_ALLOC   lib_nmalloc
#define LIB_FREE    lib_nfree

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
    CHAR_TYPE               *_envptr;       /* environment ptr (unaligned) */
    CHAR_TYPE               *envptr;        /* environment ptr (DOS 16-bit aligned to para) */
    unsigned                envseg;         /* environment segment (DOS 16-bit normalized, zero for others) */
    int                     len;
    CHAR_TYPE               *np;
    CHAR_TYPE               *p;
    CHAR_TYPE               *end_of_p;
    int                     rc;
    int                     retval;
    size_t                  cmdline_len;
    CHAR_TYPE               *cmdline_mem;
    CHAR_TYPE               *cmdline;
    CHAR_TYPE               *drive;
    CHAR_TYPE               *dir;
    CHAR_TYPE               *fname;
    CHAR_TYPE               *ext;

    if( mode == P_OVERLAY ) {
        rc = __F_NAME(execve,_wexecve)( path, argv, envp );
        return( rc );
    }

    retval = __F_NAME(__cenvarg,__wcenvarg)( argv, envp, &_envptr, &envptr, &envseg, &cmdline_len, false );
    if( retval == -1 ) {
        return( -1 );
    }
    len = __F_NAME(strlen,wcslen)( path ) + 7 + _MAX_PATH2;
    np = LIB_ALLOC( len * sizeof( CHAR_TYPE ) );
    if( np == NULL ) {
        p = (CHAR_TYPE *)alloca( len * sizeof( CHAR_TYPE ) );
        if( p == NULL ) {
            lib_free( _envptr );
            return( -1 );
        }
    } else {
        p = np;
    }
    __F_NAME(_splitpath2,_wsplitpath2)( path, p + ( len - _MAX_PATH2 ), &drive, &dir, &fname, &ext );

    /* allocate the cmdline buffer */
    cmdline_mem = LIB_ALLOC( cmdline_len * sizeof( CHAR_TYPE ) );
    if( cmdline_mem == NULL ) {
        cmdline = (CHAR_TYPE *)alloca( cmdline_len * sizeof( CHAR_TYPE ) );
        if( cmdline == NULL ) {
            retval = -1;
            lib_set_errno( E2BIG );
        }
    } else {
        cmdline = cmdline_mem;
    }

    if( cmdline != NULL ) {
        __F_NAME(_makepath,_wmakepath)( p, drive, dir, fname, ext );
        lib_set_errno( ENOENT );
        if( ext[0] != NULLCHAR ) {
            if( __F_NAME(_stricmp,_wcsicmp)( ext, STRING( ".bat" ) ) == 0 )
            {
                retval = -1; /* assume file doesn't exist */
                if( file_exists( p ) ) {
                    /* the environment will have to be reconstructed */
                    lib_free( _envptr );
                    _envptr = NULL;
                    __F_NAME(__ccmdline,__wccmdline)( p, argv, cmdline, 1 );
                    retval = spawnl( mode, getenv( STRING( "COMSPEC" ) ),
                        STRING( "COMMAND" ),
                        STRING( "/c " ),
                        p, cmdline, NULL );
                }
            } else {
                lib_set_errno( 0 );
                /* user specified an extension, so try it */
                retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envptr, argv );
            }
        } else {
            end_of_p = p + __F_NAME(strlen,wcslen)( p );
            __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".com" ) );
            lib_set_errno( 0 );
            retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envptr, argv );
            if( retval == -1 || lib_get_errno() == ENOENT || lib_get_errno() == EINVAL ) {
                lib_set_errno( 0 );
                __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".exe" ) );
                retval = __F_NAME(_dospawn,_wdospawn)( mode, p, cmdline, envptr, argv );
                if( retval == -1 || lib_get_errno() == ENOENT || lib_get_errno() == EINVAL ) {
                    /* try for a .BAT file */
                    lib_set_errno( 0 );
                    __F_NAME(strcpy,wcscpy)( end_of_p, STRING( ".bat" ) );
                    if( file_exists( p ) ) {
                        /* the environment will have to be reconstructed */
                        lib_free( _envptr );
                        _envptr = NULL;
                        __F_NAME(__ccmdline,__wccmdline)( p, argv, cmdline, 1 );
                        retval = spawnl( mode, getenv( STRING( "COMSPEC" ) ),
                            STRING( "COMMAND" ),
                            STRING( "/c " ),
                            p, cmdline, NULL );
                    }
                }
            }
        }
    }
    LIB_FREE( cmdline_mem );
    LIB_FREE( np );
    lib_free( _envptr );
    return( retval );
}
