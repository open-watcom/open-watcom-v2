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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifdef __NETWARE__
    void __Init_Argv( void ) { }
    void __Fini_Argv( void ) { }
#else
#include "dll.h"        // needs to be first
#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "liballoc.h"

extern  int         __historical_splitparms;
extern  void        _Not_Enough_Memory( void );             /* 25-jul-89 */
static  unsigned    _SplitParms( CHAR_TYPE *, CHAR_TYPE **, CHAR_TYPE ** );
_WCRTLINK extern CHAR_TYPE  *__F_NAME(_LpCmdLine,_LpwCmdLine);
_WCRTLINK extern CHAR_TYPE  *__F_NAME(_LpPgmName,_LpwPgmName);
extern  int         __F_NAME(_argc,_wargc);             /* argument count  */
extern  int         __F_NAME(__argc,__wargc);           /* argument count  */
extern  CHAR_TYPE **__F_NAME(_argv,_wargv);             /* argument vector */
extern  CHAR_TYPE **__F_NAME(__argv,__wargv);           /* argument vector */
_WCRTLINK extern int        __F_NAME(___Argc,___wArgc); /* argument count */
_WCRTLINK extern CHAR_TYPE**__F_NAME(___Argv,___wArgv); /* argument vector */
static CHAR_TYPE   *__F_NAME(__CmdLine,__wCmdLine);     /* cmdline buffer */

void __F_NAME(__Init_Argv,__wInit_Argv)()
{
    unsigned    argc;           /* argument count */
    CHAR_TYPE   *endptr;        /* ptr to end of command line */
    unsigned    len;            /* length of command line */
    CHAR_TYPE   *cmdline;       /* copy of command line */
    unsigned    size;           /* amount to allocate */
    unsigned    argv_offset;    /* offset of argv in storage */

    argc = _SplitParms( __F_NAME(_LpCmdLine,_LpwCmdLine), NULL, &endptr ) + 1;
    len = endptr - __F_NAME(_LpCmdLine,_LpwCmdLine) + 1;
    argv_offset = __ALIGN_SIZE(len * sizeof(CHAR_TYPE));
    size = argv_offset + (argc+1) * sizeof(CHAR_TYPE *);
    // round up size for alignment of argv pointer
    size = __ALIGN_SIZE( size );

    #if defined(__REAL_MODE__) && defined(__BIG_DATA__)
        #if defined(__OS2_286__)
            if( _osmode == DOS_MODE ) {
                cmdline = lib_nmalloc( size );
                if( (void _WCI86NEAR *) cmdline == NULL ) {
                    cmdline = lib_malloc( size );
                }
            } else {
                cmdline = lib_malloc( size );
            }
        #else
            cmdline = lib_nmalloc( size );
            if( (void _WCI86NEAR *) cmdline == NULL ) {
                cmdline = lib_malloc( size );
            }
        #endif
    #else
        cmdline = lib_malloc( size );
    #endif

    if( cmdline ) {
        __F_NAME(_argv,_wargv) = (CHAR_TYPE **)(((char*)cmdline) + argv_offset);
        memcpy( cmdline, __F_NAME(_LpCmdLine,_LpwCmdLine), len*sizeof(CHAR_TYPE) );
        __F_NAME(_argv,_wargv)[0] = __F_NAME(_LpPgmName,_LpwPgmName);
        _SplitParms( cmdline, &__F_NAME(_argv,_wargv)[1], &endptr );
        __F_NAME(_argv,_wargv)[argc] = NULL;
        __F_NAME(_argc,_wargc) = argc;
    } else {
        __F_NAME(_argv,_wargv) = NULL;
        __F_NAME(_argc,_wargc) = 0;
    }
    __F_NAME(___Argc,___wArgc) = __F_NAME(_argc,_wargc);
    __F_NAME(___Argv,___wArgv) = __F_NAME(_argv,_wargv);
    __F_NAME(__argc,__wargc) = __F_NAME(___Argc,___wArgc);
    __F_NAME(__argv,__wargv) = __F_NAME(___Argv,___wArgv);
    __F_NAME(__CmdLine,__wCmdLine) = cmdline;   // track this since we must free it
}


static unsigned _SplitParms( CHAR_TYPE *p, CHAR_TYPE **argv, CHAR_TYPE **endptr ) {
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
    for(;;) {
        while( *p == ' ' || *p == '\t' ) {
            ++p; /* skip over blanks or tabs */
        }
        if( *p == '\0' ) break;
        /* we are at the start of a parm */
        state = QUOTE_NONE;
        if( *p == '\"' ) {
            p++;
            state = QUOTE_DELIMITER;
        }
        new = start = p;
        for(;;) {
            if( *p == '\"' ) {
                if( !__historical_splitparms ) {
                    p++;
                    if( state == QUOTE_NONE ) {
                        state = QUOTE_STARTED;
                        continue;
                    } else if( state != QUOTE_NONE ) {
                        state = QUOTE_NONE;
                        continue;
                    }
                } else {
                    if( state == QUOTE_DELIMITER ) {
                        break;
                    }
                }
            }
            if( *p == ' ' || *p == '\t' ) {
                if( state == QUOTE_NONE ) {
                    break;
                }
            }
            if( *p == '\0' ) break;
            if( *p == '\\' ) {
                if( !__historical_splitparms ) {
                    if( p[1] == '\"' ) {
                        ++p;
                        if( p[-2] == '\\' ) {
                            continue;
                        }
                    }
                } else {
                    if( state == QUOTE_DELIMITER ) {
                        if( p[1] == '\"' || p[1] == '\\' ) {
                            ++p;
                        }
                    } else {
                        if( p[1] == '\"' ) {
                            ++p;
                        }
                    }
                }
            }
            if( argv ) {
                *(new++) = *(p++);
            } else {
                ++p;
            }
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

            if( *p == '\0' ) {
                *new = '\0';
                break;
            }
            *new = '\0';
            ++p;
        } else {
            ++argc;
            if( *p == '\0' ) {
                break;
            }
            ++p;
        }
    }
    *endptr = p;
    return( argc );
}

void __F_NAME(__Fini_Argv,__wFini_Argv)()
{
    if( __F_NAME(__CmdLine,__wCmdLine) != NULL ) {
        lib_free( __F_NAME(__CmdLine,__wCmdLine) );
    }
}
#endif
