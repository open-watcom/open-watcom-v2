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
#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "liballoc.h"
#include <dos.h>
#include "msdos.h"
#include "rtdata.h"
#include "seterrno.h"
#include "_process.h"

#ifdef __WIDECHAR__
extern _WCRTLINK void   __create_wide_environment( void );
#endif


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
    CHAR_TYPE           **envptr,       /* o: allocated memory for env */
    CHAR_TYPE           **envstrings,   /* o: pointer to environment strings */
    unsigned            *envseg,        /* o: start of env (on para boundary) */
    size_t              *cmdline_len,   /* o: size required to hold cmd line */
    int                 exec )          /* i: TRUE if for exec */
{
    unsigned            length;
    unsigned            oamblksiz;
    CHAR_TYPE           *p;
    CHAR_TYPE _WCNEAR   *np;
    unsigned            len;
    int                 i;

    if( envp == NULL ){
#ifdef __WIDECHAR__
        if( _RWD_wenviron == NULL )
            __create_wide_environment();
#endif
        envp = (const CHAR_TYPE * const *)__F_NAME(_RWD_environ,_RWD_wenviron);
    }
    length = 0;
    if( envp != NULL ){
        for( i = 0; envp[i] != NULL; i++ ) {
            length += __F_NAME(strlen,wcslen)( envp[i] ) + 1;
        }
    }
    ++length; /* trailing \0 for env */
    if( exec ){
        /* store argv[0] at 2 bytes past end of env */
        length += 2 + __F_NAME(strlen,wcslen)( argv[0] ) + 1;
    }
    length += 15;       /* so we can start on a paragraph boundary */

    oamblksiz = _RWD_amblksiz;
    _RWD_amblksiz = 16; /* force allocation in 16 byte increments */
    p = np = lib_nmalloc( length*sizeof(CHAR_TYPE) );
    if( np == NULL ){   /* 03-aug-88 */
        p = lib_malloc( length*sizeof(CHAR_TYPE) );
        if( p == NULL ){
            __set_errno( ENOMEM );
            __set_doserrno( E_nomem );
            _RWD_amblksiz = oamblksiz;
            return( -1 );
        }
    }
    _RWD_amblksiz = oamblksiz;
    *envptr = p;
#if defined( _M_I86 ) && defined( __DOS__ )
  #if defined(__SMALL_DATA__)
    p = (char *) (((unsigned) p + 15) & 0xfff0);
  #else           /* large data models */         /* 12-aug-88 */
    p = MK_FP( FP_SEG(p), (( FP_OFF(p) + 15) & 0xfff0) );
  #endif
    {
        CHAR_TYPE _WCFAR *temp;

        temp = p;
        *envseg = FP_SEG( temp ) + FP_OFF( temp )/16;
    }
#else
    *envseg = 0;
#endif
    *envstrings = p;            /* save ptr to env strings. 07-oct-92 */
    if( envp != NULL ){
        for( i = 0; envp[i] != NULL; ++i ){
            p = stpcpy( p, envp[i] ) + 1;
        }
    }
    *p++ = '\0';
    if( exec ) {
        __F_NAME(strcpy,wcscpy)( p + 2, argv[0] );
    }

    len = 0;
    if( argv[0] != NULL ) {
        for( i = 1; argv[i] != NULL; ++i ){
            if( len != 0 ) ++len;       /* plus 1 for blank separator */
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
#else
    if( len > 126 ) {
        __set_errno( E2BIG );
        __set_doserrno( E_badenv );
        lib_free( *envptr );
        return( -1 );
    }
    len = _MAX_PATH;    /* always use _MAX_PATH chars for DOS */
#endif
    *cmdline_len = len;

    return( length / 16 );
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
	if( path[0] != '"' ) *p++ = '"';
	p = stpcpy( p, path );
	if( path[0] != '"' ) *p++ = '"';
	*p++ = ' ';
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
                if( *argv == NULL ) break;
                *p++ = ' '; /* put the space between args */
            }
        }
    }
#if defined( __OS2__ )
    *p++ = 0;
    *p = 0;
#elif defined( __NT__ )
    *p = 0;
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    *p = 0;
#else
    if( just_args ) {
        *p = 0;
    } else {
        *p = '\r';
        buffer[0] = ( p - buffer ) - 1;
    }
#endif
}
