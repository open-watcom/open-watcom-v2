/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wmake input analysis
*
****************************************************************************/


#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#include <ctype.h>
#include "make.h"
#include "mtarget.h"
#include "macros.h"
#include "mcache.h"
#include "mmemory.h"
#include "mexec.h"
#include "mmisc.h"
#include "mparse.h"
#include "mrcmsg.h"
#include "msg.h"
#include "msuffix.h"
#include "mupdate.h"
#include "mvecstr.h"
#include "mautodep.h"

#include "clibint.h"
#include "clibext.h"


STATIC TLIST    *mustTargs;         /* targets we must update           */
STATIC TLIST    *firstTargFound;    /* first targets we ever found      */
STATIC NODE     *filesToDo;         /* pointers into argv to -f files   */

static void parseString( const char *s )
{
    TLIST       *tlist;

    InsString( s, false );
    tlist = Parse();
    FreeTList( tlist );
}

#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC void doBuiltIns( const char *makeopts )
/*********************************************
 * perform the builtin commands
 */
{
    char        buf[2048];
    char        *cpy;
    const char  FAR *suffices;
    const char  FAR *builtins;

    if( !Glob.overide ) {
        DoingBuiltIn = true;
        cpy = MallocSafe( 2048 + strlen( makeopts ) );
        FmtStr( buf, "%F", BuiltIns );
        FmtStr( cpy, buf, makeopts );
        parseString( cpy );
        strcpy(cpy, "MAKE=" );
        if( _cmdname( cpy + sizeof( "MAKE=" ) - 1 ) == NULL ) {
            strcat( cpy, "wmake" );
        }
        parseString( cpy );
        if( Glob.compat_nmake || Glob.compat_unix ) {
            FmtStr( cpy, "%%MAKEFLAGS=$(%%MAKEFLAGS) %s", makeopts );
            parseString( cpy );
        }
        if( Glob.compat_nmake ) {
            suffices = MSSuffixList;
            builtins = MSBuiltIn;
        } else if( Glob.compat_posix ) {
            suffices = POSIXSuffixList;
            builtins = POSIXBuiltIn;
        } else if( Glob.compat_unix ) {
            suffices = UNIXSuffixList;
            builtins = UNIXBuiltIn;
        } else {
            suffices = SuffixList;
            builtins = NULL;
        }
        // suffixes must be parsed before builtins
        if( suffices != NULL ) {
            FmtStr( cpy, "%F", suffices );
            parseString( cpy );
        }
        if( builtins != NULL ) {
            FmtStr( buf, "%F", builtins );
            FmtStr( cpy, buf, makeopts );
            parseString( cpy );
        }
        FreeSafe( cpy );
        DoingBuiltIn = false;
    }
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif

static void setFirstTarget( TLIST *potential_first )
/**************************************************/
{
    if( firstTargFound != NULL || potential_first == NULL ) {
        if( potential_first != NULL ) {
            FreeTList( potential_first );
        }
        return;
    }
    /*  Note all first targets must not have attribute explicit */
    firstTargFound = potential_first;
}

STATIC void handleMacroDefn( const char *buf )
/*********************************************
 * Can't use Parse() at this point because we need readonly macros, so we
 * simply use LexToken().
 * This statement is not true for microsoft option.  In microsoft, all the
 * macros even the one on the command line can be overwritten
 */
{
    char        *p;
    char        *q;

    assert( buf != NULL );

    q = StrDupSafe( buf );    /* we need our own copy */

    p = strpbrk( q, "#=" );
    assert( p != NULL );
    *p = '=';                   /* lex doesn't recognize '#' */

    InsString( q, false );     /* put arg into stream */
    while( LexToken( LEX_PARSER ) != TOK_END ) {
        /* NOP - eat all the characters */
    }

    if( Glob.compat_nmake ) {
        /* Insert twice because in nmake declaring a macro in the command line */
        /* is equivalent to declaring one as is and one that is all upper case */
        /* Approximately so. we cater for foo meaning FOO but not FoO */
        /* This is no problem! In make, foo=bar only sets foo and FOO */
        while( --p >= q ) {
            *p = ctoupper( *p );
        }

        InsString( q, false );     /* put arg into stream */
        while( LexToken( LEX_PARSER ) != TOK_END ) {
            /* NOP - eat the characters. Needs own eater. */
        }
    }
    FreeSafe( q );
}


STATIC void handleTarg( const char *buf )
/***************************************/
{
    assert( buf != NULL );

    /* if it is not a valid target name, Update() won't be able to make it */
    WildTList( &mustTargs, buf, false, true );
}


STATIC void checkCtrl( const char *p )
/************************************/
{
    // p != NULL is checked by caller
    while( *p != NULLCHAR ) {       // scan for control characters
        if( !cisprint( *p ) ) {
            PrtMsg( FTL | CTRL_CHAR_IN_CMD, *p );
            ExitFatal();
            // never return
        }
        ++p;
    }
}


STATIC char *procFlags( char const * const *argv, const char **log_name )
/************************************************************************
 * process the flags, macro=defn, and targets from the command line
 * writes targets to mustTargs, files to fileToDo, and defines macros
 * it is important to have the ms switch first to have correct functionality
 * of some features in microsoft compatability
 */
{
    char        select;         /* - or swchar (*argv)[0]       */
    char        option;         /* the option (*argv)[1]        */
    const char  *p;             /* working pointer to *argv     */
    NODE        *new;           /* for adding a new file        */
    bool        options[256] = { false };

#define SET_OPTION(o)   options[(unsigned char)(o) | 0x20] = true
#define CHK_OPTION(o)   options[(unsigned char)(o)]

    if( (p = argv[1]) != NULL ) {
        if( strcmp( p, "?" ) == 0 || ((p[0] == '-' || p[0] == Glob.swchar) && strcmp( p + 1, "?" ) == 0) ) {
            Usage();
            // never return
        }
    }

    Glob.macreadonly = true;
    while( (p = *++argv) != NULL ) {
        checkCtrl( p );
        select = p[0];
        option = ctolower( p[1] );
        if( select == '-' || select == Glob.swchar ) {
            if( option != NULLCHAR && p[2] == NULLCHAR ) {
                switch( option ) {
                case '?':
                    Usage();
                    // never return
                    break;
                case 'a':   Glob.all            = true; break;
                case 'b':   Glob.block          = true; break;
                case 'c':   Glob.nocheck        = true; break;
                case 'd':   Glob.debug          = true; break;
                case 'e':   Glob.erase          = true; break;
                case 'h':   Glob.noheader       = true; break;
                case 'i':   Glob.ignore         = true; break;
                case 'j':   Glob.rcs_make       = true; break;
                case 'k':   Glob.cont           = true; break;
                case 'm':   Glob.nomakeinit     = true; break;
                case 'n':   Glob.noexec         = true; break;
                case 'o':   Glob.optimize       = true; break;
                case 'p':   Glob.print          = true; break;
                case 'q':   Glob.query          = true; break;
                case 'r':   Glob.overide        = true; break;
                case 's':   Glob.silent         = true; break;
                case 't':   Glob.touch          = true; break;
                case 'u':   Glob.compat_unix    = true; break;
                case 'v':   Glob.verbose        = true; break;
                case 'w':   Glob.auto_depends   = true; break;
#ifdef CACHE_STATS
                case 'x':   Glob.cachestat      = true; break;
#endif
                case 'y':   Glob.show_offenders = true; break;
                case 'z':   Glob.hold           = true; break;
                    /* these options require a filename */
                case 'f':
                case 'l':
                    if( (p = *++argv) == NULL ) {
                        PrtMsg( ERR | INVALID_FILE_OPTION, select, option );
                        Usage();
                        // never return
                    }
                    checkCtrl( p );
                    if( option == 'f' ) {
                        if( (p[0] == '-') && (p[1] == NULLCHAR) ) {
                            // stdin
                        } else if( (p[0] == '-') || (p[0] == Glob.swchar) ) {
                            PrtMsg( ERR | INVALID_FILE_OPTION, select, option );
                            Usage();
                            // never return
                        }
                        new = MallocSafe( sizeof( *new ) );
                        new->name = (char *)p;
                        new->next = filesToDo;
                        filesToDo = new;
                    } else
                        *log_name = p;
                    break;
                default:
                    PrtMsg( ERR | INVALID_OPTION, select, option );
                    Usage();
                    // never return
                    break;
                }
                SET_OPTION( option );
                continue;
            }
            if( p[3] == NULLCHAR ) {
#if defined( __DOS__ )
                if( option == 'e' && ctolower( p[2] ) == 'r' ) {
                    Glob.redir_err = true;
                    SET_OPTION( option );
                    continue;
                }
#endif
                if( option == 'm' && ctolower( p[2] ) == 's' ) {
                    Glob.compat_nmake = true;
                    Glob.nocheck   = true;
                    SET_OPTION( option );
                    continue;
                }
                if( option == 's' && ctolower( p[2] ) == 'n' ) {
                    Glob.silentno  = true;
                    SET_OPTION( option );
                    continue;
                }
                if( option == 'u' && ctolower( p[2] ) == 'x' ) {
                    /* POSIX compatibility */
                    Glob.compat_posix = true;
                    Glob.compat_unix = true;
                    Glob.nomakeinit = true;
                    Glob.nocheck    = true;
                    SET_OPTION( option );
                    continue;
                }
            }
        }
        if( strpbrk( p, "=#" ) != NULL ) {     /* is macro=defn */
            handleMacroDefn( p );
        } else {                /* is a target */
            handleTarg( p );
        }
    } // while( *++argv != NULL )

    if( Glob.compat_nmake && Glob.compat_unix ) {
        PrtMsg( ERR | INCOMPATIBLE__OPTIONS );
        Usage();
        // never return
    }

    Glob.macreadonly = false;

    {
        char    *makeopts;
        char    default_option[] = " -?";

        // 120 allows for 30 options.
        makeopts = MallocSafe( 120 + strlen( *log_name ) + 1 + 1 );
        makeopts[0] = NULLCHAR;
        for( option = 'a'; option <= 'z'; ++option ) {
            if( CHK_OPTION( option ) ) {
                switch( option ) {
#if defined( __DOS__ )
                case 'e':
                    if( Glob.erase ) {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -e" : "-e" );
                    } else {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -er" : "-er" );
                    }
                    break;
#endif
                case 'f':
                case 'n':
                    break;
                case 'l':
                    strcat( makeopts, *makeopts != NULLCHAR ? " -l " : "-l " );
                    strcat( makeopts, *log_name );
                    break;
                case 'm':
                    if( Glob.nomakeinit ) {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -m" : "-m" );
                    }
                    if( Glob.compat_nmake ) {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -ms" : "-ms" );
                    }
                    break;
                case 's':
                    if( Glob.silentno ) {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -sn" : "-sn" );
                    } else {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -s" : "-s" );
                    }
                    break;
                case 'u':
                    if( Glob.compat_posix ) {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -ux" : "-ux" );
                    } else {
                        strcat( makeopts, *makeopts != NULLCHAR ? " -u" : "-u" );
                    }
                    break;
                default:
                    default_option[2] = option;
                    p = default_option;
                    if( *makeopts == NULLCHAR )
                        ++p;
                    strcat( makeopts, p );
                }
            }
        }
        return( makeopts );
    }

#undef SET_OPTION
#undef CHK_OPTION

}


STATIC const char *procLogName( const char * const *argv )
/*********************************************************
 Find log file name
 */
{
    const char *p;      /* working pointer to *argv                 */

    while( *++argv != NULL ) {
        p = *argv;
        if( ((p[0] == '-') || (p[0] == Glob.swchar)) &&
                (ctolower( p[1] ) == 'l') && (p[2] == NULLCHAR) ) {
            return( ((p = *++argv) == NULL || (p[0] == '-')
                || (p[0] == Glob.swchar)) ? NULL : p );
        }
    }
    return( NULL );
}

STATIC void parseFiles( void )
/*****************************
 * Parse()s each of the files in filesToDo
 * post:    filesToDo == NULL
 */
{
    const char  *p;
    NODE        *cur;
    NODE        *newhead;
    bool        ok;

    Glob.preproc = true;            /* turn on preprocessor */
    if( !Glob.nomakeinit ) {        /* process makeinit */
        if( Glob.compat_nmake ) {
            ok = InsFile( TOOLSINI_NAME, true );
        } else {
            ok = InsFile( MAKEINIT_NAME, true );
        }
        if( ok ) {
            setFirstTarget( Parse() );
            if( firstTargFound != NULL ) {
                PrtMsg( WRN | MAKEINIT_HAS_TARGET );
            }
        }
    }

    if( filesToDo == NULL ) {
        ok = InsFile( MAKEFILE_NAME, false );
        if( ok ) {
            setFirstTarget( Parse() );
#ifdef MAKEFILE_ALT
        } else {
            ok = InsFile( MAKEFILE_ALT, false );
            if( ok ) {
                setFirstTarget( Parse() );
            }
#endif
        }
    } else {
        newhead = NULL;     /* reverse order of files stacked by procFlags */
        while( filesToDo != NULL ) {
            cur = filesToDo;
            filesToDo = cur->next;

            cur->next = newhead;
            newhead = cur;
        }

        while( newhead != NULL ) {
            cur = newhead;
            newhead = cur->next;
            p = cur->name;
            FreeSafe( cur );
            if( p[0] == '-' && p[1] == NULLCHAR ) { /* handle -f - */
                InsOpenFile( stdin );
                ok = true;
            } else {
                ok = InsFile( p, false );
            }
            if( ok ) {
                setFirstTarget( Parse() );
            } else {
                PrtMsg( ERR | UNABLE_TO_INCLUDE, p );
            }
        }
    }

    if( !Glob.nomakeinit ) {
        if( !Glob.compat_nmake ) {
            ok = InsFile( MAKEFINI_NAME, true );
            if( ok ) {
                setFirstTarget( Parse() );
            }
        }
    }

    Glob.preproc = false;           /* turn off preprocessor */
}


STATIC void print( void )
/***********************/
{
    PrintMacros();
    PrintSuffixes();
    PrintTargets();
}

STATIC void ignoreNoCommands( const TLIST *tlist )
{
    // set targets to be OK if there are no commands to update it
    for( ; tlist != NULL; tlist = tlist->next ) {
        tlist->target->allow_nocmd = true;
    }
}

STATIC bool doMusts( void )
/*************************/
{
    bool    ok;

    if( firstTargFound == NULL && mustTargs == NULL ) {
        PrtMsg( FTL | NO_TARGETS_SPECIFIED );
        ExitFatal();
        // never return
    }

    UpdateInit();
    if( Glob.compat_nmake || Glob.compat_unix ) {
        /* For MS/UNIX mode, targets with no commands may be symbolic.
         * We need to check this now, after input files have been processed
         * but before any commands have been executed.
         */
        CheckNoCmds();
    }

    if( mustTargs == NULL ) {
        ignoreNoCommands( firstTargFound );
        ok = MakeList( firstTargFound );
    } else {
        ignoreNoCommands( mustTargs );
        ok = MakeList( mustTargs );
    }

    UpdateFini();
    return( ok );
}


STATIC void globInit( void )
/**************************/
{
    Glob.swchar = (char)SwitchChar();
}


STATIC void init( char const * const *argv )
/******************************************/
{
    char        *makeopts;
    char const  *log_name;

    LogInit( NULL );
    globInit();
    MemInit();          /* memory handlers          */
    if( !MsgInit() ) {
        exit( EXIT_FAILURE );
        // never return
    }
    VecInit();          /* vector strings           */
    CacheInit();        /* directory cacheing       */
    MacroInit();        /* initialize macros        */
    TargetInit();       /* target must come before  */
    SuffixInit();       /* suffix                   */
    LexInit();
    ExecInit();
    AutoDepInit();

#if defined( __WATCOMC__ ) && defined( __NT__ )
    _fileinfo = 0;      /* C Library Kludge -------------------------- */
#endif
#ifdef __OS2__
    _grow_handles( 100 ); /* Some OS/2 versions allow only 20 files open by default */
#endif
    filesToDo = NULL;
    mustTargs = NULL;
    log_name = "";
    LogFini();
    LogInit( procLogName( argv ) );
    makeopts = procFlags( argv, &log_name );
    ParseInit();
    doBuiltIns( makeopts );
    FreeSafe( makeopts );
}


static int ExitSafe( int rc )
/***************************/
{
    static bool busy = false;   /* recursion protection */

    if( !busy ) {
        busy = true;
        if( rc == EXIT_ERROR || rc == EXIT_FATAL ) {
            PrtMsg( ERR | MAKE_ABORT );
        }
#ifndef NDEBUG
        while( filesToDo != NULL ) {
            NODE * const cur = filesToDo;
            filesToDo = cur->next;
            FreeSafe( cur );
        }
        if( mustTargs != NULL ) {
            FreeTList( mustTargs );
        }
        if( firstTargFound != NULL ) {
            FreeTList( firstTargFound );
        }
#endif

        AutoDepFini();
        ExecFini();
        LexFini();
        SuffixFini();       /* suffix must come before target   */
        TargetFini();
        MacroFini();
        CacheFini();
        VecFini();
#ifndef NDEBUG
        SetEnvFini();
        DLLFini();
#endif
        MsgFini();
        MemFini();
        LogFini();
    }

    return( rc );
}

void ExitFatal( void )
{
    exit( ExitSafe( EXIT_FATAL ) );
    // never return
}

void ExitError( void )
{
    exit( ExitSafe( EXIT_ERROR ) );
    // never return
}

void ExitOK( void )
{
    exit( ExitSafe( EXIT_OK ) );
    // never return
}

int main( int argc, char **argv )
/*******************************/
{
#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc;
#endif

    InitSignals();
    InitHardErr();
    init( (const char **)argv );        /* initialize, process cmdline */
    if( !Glob.noheader && !Glob.headerout ) {
        PrintBanner();
    }
    parseFiles();
    if( Glob.print ) {
        print();
        return( ExitSafe( EXIT_OK ) );
    }
    if( Glob.erroryet ) {
        return( ExitSafe( EXIT_ERROR ) );
    }
    if( !doMusts() ) {
        return( ExitSafe( EXIT_ERROR ) );
    }
    ParseFini();
    return( ExitSafe( EXIT_OK ) );
}
