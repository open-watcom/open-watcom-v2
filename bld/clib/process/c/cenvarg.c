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
* Description:  Build environment and arguments for new process.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "dll.h"        // needs to be first
#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "rtdata.h"
#include "rterrno.h"
#include "liballoc.h"
#include "msdos.h"
#include "_process.h"
#include "thread.h"
#include "_environ.h"


static CHAR_TYPE *stpcpy( CHAR_TYPE *dest, const CHAR_TYPE *src )
{
    while( *dest = *src ) {
        ++dest;
        ++src;
    }
    return( dest );
}


int __F_NAME(__cenvarg,__wcenvarg)(
/*
 *  Build environment and command line for new process.  Length of environment
 *  (in bytes) is returned on success.  -1 is returned on failure.
 */
    const CHAR_TYPE     *const argv[],  /* i: arguments for new process */
    const CHAR_TYPE     *const envp[],  /* i: env strings for new process */
    CHAR_TYPE           **_envptr,      /* o: environment ptr (unaligned) */
    CHAR_TYPE           **envptr,       /* o: environment ptr (DOS 16-bit aligned to para) */
    unsigned            *envseg,        /* o: environment segment (DOS 16-bit normalized, zero for others) */
    size_t              *cmdline_len,   /* o: size required to hold cmd line */
    int                 exec )          /* i: TRUE if for exec */
{
    unsigned            length;         /* environment length in bytes */
    unsigned            old_amblksiz;
    CHAR_TYPE           *p;
    CHAR_TYPE _WCNEAR   *np;
    unsigned            len;            /* command line length in characters */
    int                 i;

#if !defined( __DOS_086__ )
    exec = exec;
#endif
    if( envp == NULL ) {
#ifdef __WIDECHAR__
        if( _RWD_wenviron == NULL )
            __create_wide_environment();
#endif
        envp = (const CHAR_TYPE * const *)__F_NAME(_RWD_environ,_RWD_wenviron);
    }
    length = 0;
    if( envp != NULL ) {
        for( i = 0; envp[i] != NULL; i++ ) {
            length += ( __F_NAME(strlen,wcslen)( envp[i] ) + 1 ) * sizeof( CHAR_TYPE );
        }
    }
    length += sizeof( CHAR_TYPE ); /* trailing \0 for env */
#if defined( __DOS_086__ )
    if( exec ) {
        /* store argv[0] at 2 bytes past end of env */
        length += 2;
        length += strlen( argv[0] ) + 1;
    }
#endif
    /* round environment length to para */
    length = __ROUND_UP_SIZE_PARA( length );
#if defined( __DOS_086__ )
    /* add space for pointer alignment */
    /* so we can start on a paragraph boundary even if memory pointer is not aligned to para */
    length += 15;
#endif
    /* allocate space for new environment */
    old_amblksiz = _RWD_amblksiz;
    _RWD_amblksiz = 16; /* force allocation in 16 byte increments */
    p = np = lib_nmalloc( length );
    if( np == NULL ) {
        p = lib_malloc( length );
        if( p == NULL ) {
            _RWD_errno = ENOMEM;
            _RWD_doserrno = E_nomem;
            _RWD_amblksiz = old_amblksiz;
            return( -1 );
        }
    }
    _RWD_amblksiz = old_amblksiz;
    *_envptr = p;
#if defined( __DOS_086__ )
    /* align DOS 16-bit environment pointer to para boundary */
  #if defined(__SMALL_DATA__)
    p = (char *)__ROUND_UP_SIZE_PARA( FP_OFF( p ) );
  #else     /* __LARGE_DATA__ */
    p = MK_FP( FP_SEG( p ), __ROUND_UP_SIZE_PARA( FP_OFF( p ) ) );
  #endif
    /* normalize DOS 16-bit aligned environment pointer to segment */
    *envseg = FP_SEG( p ) + __ROUND_DOWN_SIZE_TO_PARA( FP_OFF( p ) );
    /* correct environment length (subtract aditional pointer alignment space) */
    length -= 15;
#else
    *envseg = 0;
#endif
    *envptr = p;            /* save ptr to env strings. */
    if( envp != NULL ) {
        for( i = 0; envp[i] != NULL; ++i ) {
            p = stpcpy( p, envp[i] ) + 1;
        }
    }
    *p++ = NULLCHAR;
#if defined( __DOS_086__ )
    if( exec ) {
        *p++ = 1;
        *p++ = 0;
        strcpy( p, argv[0] );
    }
#endif
    len = 0;
    if( argv[0] != NULL ) {
        for( i = 1; argv[i] != NULL; ++i ) {
            if( len != 0 )
                ++len;       /* plus 1 for blank separator */
            len += __F_NAME(strlen,wcslen)( argv[i] );
        }
    }
#if defined( __NT__ )
    // we are going to add quotes around program name (argv[0])
    len += _MAX_PATH2 + 3;
#elif defined( __OS2__ )
    len += _MAX_PATH2 + 1;
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    len += _MAX_PATH2 + 1;
#else       /* __DOS__ */
    if( len > 126 ) {
        _RWD_errno = E2BIG;
        _RWD_doserrno = E_badenv;
        lib_free( *_envptr );
        return( -1 );
    }
    len = _MAX_PATH;    /* always use _MAX_PATH chars for DOS */
#endif
    *cmdline_len = len;

    /* convert environment length in bytes to length in para */
    return( __ROUND_DOWN_SIZE_TO_PARA( length ) );
}


void __F_NAME(__ccmdline,__wccmdline)( CHAR_TYPE *path, const CHAR_TYPE * const argv[],
                                      CHAR_TYPE *buffer, int just_args )
{
/*
 * path is the name of the program we are about to spawn
 * argv is the argument array
 * buffer points to memory large enough to hold the resulting command line
 * just_args means build a command line with arguments only
 *
 * Each OS wants the command line to look a little bit different.
 */

    CHAR_TYPE *p;

    p = buffer;
    if( ! just_args ) {
#if defined( __OS2__ )
        path = path;
        /* OS/2 wants:  argv[0] '\0' arguments '\0' '\0'
           IMPORTANT: it is vital that there are 2 trailing '\0's
           for DosExecPgm()
         */
        p = stpcpy( p, argv[0] ) + 1;
#elif defined( __NT__ )
        /* NT wants: "path" ' ' arguments '\0'
           IMPORTANT: use of quotation marks fixes misparsing of spacey
           file names like "c:\program files\common\tool.exe". Also overcomes
           bug in WinXP SP2 (beta).
        */
        if( path[0] != STRING( '"' ) )
            *p++ = STRING( '"' );
        p = stpcpy( p, path );
        if( path[0] != STRING( '"' ) )
            *p++ = STRING( '"' );
        *p++ = STRING( ' ' );
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
        /* RDOS wants: arguments '\0' */
        path = path;
#else
        path = path;
        /* DOS wants: len_byte arguments '\r' */
        p++;
#endif
    }
    if( argv[0] != NULL ) {
        ++argv;
        if( *argv != NULL ) {
            for(;;) {
                p = stpcpy( p, *argv );
                ++argv;
                if( *argv == NULL )
                    break;
                *p++ = STRING( ' ' ); /* put the space between args */
            }
        }
    }
#if defined( __OS2__ )
    *p++ = NULLCHAR;
    *p = NULLCHAR;
#elif defined( __NT__ )
    *p = NULLCHAR;
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    *p = NULLCHAR;
#else
    if( just_args ) {
        *p = NULLCHAR;
    } else {
        *p = STRING( '\r' );
        buffer[0] = ( p - buffer ) - 1;
    }
#endif
}
