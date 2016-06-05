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
* Description:  wmake input analysis
*
****************************************************************************/


#include <string.h>
#if defined(__OS2__) || defined(__DOS__)
   #include <stdio.h>
#endif
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif

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
#include "clibext.h"


STATIC TLIST    *mustTargs;         /* targets we must update           */
STATIC TLIST    *firstTargFound;    /* first targets we ever found      */
STATIC NODE     *filesToDo;         /* pointers into argv to -f files   */

#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC void doBuiltIns( const char *makeopts )
/*********************************************
 * perform the builtin commands
 */
{
    TLIST   *tlist;
    char    buf[2048];
    char    *cpy;

    if( !Glob.overide ) {
        DoingBuiltIn = TRUE;
        FmtStr( buf, "%F", BuiltIns );
        cpy = MallocSafe( 2048 + strlen( makeopts ) );
        FmtStr( cpy, buf, makeopts );
        InsString( cpy, FALSE );
        tlist = Parse();
        FreeTList( tlist );
        strcpy(cpy, "MAKE=" );
        if( _cmdname( cpy + sizeof "MAKE=" - 1 ) == NULL ) {
            strcat( cpy, "wmake" );
        }
        InsString( cpy, FALSE );
        tlist = Parse();
        FreeTList( tlist );
        if( Glob.compat_nmake || Glob.compat_unix ) {
            // suffixes must be parsed before builtins
            const char  *suffices = MSSuffixList;
            const char  *builtins = MSBuiltIn;

            FmtStr( cpy, "%%MAKEFLAGS=$(%%MAKEFLAGS) %F", makeopts );
            InsString( cpy, FALSE );
            tlist = Parse();
            FreeTList( tlist );
            if( Glob.compat_posix ) {
                suffices = POSIXSuffixList;
                builtins = POSIXBuiltIn;
            } else if( Glob.compat_unix ) {
                suffices = UNIXSuffixList;
                builtins = UNIXBuiltIn;
            }
            FmtStr( cpy, "%F", suffices );
            InsString( cpy, FALSE );
            tlist = Parse();
            FreeTList( tlist );
            FmtStr( buf, "%F", builtins );
        } else {
            FmtStr( buf, "%F", SuffixList );
        }
        FmtStr( cpy, buf, makeopts );
        InsString( cpy, FALSE );
        tlist = Parse();
        FreeTList( tlist );
        FreeSafe( cpy );
        DoingBuiltIn = FALSE;
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

void Header( void )
/*****************/
{
    if( Glob.noheader ) {
        return;
    }
    if( !Glob.headerout ) {
        Glob.headerout = TRUE;  /* so we don't print more than once */
        PrtMsg( INF | BANNER );
    }
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

    InsString( q, FALSE );     /* put arg into stream */
    while( LexToken( LEX_PARSER ) != TOK_END ) {
        /* NOP - eat all the characters */
    }

    if( Glob.compat_nmake ) {
        /* Insert twice because in nmake declaring a macro in the command line */
        /* is equivalent to declaring one as is and one that is all upper case */
        /* Approximately so. we cater for foo meaning FOO but not FoO W.Briscoe 20031114 */
        /* This is no problem! In make, foo=bar only sets foo and FOO W.Briscoe 20041014 */
        while( --p >= q ) {
            *p = toupper( *p );
        }

        InsString( q, FALSE );     /* put arg into stream */
        while( LexToken( LEX_PARSER ) != TOK_END ) {
            /* NOP - eat the characters. Needs own eater. W.Briscoe 20041014 */
        }
    }
    FreeSafe( q );
}


STATIC void handleTarg( const char *buf )
/***************************************/
{
    assert( buf != NULL );

    /* if it is not a valid target name, Update() won't be able to make it */
    WildTList( &mustTargs, buf, FALSE, TRUE );
}


STATIC void checkCtrl( const char *p )
/************************************/
{
    // p != NULL is checked by caller
    while( *p ) {          // scan for control characters
        if( !isprint( *p ) ) {
            PrtMsg( FTL | CTRL_CHAR_IN_CMD, *p );
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
    char        select = '\0';  /* - or swchar (*argv)[0]       */
    char        option = '\0';  /* the option (*argv)[1]        */
    const char  *p;             /* working pointer to *argv     */
    NODE        *new;           /* for adding a new file        */
    int         options[256 + 1] = { 0 };

    if( (p = argv[1]) != NULL ) {
        if( strcmp( p, "?" ) == 0
        || ((p[0] == '-' || p[0] == Glob.swchar) && strcmp( p + 1, "?" ) == 0) ) {
            Usage();
        }
    }

    Glob.macreadonly = TRUE;

    while( (p = *++argv) != NULL ) {
        checkCtrl( p );
        select = p[0];
        option = (char)tolower( p[1] );
        if( select == '-' || select == Glob.swchar ) {
            if( option != NULLCHAR && p[2] == NULLCHAR ) {
                switch( option ) {
                case '?':   Usage();                break;
                case 'a':   Glob.all       = TRUE;  break;
                case 'b':   Glob.block     = TRUE;  break;
                case 'c':   Glob.nocheck   = TRUE;  break;
                case 'd':   Glob.debug     = TRUE;  break;
                case 'e':   Glob.erase     = TRUE;  break;
                case 'h':   Glob.noheader  = TRUE;  break;
                case 'i':   Glob.ignore    = TRUE;  break;
                case 'j':   Glob.rcs_make  = TRUE;  break;
                case 'k':   Glob.cont      = TRUE;  break;
                case 'm':   Glob.nomakeinit= TRUE;  break;
                case 'n':   Glob.noexec    = TRUE;  break;
                case 'o':   Glob.optimize  = TRUE;  break;
                case 'p':   Glob.print     = TRUE;  break;
                case 'q':   Glob.query     = TRUE;  break;
                case 'r':   Glob.overide   = TRUE;  break;
                case 's':   Glob.silent    = TRUE;  break;
                case 't':   Glob.touch     = TRUE;  break;
                case 'u':   Glob.compat_unix= TRUE;  break;
                case 'v':   Glob.verbose   = TRUE;  break;
                case 'w':   Glob.auto_depends = TRUE;break;
#ifdef CACHE_STATS
                case 'x':   Glob.cachestat = TRUE;  break;
#endif
                case 'y':   Glob.show_offenders = TRUE; break;
                case 'z':   Glob.hold      = TRUE;  break;
                    /* these options require a filename */
                case 'f':
                case 'l':
                    if( (p = *++argv) == NULL ) {
                        PrtMsg( ERR | INVALID_FILE_OPTION, select, option );
                        Usage();
                    }
                    checkCtrl( p );
                    if( option == 'f' ) {
                        if( (p[0] == '-') && (p[1] == NULLCHAR) ) {
                            // stdin
                        } else if( (p[0] == '-') || (p[0] == Glob.swchar) ) {
                            PrtMsg( ERR | INVALID_FILE_OPTION, select, option );
                            Usage();
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
                    break;
                }
                options[(option | 0x20) + 1] = TRUE;
                continue;
            }
            if( p[3] == NULLCHAR ) {
                if( option == 'm'  && tolower( p[2] ) == 's' ) {
                    Glob.compat_nmake = TRUE;
                    Glob.nocheck   = TRUE;
                    options[(option | 0x20) + 1] = TRUE;
                    continue;
                }
                if( option == 's'  && tolower( p[2] ) == 'n' ) {
                    Glob.silentno  = TRUE;
                    options[(option | 0x20) + 1] = TRUE;
                    continue;
                }
                if( option == 'u'  && tolower( p[2] ) == 'x' ) {
                    Glob.compat_unix = TRUE;
                    Glob.compat_posix = TRUE;
                    Glob.nomakeinit = TRUE;
                    Glob.nocheck    = TRUE;
                    options[(option | 0x20) + 1] = TRUE;
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
        PrtMsg( ERR | INCOMPATIBLE__OPTIONS, select, option );
        Usage();
    }

    Glob.macreadonly = FALSE;

    {
        // 120 allows for 30 options.
        size_t const    optsize = 120 + (*log_name ? strlen( *log_name ) + 1: 0) + 1;
        char * const    makeopts = MallocSafe( optsize );
        unsigned        opt_index;
        char            default_option[] = " -?";

        makeopts[0] = 0;
        opt_index = 'a' - 1;
        while( ++opt_index <= 'z' ) {
            if( options[opt_index + 1] ) {
                switch( opt_index ) {
                case 'f':
                case 'n':
                    break;
                case 'l':
                    strcat( makeopts, *makeopts ? " -l " : "-l " );
                    strcat( makeopts, *log_name );
                    break;
                case 'm':
                    if( Glob.nomakeinit ) {
                        strcat( makeopts, *makeopts ? " -m" : "-m" );
                    }
                    if( Glob.compat_nmake ) {
                        strcat( makeopts, *makeopts ? " -ms" : "-ms" );
                    }
                    break;
                case 's':
                    if( Glob.silentno ) {
                        strcat( makeopts, *makeopts ? " -sn" : "-sn" );
                    } else {
                        strcat( makeopts, *makeopts ? " -s" : "-s" );
                    }
                    break;
                default:
                    default_option[2] = (char)opt_index;
                    strcat( makeopts, default_option + (int)(*makeopts == '\0') );
                }
            }

        }
        return( makeopts );
    }
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
                (tolower( p[1] ) == 'l') && (p[2] == NULLCHAR) ) {
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
    RET_T       ret;

    Glob.preproc = TRUE;            /* turn on preprocessor */

                                    /* process makeinit */
    if( !Glob.nomakeinit ) {
        if( Glob.compat_nmake ) {
            ret = InsFile( TOOLSINI_NAME, TRUE );
        } else {
            ret = InsFile( MAKEINIT_NAME, TRUE );
        }
        if( ret == RET_SUCCESS ) {
            setFirstTarget( Parse() );
            if( firstTargFound != NULL ) {
                PrtMsg( WRN | MAKEINIT_HAS_TARGET );
            }
        }
    }

    if( filesToDo == NULL ) {
        ret = InsFile( MAKEFILE_NAME, FALSE );
        if( ret == RET_SUCCESS ) {
            setFirstTarget( Parse() );
#ifdef MAKEFILE_ALT
        } else if( (ret = InsFile( MAKEFILE_ALT, FALSE )) == RET_SUCCESS ) {
            setFirstTarget( Parse() );
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
                InsOpenFile( STDIN_FILENO );
                ret = RET_SUCCESS;
            } else {
                ret = InsFile( p, FALSE );
            }
            if( ret == RET_SUCCESS ) {
                setFirstTarget( Parse() );
            } else {
                PrtMsg( ERR | UNABLE_TO_INCLUDE, p );
            }
        }
    }

    if( !Glob.nomakeinit ) {
        if( !Glob.compat_nmake ) {
            ret = InsFile( MAKEFINI_NAME, TRUE );
            if( ret == RET_SUCCESS ) {
                setFirstTarget( Parse() );
            }
        }
    }

    Glob.preproc = FALSE;           /* turn off preprocessor */
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
        tlist->target->allow_nocmd = TRUE;
    }
}

STATIC RET_T doMusts( void )
/**************************/
{
    RET_T   ret;

    if( firstTargFound == NULL && mustTargs == NULL ) {
        PrtMsg( FTL | NO_TARGETS_SPECIFIED );
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
        ret = MakeList( firstTargFound );
    } else {
        ignoreNoCommands( mustTargs );
        ret = MakeList( mustTargs );
    }

    UpdateFini();
    return( ret );
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
    log_name = NULL;
    LogFini();
    LogInit( procLogName( argv ) );
    makeopts = procFlags( argv, &log_name );
    ParseInit();
    doBuiltIns( makeopts );
    FreeSafe( makeopts );
}


int ExitSafe( int rc )
/********************/
{
    static BOOLEAN  busy = FALSE;   /* recursion protection */

    if( !busy ) {
        busy = TRUE;
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
        PutEnvFini();
        DLLFini();
#endif
        MemFini();
        MsgFini();
        LogFini();
    }

    return( rc );
}

int main( int argc, char **argv )
/*********************************************/
{
    assert( argv[argc] == NULL );       /* part of ANSI standard */
#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif
    InitSignals();
    InitHardErr();
    init( (const char **)argv );        /* initialize, process cmdline */
    Header();
    parseFiles();
    if( Glob.print ) {
        print();
        return( ExitSafe( EXIT_OK ) );
    }
    if( Glob.erroryet ) {
        return( ExitSafe( EXIT_ERROR ) );
    }
    if( doMusts() != RET_SUCCESS ) {
        return( ExitSafe( EXIT_ERROR ) );
    }
    ParseFini();
    return( ExitSafe( EXIT_OK ) );
}
