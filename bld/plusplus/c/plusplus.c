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
* Description:  C++ compiler top level driver.
*
****************************************************************************/

#include "plusplus.h"
#include <signal.h>
#include <ctype.h>
#include <setjmp.h>
#include <limits.h>
#include "wio.h"
#include "walloca.h"
#include "scan.h"
#include "memmgr.h"
#include "stats.h"
#include "iosupp.h"
#include "preproc.h"
#include "fname.h"
#include "yydriver.h"
#include "cmdline.h"
#include "cgfront.h"
#include "context.h"
#include "srcfile.h"
#include "initdefs.h"
#include "cppdwarf.h"
#include "rtngen.h"
#include "cppexit.h"
#include "pragdefn.h"
#include "cgback.h"
#include "cusage.h"
#include "brinfo.h"
#include "autodep.h"
#include "swchar.h"
#include "ialias.h"
#include "ideentry.h"
#include "pathgrp2.h"
#ifndef NDEBUG
    #include "dbg.h"
    #include "pragdefn.h"
    #include "togglesd.h"
#endif

#include "clibint.h"
#include "clibext.h"


#define DEFAULT_PREINCLUDE_FILE     "_preincl.h"

enum {
    WPP_WARNINGS        = 0x01,                 /* only if -we is on */
    WPP_ERRORS          = 0x02,
    WPP_FATAL           = 0x04,
    WPP_BATCH_FILES     = 0x08,
    WPP_COMPILE_FAIL    = WPP_WARNINGS
                        | WPP_ERRORS
                        | WPP_FATAL
                        ,
    WPP_SUCCESS         = 0
};


static struct {                     // flags:
    unsigned dll_first_time :1;     // - first-time thru
    unsigned batch_cmds     :1;     // - processing batch cmd file
} flags =
{   true                            // - first-time thru
,   false
};


static void CloseFiles( void )
{
    if( CppFile != NULL ) {
        fflush( CppFile );
        if( ferror( CppFile ) ) {
            /* issue message */
        }
        IoSuppCloseFile( &CppFile );
    }
    IoSuppCloseFile( &DefFile );
}


static void resetHandlers( void )
{
    CloseFiles();
}


