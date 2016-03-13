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
* Description:  Functions to set up argc and argv parameters of main(), etc.
*
****************************************************************************/


#ifdef __NETWARE__
    void __Init_Argv( void ) { }
    void __Fini_Argv( void ) { }
#else
#include "dll.h"        // needs to be first
#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "liballoc.h"
#include "initarg.h"
#include "rtdata.h"

extern  int         __historical_splitparms;
extern  void        _Not_Enough_Memory( void );             /* 25-jul-89 */
static  unsigned    _SplitParms(int, CHAR_TYPE *, CHAR_TYPE **, CHAR_TYPE ** );
_WCRTDATA static CHAR_TYPE  *__F_NAME(__CmdLine,__wCmdLine);    /* cmdline buffer */

_WCRTLINK void *__F_NAME( _getargv, _wgetargv )(
        int historical, CHAR_TYPE *exe, CHAR_TYPE *cmd,
        int *pargc, CHAR_TYPE ***pargv );

void __F_NAME(__Init_Argv,__wInit_Argv)( void )
{
    __F_NAME( __CmdLine, __wCmdLine ) = __F_NAME( _getargv, _wgetargv )(
        __historical_splitparms,
        __F_NAME( _LpPgmName, _LpwPgmName ), __F_NAME( _LpCmdLine, _LpwCmdLine ),
        &__F_NAME( _argc, _wargc ), &__F_NAME( _argv, _wargv ) );

    __F_NAME( __argc, __wargc )   = __F_NAME( _argc, _wargc );
    __F_NAME( ___Argc, ___wArgc ) = __F_NAME( _argc, _wargc );
    __F_NAME( __argv, __wargv )   = __F_NAME( _argv, _wargv );
    __F_NAME( ___Argv, ___wArgv ) = __F_NAME( _argv, _wargv );
}

_WCRTLINK void *__F_NAME( _getargv, _wgetargv )(
        int historical, CHAR_TYPE *exe, CHAR_TYPE *cmd,
        int *pargc, CHAR_TYPE ***pargv )
{
    unsigned    argc;           /* argument count */
    CHAR_TYPE   **argv;         /* Actual arguments */
    CHAR_TYPE   *endptr;        /* ptr to end of command line */
    unsigned    len;            /* length of command line */
    CHAR_TYPE   *cmdline;       /* copy of command line */
    unsigned    size;           /* amount to allocate */
    unsigned    argv_offset;    /* offset of argv in storage */
#if defined(__REAL_MODE__) && defined(__BIG_DATA__)
    void _WCI86NEAR *ncmd;      /* near cmdline, if we can get it */
#endif

    argc = _SplitParms( historical, cmd, NULL, &endptr ) + 1;
    len = (unsigned) ( endptr - cmd ) + 1;
    argv_offset = __ALIGN_SIZE(len * sizeof(CHAR_TYPE));
    size = argv_offset + (argc+1) * sizeof(CHAR_TYPE *);
    // round up size for alignment of argv pointer
    size = __ALIGN_SIZE( size );

    #if defined(__REAL_MODE__) && defined(__BIG_DATA__)
        #if defined(__OS2_286__)
            if( _RWD_osmode == DOS_MODE ) {
                cmdline = ncmd = lib_nmalloc( size );
                if( ncmd == NULL ) {
                    cmdline = lib_malloc( size );
                }
            } else {
                cmdline = lib_malloc( size );
            }
        #else
            cmdline = ncmd = lib_nmalloc( size );
            if( ncmd == NULL ) {
                cmdline = lib_malloc( size );
            }
        #endif
    #else
        cmdline = lib_malloc( size );
    #endif
    argv = NULL;
    argc = 0;
    if( cmdline ) {
        memcpy( cmdline, cmd, len * sizeof(CHAR_TYPE) );
        argv = (void *) ( ( ( char*) cmdline ) + argv_offset );
        argv[0] = exe;
        argc = _SplitParms( historical, cmdline, argv + 1, &endptr ) + 1;
        argv[argc] = NULL;
    }
    *pargc = argc;
    *pargv = argv;
    return( cmdline );
}


static unsigned _SplitParms( int historical, CHAR_TYPE *p, CHAR_TYPE **argv, CHAR_TYPE **endptr )
{
    register unsigned argc;
    register CHAR_TYPE *start;
    register CHAR_TYPE *new;
    enum QUOTE_STATE {
        QUOTE_NONE,             /* no " active in current parm */
        QUOTE_DELIMITER,        /* " was first char and must be last */
        QUOTE_STARTED   /* " was seen, look for a match */
    };
    register enum QUOTE_STATE state;

    argc = 0;
    for( ;; ) {
        while( *p == STRING( ' ' ) || *p == STRING( '\t' ) ) {
            ++p; /* skip over blanks or tabs */
        }
        if( *p == NULLCHAR )
            break;
        /* we are at the start of a parm */
        state = QUOTE_NONE;
        if( *p == STRING( '\"' ) ) {
            p++;
            state = QUOTE_DELIMITER;
        }
        new = start = p;
        for( ;; ) {
            if( *p == STRING( '\"' ) ) {
                if( !historical ) {
                    p++;
                    if( state == QUOTE_NONE ) {
                        state = QUOTE_STARTED;
                    } else {
                        state = QUOTE_NONE;
                    }
                    continue;
                } else {
                    if( state == QUOTE_DELIMITER ) {
                        break;
                    }
                }
            }
            if( *p == STRING( ' ' ) || *p == STRING( '\t' ) ) {
                if( state == QUOTE_NONE ) {
                    break;
                }
            }
            if( *p == NULLCHAR )
                break;
            if( *p == STRING( '\\' ) ) {
                if( !historical ) {
                    if( p[1] == STRING( '\"' ) ) {
                        ++p;
                        if( p[-2] == STRING( '\\' ) ) {
                            continue;
                        }
                    }
                } else {
                    if( p[1] == STRING( '\"' ) || p[1] == STRING( '\\' ) && state == QUOTE_DELIMITER ) {
                        ++p;
                    }
                }
            }
            if( argv ) {
                *(new++) = *p;
            }
            ++p;
        }
        if( argv ) {
            argv[ argc ] = start;
            ++argc;

            /*
              The *new = '\0' is req'd in case there was a \" to "
              translation. It must be after the *p check against
              '\0' because new and p could point to the same char
              in which case the scan would be terminated too soon.
            */

            if( *p == NULLCHAR ) {
                *new = NULLCHAR;
                break;
            }
            *new = NULLCHAR;
            ++p;
        } else {
            ++argc;
            if( *p == NULLCHAR ) {
                break;
            }
            ++p;
        }
    }
    *endptr = p;
    return( argc );
}

void __F_NAME(__Fini_Argv,__wFini_Argv)( void )
{
    if( __F_NAME(__CmdLine,__wCmdLine) != NULL ) {
        lib_free( __F_NAME(__CmdLine,__wCmdLine) );
    }
}
#endif
