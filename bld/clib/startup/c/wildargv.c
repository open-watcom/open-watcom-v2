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
* Description:  Command line argument wildcard expansion. Substitute for
*               the default "initargv" module.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <malloc.h>
#include <tchar.h>

#ifdef _UNICODE
    #define __F_NAME(n1,n2) n2
    #define CHAR_TYPE   wchar_t
    #define CMDLINE     _LpwCmdLine
    #define PGMNAME     _LpwPgmName
    #define _ARGC       _wargc
    #define _ARGV       _wargv
    #define ___ARGC     ___wArgc
    #define ___ARGV     ___wArgv
    #define __INIT_ARGV __wInit_Argv
    #define __FINI_ARGV __wFini_Argv
#else
    #define __F_NAME(n1,n2) n1
    #define CHAR_TYPE   char
    #define CMDLINE     _LpCmdLine
    #define PGMNAME     _LpPgmName
    #define _ARGC       _argc
    #define _ARGV       _argv
    #define ___ARGC     ___Argc
    #define ___ARGV     ___Argv
    #define __INIT_ARGV __Init_Argv
    #define __FINI_ARGV __Fini_Argv
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "initarg.h"
#include "histsplt.h"
#include "exitwmsg.h"

#ifdef __cplusplus
};
#endif


static void *_allocate( unsigned amount )
{
    void *p;
#if defined( _M_I86 )
  #if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
    void __near *np;

    p = np = _nmalloc( amount );
    if( np == NULL ) {
        p = malloc( amount );
    }
  #else
    p = malloc( amount );
  #endif
#else
    p = malloc( amount );
#endif
    if( p == NULL ) {
        _Not_Enough_Memory();
    }
    return( p );
}


static int _make_argv( TCHAR *p, TCHAR ***argv )
{
    int             argc;
    TCHAR           *start;
    TCHAR           *new_arg;
    TCHAR           wildcard;
    TCHAR           lastchar;
    struct _tdirent *dir;
    struct _tdirent *dirent;
    TCHAR           drive[_MAX_DRIVE];
    TCHAR           directory[_MAX_DIR];
    TCHAR           name[_MAX_FNAME];
    TCHAR           extin[_MAX_EXT];
    TCHAR           pathin[_MAX_PATH];
    enum QUOTE_STATE {
        QUOTE_NONE,         /* no " active in current parm */
        QUOTE_DELIMITER,    /* " was first char and must be last */
        QUOTE_STARTED       /* " was seen, look for a match */
    };
    enum QUOTE_STATE state;

    argc = 1;
    for( ;; ) {
        while( *p == ' ' || *p == '\t' ) {
            ++p;    /* skip over blanks or tabs */
        }
        if( *p == '\0' ) {
            break;
        }
        /* we are at the start of a parm */
        wildcard = 0;
        state = QUOTE_NONE;
        if( *p == '\"' ) {
            p++;
            state = QUOTE_DELIMITER;
        }
        new_arg = start = p;
        for( ;; ) {
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
            } else if( *p == '?' || *p == '*' ) {
                if( state == QUOTE_NONE ) {
                    wildcard = 1;
                }
            }
            *new_arg++ = *p++;
        }
        *argv = (TCHAR **) realloc( *argv, (argc+2) * sizeof( TCHAR * ) );
        if( *argv == NULL ) {
            _Not_Enough_Memory();
        }
        (*argv)[ argc ] = start;
        ++argc;
        lastchar = *p;
        *new_arg = '\0';
        ++p;
        if( wildcard ) {
            /* expand file names */
            dir = _topendir( start );
            if( dir != NULL ) {
                --argc;
                _tsplitpath( start, drive, directory, name, extin );
                for( ;; ) {
                    dirent = _treaddir( dir );
                    if( dirent == NULL ) {
                        break;
                    }
                    if( dirent->d_attr &
                      (_A_HIDDEN+_A_SYSTEM+_A_VOLID+_A_SUBDIR) ) {
                        continue;
                    }
                    _tsplitpath( dirent->d_name, NULL, NULL, name, extin );
                    _tmakepath( pathin, drive, directory, name, extin );
                    *argv = (TCHAR **) realloc( *argv, (argc+2) * sizeof( TCHAR * ) );
                    if( *argv == NULL ) {
                        _Not_Enough_Memory();
                    }
                    new_arg = (TCHAR *)_allocate( (_tcslen( pathin ) + 1) * sizeof( TCHAR ) );
                    _tcscpy( new_arg, pathin );
                    (*argv)[argc++] = new_arg;
                }
                _tclosedir( dir );
            }
        }
        if( lastchar == '\0' ) {
            break;
        }
    }
    return( argc );
}


#ifdef __cplusplus
extern "C"
#endif
void __INIT_ARGV( void )
{
    TCHAR *cln;

    _ARGV = (TCHAR **)_allocate( 2 * sizeof( TCHAR * ) );
    _ARGV[0] = PGMNAME;     /* fill in program name */
    cln = (TCHAR *)_allocate( (_tcslen( CMDLINE ) + 1) * sizeof( TCHAR ) );
    _tcscpy( cln, CMDLINE );
    _ARGC = _make_argv( cln, &_ARGV );
    _ARGV[_ARGC] = NULL;
    ___ARGC = _ARGC;
    ___ARGV = _ARGV;
    __targc = ___ARGC;      /* from stdlib.h */
    __targv = ___ARGV;      /* from stdlib.h */
}

#ifdef __cplusplus
extern "C"
#endif
void __FINI_ARGV( void )
{
    if( _ARGV != NULL ) {
        if( (_ARGC > 1) && (_ARGV[1] != NULL) ) {
            free( _ARGV[1] );
        }
        free( _ARGV );
    }
}