// open the primary source file, and return pointer to root file name
//
static void MakePgmName(        // MAKE CANONICAL FILE NAME
    char *argv )                // - input name
{
    pgroup2     pg;

    _splitpath2( argv, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    SrcFName = FNameAdd( pg.fname );
    if( ModuleName == NULL ) {
        ModuleName = strsave( SrcFName );
    }
}


static void OpenPgmFile(        // OPEN PROGRAM FILE
    void )
{
    if( IoSuppOpenSrc( WholeFName, FT_SRC ) ) {
        CompInfo.primary_srcfile = SrcFileCurrent();
        PpStartFile();
    } else {
        CErr2p( ERR_CANT_OPEN_FILE, WholeFName );
        CSuicide();
    }
}


bool OpenSrcFile(               // OPEN A SOURCE FILE
    const char * filename,      // - file name
    src_file_type typ )         // - source file type
                                // return: true ==> opened ok
{
    if( IoSuppOpenSrc( filename, typ ) ) {
        PpStartFile();
        return( true );
    }
    if( !CompFlags.ignore_fnf ) {
        CErr2p( ERR_CANT_OPEN_FILE, filename );
    }
    return( false );
}


static bool parseCmdLine(       // PARSE COMMAND LINE
    char **argv )               // command line vector
{
    char    *p;
    bool    display_only;

    if( argv[0] == NULL )
        argv[0] = "";
    p = argv[0];
    while( isspace( *p ) )
        ++p;
    if( *p == '?' || *p == '\0' ) {
        CUsage();
        display_only = true;
    } else {
        GenCOptions( argv );
        if( WholeFName != NULL ) {
            MakePgmName( WholeFName );
        }
        display_only = false;
    }
    return( display_only );
}

static int makeExitStatus( int exit_status )
{
    CompFlags.compile_failed = false;
    CompFlags.fatal_error = false;
    if( exit_status & WPP_FATAL ) {
        CompFlags.fatal_error = true;
    }
    if( ErrCount != 0 ) {
        exit_status |= WPP_ERRORS;
    }
    if( CompFlags.warnings_cause_bad_exit && WngCount != 0 ) {
        exit_status |= WPP_WARNINGS;
    }
    if( exit_status & WPP_COMPILE_FAIL ) {
        CompFlags.compile_failed = true;
    }
    return( exit_status );
}

static bool openForceIncludeFile( void )
{
    bool    ok;

    if( CompFlags.cpp_output ) {
        PrtChar( '\n' );
    }
    ok = OpenSrcFile( ForceInclude, FT_HEADER_FORCED );
    CMemFreePtr( &ForceInclude );
    return( ok );
}

static void setForceIncludeFromEnv( void )
{
    const char  *force;

    force = CppGetEnv( "FORCE" );
    if( force != NULL ) {
        ForceInclude = strsave( force );
    } else {
        ForceInclude = NULL;
    }
}

static bool openForcePreIncludeFile( void )
{
    bool    ok;

    if( CompFlags.cpp_output ) {
        PrtChar( '\n' );
    }
    CompFlags.ignore_fnf = true;
    ok = OpenSrcFile( ForcePreInclude, FT_HEADER_PRE );
    CompFlags.ignore_fnf = false;
    CMemFreePtr( &ForcePreInclude );
    return( ok );
}

static void setForcePreInclude( void )
{
    ForcePreInclude = strsave( DEFAULT_PREINCLUDE_FILE );
}

static int doCCompile(          // COMPILE C++ PROGRAM
    char **argv )               // - command line vector
{
    int exit_status;            // - status for exit return code
    bool gen_code;              // - generate code?
    jmp_buf env;                // - for suicide

    exit_status = WPP_SUCCESS;
    ExitPointAcquire( cpp_front_end );
    Environment = env;
    if( setjmp( env ) ) {   /* if fatal error has occurred */
        exit_status |= WPP_FATAL;
        IAliasFini();
        CtxSetCurrContext( CTX_FINI );
    } else {
        ScanInit();
        setForceIncludeFromEnv();
        setForcePreInclude();
        if( flags.batch_cmds ) {
            CompFlags.batch_file_processing = true;
            CompFlags.banner_printed = true;
        }
        if( parseCmdLine( argv ) ) {
            exit_status |= WPP_WARNINGS;
        } else if( CompFlags.batch_file_primary && !CompFlags.batch_file_processing ) {
            if( CompFlags.batch_file_eof ) {
                exit_status |= WPP_ERRORS;
            } else {
                exit_status |= WPP_BATCH_FILES;
            }
        } else if( WholeFName == NULL ) {
            CErr1( ERR_FILENAME_REQUIRED );
            CompFlags.cmdline_error = true;
            exit_status |= WPP_ERRORS;
        } else {
            ErrFileErase();
            if( !CompFlags.quiet_mode ) {
                if( CompFlags.batch_file_processing || CompInfo.compfile_max != 1 ) {
                    MsgDisplayLineArgs( "Compiling: ", WholeFName, NULL );
                }
            }
            if( ErrCount != 0 ) {
                CompFlags.cmdline_error = true;
            }
            PTypeCheckInit();       /* must come after command line parsing */
            if( CompFlags.cpp_output ) {
                PpOpen();           /* must be before OpenPgmFile() */
            } else {
                BrinfInit( true );  /* must be before OpenPgmFile() */
            }
            CompFlags.srcfile_compiled = true;
            ExitPointAcquire( cpp_preproc );
            if( CompFlags.cpp_output ) {
                CtxSetCurrContext( CTX_SOURCE );
                ExitPointAcquire( cpp_preproc_only );
                CompFlags.cpp_output = false;
                if( ForcePreInclude != NULL ) {
                    CtxSetCurrContext( CTX_PREINCL );
                    if( openForcePreIncludeFile() ) {
                        PpParse();
                        SrcFileClose( true );
                    }
                }
                CompFlags.cpp_output = true;
                if( ForceInclude != NULL ) {
                    CppEmitPoundLine( 1, WholeFName, EL_NULL );
                    CtxSetCurrContext( CTX_FORCED_INCS );
                    if( openForceIncludeFile() ) {
                        PpParse();
                        SrcFileClose( true );
                    }
                }
                OpenPgmFile();
                PpParse();
                ExitPointRelease( cpp_preproc_only );
            } else {
                if( ForcePreInclude != NULL ) {
                    CtxSetCurrContext( CTX_PREINCL );
                    if( openForcePreIncludeFile() ) {
                        NextToken();    /* process pre-include file, must not include any code or variable */
                        SrcFileClose( true );
                    }
                }
                OpenPgmFile();
                CtxSetCurrContext( CTX_SOURCE );
                ExitPointAcquire( cpp_object );
                ExitPointAcquire( cpp_analysis );
                CgFrontModInitInit();       // must be before pchdr read point
                if( CompFlags.use_pcheaders ) {
                    // getting the first token should involve opening
                    // the first #include if there are no definitions
                    // in the primary source file
                    CompFlags.watch_for_pcheader = true;
                }
                if( ForceInclude != NULL ) {
                    CtxSetCurrContext( CTX_FORCED_INCS );
                    if( openForceIncludeFile() ) {
                        DbgVerify( !CompFlags.watch_for_pcheader,
                            "force include file wasn't used for PCH" );
                    }
                }
                NextToken();
                CompFlags.watch_for_pcheader = false;
                CompFlags.external_defn_found = false;
                ParseDecls();
                CtxSetCurrContext( CTX_ENDFILE );
                ModuleInitFini();
                ScopeEndFileScope();
#ifndef NDEBUG
                if( TOGGLEDBG( dump_scopes ) ) {
                    DumpScopes();
                }
                if( TOGGLEDBG( dump_hash ) ) {
                    DumpHashStats();
                }
#endif
                PragmaExtrefsValidate();
                BrinfWrite();
                ExitPointRelease( cpp_analysis );
                gen_code = true;
                if( CompFlags.warnings_cause_bad_exit && WngCount != 0 ) {
                    gen_code = false;
                } else if( ErrCount != 0 ) {
                    gen_code = false;
                } else if( CompFlags.check_syntax ) {
                    gen_code = false;
                }
                if( gen_code ) {
                    DwarfBrowseEmit();
                    ScopeCreatePCHDebugSym(); // must be done before cg dwarf init
                    CgBackEnd();
                    if( CompFlags.generate_auto_depend ) {
                        AdOpen();
                        AdDump();
                        AdClose( false );
                    }
                } else {
                    AdClose( true );
                }
                CtxSetCurrContext( CTX_FINI );
                ExitPointRelease( cpp_object );
            }
            ExitPointRelease( cpp_preproc );
        }
    }
    exit_status = makeExitStatus( exit_status );
    CgFrontFini();
    IAliasFini();
    CloseFiles();
    ExitPointRelease( cpp_front_end );
    return( exit_status );
}

static void initCompFlags( void )
{
    struct {
        boolbit     ignore_environment      : 1;
        boolbit     ignore_current_dir      : 1;
        boolbit     ide_cmd_line_has_files  : 1;
        boolbit     ide_console_output      : 1;
    } xfer_flags;

    #define __save_flag( x ) xfer_flags.x = CompFlags.x;
    #define __restore_flag( x ) CompFlags.x = xfer_flags.x;

    __save_flag( ignore_environment );
    __save_flag( ignore_current_dir );
    __save_flag( ide_cmd_line_has_files );
    __save_flag( ide_console_output );
    memset( &CompFlags, 0, sizeof( CompFlags ) );
    __restore_flag( ignore_environment );
    __restore_flag( ignore_current_dir );
    __restore_flag( ide_cmd_line_has_files );
    __restore_flag( ide_console_output );
    CompFlags.dll_subsequent = true;
    CompFlags.banner_printed = true;

    #undef __save_flag
    #undef __restore_flag
}

static int front_end(           // FRONT-END PROCESSING
    char **argv )               // - command line vector
{
    int exit_status;
    jmp_buf exit_jmpbuf;        // - exit jmpbuf

    if( flags.dll_first_time ) {
        flags.dll_first_time = false;
    } else {
        initCompFlags();
    }
    if( 0 == setjmp( exit_jmpbuf ) ) {
        CppExitInit();
#if defined( __WATCOMC__ ) && defined( _M_IX86 )
        _real87 = 0;
        _8087 = 0;
#endif
        CompInfo.exit_jmpbuf = exit_jmpbuf;
        CtxSetCurrContext( CTX_INIT );
//printf( "ErrLimit = %u\n", ErrLimit );
        ExitPointAcquire( cpp );
        if( CompFlags.ide_console_output ) {
            IoSuppSetLineBuffering( stdout, 256 );
            IoSuppSetLineBuffering( stderr, 256 );
#if defined(__DOS__)
            if( !CompFlags.dll_subsequent ) {
                SrcFileFClose( stdaux );
                SrcFileFClose( stdprn );
            }
#endif
        }
        CppAtExit( &resetHandlers );
        SwitchChar = _dos_switch_char();
        PpInit();
        IAliasInit();
        exit_status = doCCompile( argv );
        CtxSetCurrContext( CTX_FINI );
        ExitPointRelease( cpp );
        CppExitFini();
    } else {
        exit_status = makeExitStatus( WPP_FATAL );
    }
#ifndef NDEBUG
  #if defined( __WATCOMC__ )
    CheckEnterDebugger();
  #endif
#endif
    return( exit_status );
}


static int compileFiles(        // COMPILE FILES
    char **argv )               // - command line vector
{
    int exit_status;            // - exit status for pgm
    int file_status;            // - exit status for compilation

    exit_status = 0;
    for( CompInfo.compfile_cur = 1; ; ++CompInfo.compfile_cur ) {
        CompInfo.compfile_max = false;
        file_status = front_end( argv );
        if( file_status > exit_status ) {
            if( exit_status == WPP_BATCH_FILES )
                break;
            exit_status = file_status;
        }
        if( CompFlags.fatal_error )
            break;
        if( CompFlags.cmdline_error )
            break;
        if( CompInfo.compfile_cur >= CompInfo.compfile_max ) {
            break;
        }
    }
    return( exit_status );
}


static int compilePrimaryCmd(   // COMPILE PRIMARY CMD LINE
    char **argv )               // - command line vector
{
    int exit_status;            // - exit status for pgm

    flags.batch_cmds = false;
    exit_status = compileFiles( argv );
    if( exit_status == WPP_BATCH_FILES ) {
        exit_status = WPP_SUCCESS;
        flags.batch_cmds = true;
        for( ; ; ) {
            int cmd_status = compileFiles( argv );
            if( cmd_status > exit_status ) {
                exit_status = cmd_status;
            }
            if( CompFlags.batch_file_eof )
                break;
            if( exit_status != WPP_SUCCESS ) {
                if( exit_status == WPP_FATAL || !CompFlags.batch_file_continue ) {
                    CmdLnBatchAbort();
                    break;
                }
            }
        }
    }
    return( exit_status );
}


#ifndef NDEBUG
#define ZAP_NUM 20
#define ZAP_SIZE 1024
#define ZAP_CHAR 0xA7
static void stackZap( void )        // ZAP 20K OF STACK TO 0xA7
{
    int i;
    char *stack;

    for( i = 0; i < ZAP_NUM; ++i ) {
        stack = alloca( ZAP_SIZE );
        if( NULL == stack )
            break;
        DbgZapMem( stack, ZAP_CHAR, ZAP_SIZE );
    }
}
#undef ZAP_CHAR
#undef ZAP_SIZE
#else
#define stackZap()
#endif

#if defined(__AXP__)
extern void __noalignfault( void );
#endif


static void exitPointStart(     // CALLED AT EACH exitPointAcquire
    EXIT_POINT* defn )          // - exit-point acquired
{
    CompFlags.dll_subsequent = defn->subsequent;
}


int WppCompile(                 // MAIN-LINE (DLL)
    DLL_DATA* dll_data,         // - data for DLL
    char *input,                // - input file name argv string
    char *output )              // - output file name argv string
{
    int exit_status;

    stackZap();
    InitFiniStartup( &exitPointStart );
    ExitPointAcquire( mem_management );
#ifndef NDEBUG
    DbgHeapInit();
#endif
    if( dll_data->cmd_line != NULL ) {
        char* vect[4];
        unsigned i = 1;
        CompInfo.dll_data = dll_data;
        vect[0] = dll_data->cmd_line;
        if( input[0] != '\0' ) {
            vect[i] = input;
            ++i;
        }
        if( output[0] != '\0' ) {
            vect[i] = output;
            ++i;
        }
        vect[i] = NULL;
        exit_status = compilePrimaryCmd( vect );
    } else {
        char **new_argv;

        if( input[0] == '\0' && output[0] == '\0' ) {
            new_argv = &(dll_data->argv[1]);
        } else {
            new_argv = alloca(( dll_data->argc + 2 ) * sizeof( char * ));
            if( new_argv != NULL ) {
                char **s = &(dll_data->argv[1]);
                char **d = new_argv;
                while( *s ) {
                    *d = *s;
                    ++d;
                    ++s;
                }
                if( input[0] != '\0' ) {
                    *d = input;
                    ++d;
                }
                if( output[0] != '\0' ) {
                    *d = output;
                    ++d;
                }
                *d = NULL;

            }
        }
        if( new_argv != NULL ) {
            exit_status = compilePrimaryCmd( new_argv );
        } else {
            // need a message to be output
            exit_status = WPP_FATAL;
        }
    }
#ifndef NDEBUG
    DbgHeapFini();
#endif
    ExitPointRelease( mem_management );
    return( exit_status );
}
