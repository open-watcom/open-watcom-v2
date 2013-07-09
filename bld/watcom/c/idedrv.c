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
* Description:  Driver for DLLs pluggable into the IDE (and wmake).
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "wio.h"
#include "idedll.h"
#include "idedrv.h"
#include "walloca.h"
#include "errout.h"


#ifdef DLLS_IMPLEMENTED
#undef DLLS_IMPLEMENTED
#endif


#if defined( __OS2__ ) || defined( __NT__ ) || defined( __DOS__ ) && defined( CAUSEWAY )
    //
    // DLLs implemented only for:
    //      DOS (386 Causeway Extender)
    //      OS/2 (386, PowerPC)
    //      NT (386, Alpha AXP, PowerPC)

    // you can use link in the dll objects into the stub in other os's
    //(eg dos--if you consider that an os) by defining STATIC_LINKAGE
    // and IDE_PGM. note that IDE_PGM needs to be defined anywhere that
    // IDEAPI is used and/or idedll.h is included

    #if defined( __OS2__ ) && !defined( __OSI__ )
        #define DLLS_IMPLEMENTED

        // The following are defined in os2.h
        #undef  COMMENT
        #define ERR         OS2_ERR

        #define INCL_DOS
        #include "os2.h"
        #undef  ERR
        typedef HMODULE DLL_HANDLE;
    #elif defined( __NT__ )
        #define DLLS_IMPLEMENTED
        #include <windows.h>
        typedef HINSTANCE DLL_HANDLE;
    #elif defined( __DOS__ ) && defined( CAUSEWAY )
        #define DLLS_IMPLEMENTED
        #include "cwdll.h"
        #include "bool.h"
    #else
        #include "bool.h"
    #endif

    #if defined( __WATCOMC__ ) && defined( __386__ )
        #define IDETOOL_GETVER          "_IDEGetVersion@0"
        #define IDETOOL_INITDLL         "_IDEInitDLL@12"
        #define IDETOOL_RUNSELF         "_IDERunYourSelf@12"
        #define IDETOOL_RUNSELF_ARGV    "_IDERunYourSelfArgv@16"
        #define IDETOOL_FINIDLL         "_IDEFiniDLL@4"
        #define IDETOOL_STOPRUN         "_IDEStopRunning@0"
        #define IDETOOL_INITINFO        "_IDEPassInitInfo@8"
    #else
        #define IDETOOL_GETVER          "IDEGetVersion"
        #define IDETOOL_INITDLL         "IDEInitDLL"
        #define IDETOOL_RUNSELF         "IDERunYourSelf"
        #define IDETOOL_RUNSELF_ARGV    "IDERunYourSelfArgv"
        #define IDETOOL_FINIDLL         "IDEFiniDLL"
        #define IDETOOL_STOPRUN         "IDEStopRunning"
        #define IDETOOL_INITINFO        "IDEPassInitInfo"
    #endif

    #ifdef DLLS_IMPLEMENTED
        typedef unsigned IDEAPI (*GetVerFn)( void );
        typedef IDEBool IDEAPI (*InitDllFn)( IDECBHdl hdl
                                                  , IDECallBacks *cb
                                                  , IDEDllHdl *info );
        typedef IDEBool IDEAPI (*RunSelfFn)( IDEDllHdl hdl
                                                  , const char *opts
                                                  , IDEBool *fatalerr );
        typedef IDEBool IDEAPI (*RunSelfFnArgv)( IDEDllHdl hdl
                                                  , int
                                                  , char **argv
                                                  , IDEBool *fatalerr );
        typedef void IDEAPI (*FiniDllFn)( IDEDllHdl hdl );
        typedef void IDEAPI (*StopRunFn)( void );
        typedef IDEBool IDEAPI (*PassInitInfo)( IDEDllHdl hdl, IDEInitInfo *info );
    #endif

#else
    #include "bool.h"
#endif

#if (defined DLLS_IMPLEMENTED || (defined STATIC_LINKAGE && defined IDE_PGM) )

typedef void (*P_FUN)( void );

#ifndef STATIC_LINKAGE

#ifdef __OS2__
    //
    // OS/2 Interface
    //

static int sysdepDLLLoad( IDEDRV *inf )
{
#define SIZE 32
    unsigned char   badfile[SIZE];

    return (int)DosLoadModule( (PSZ)badfile
                             , sizeof( badfile )
                             , (PSZ)inf->dll_name
                             , (DLL_HANDLE *)&inf->dll_handle );
}

static int sysdepDLLUnload( IDEDRV *inf )
{
    DosFreeModule( (DLL_HANDLE)inf->dll_handle );
    return( 0 );    // sometimes get failure in good situations
}

static int sysdepDLLgetProc( IDEDRV *inf
                           , char const *fun_name
                           , P_FUN *fun )
{
    int retcode;

    retcode = (int)DosQueryProcAddr( (DLL_HANDLE)inf->dll_handle
                                   , 0
                                   , (PSZ)fun_name
                                   , (PFN *)fun );
    if( 0 != retcode ) {
        // DLL could be linked case-insensitive
        unsigned    size = strlen( fun_name ) + 1;
        char        *p = alloca( size );

        p = memcpy( p, fun_name, size );
        p = strupr( p );
        retcode = (int)DosQueryProcAddr( (DLL_HANDLE)inf->dll_handle
                                       , 0
                                       , (PSZ)p
                                       , (PFN *)fun );
    }
    return( retcode );
}

#endif // __OS2__

#ifdef __NT__
    //
    // NT Interface
    //

static int sysdepDLLLoad( IDEDRV *inf )
{
    inf->dll_handle = (void *)LoadLibrary( inf->dll_name );
    return( 0 == inf->dll_handle );
}

static int sysdepDLLUnload( IDEDRV *inf )
{
    return( !(int)FreeLibrary( (DLL_HANDLE)inf->dll_handle ) );
}

static int sysdepDLLgetProc( IDEDRV *inf,
                             char const *fun_name,
                             P_FUN *fun )
{
    P_FUN fp = (P_FUN)GetProcAddress( (DLL_HANDLE)inf->dll_handle, fun_name );

    *fun = fp;
    return( 0 == fp );
}

#endif // __NT__

#if defined( __DOS__ ) && defined( CAUSEWAY )
    //
    // DOS Causeway Extender Interface
    //

static int sysdepDLLLoad( IDEDRV *inf )
{
    inf->dll_handle = LoadModule( (void *)inf->dll_name );
    return( 0 == inf->dll_handle );
}

static int sysdepDLLUnload( IDEDRV *inf )
{
    FreeModule( inf->dll_handle );
    return( 0 );
}

static int sysdepDLLgetProc( IDEDRV *inf
                           , char const *fun_name
                           , P_FUN *fun )
{
    *fun = GetProcAddress( inf->dll_handle, (void *)fun_name );
    if( 0 == *fun ) {
        // DLL could be linked case-insensitive
        unsigned    size = strlen( fun_name ) + 1;
        char        *p = alloca( size );

        p = memcpy( p, fun_name, size );
        p = strupr( p );
        *fun = GetProcAddress( inf->dll_handle, (void *)p );
    }
    return( 0 == *fun );
}

#endif // __DOS__

#endif // STATIC_LINKAGE



typedef int (*USER_DLL_FUN)( char const * );
typedef int (*USER_DLL_FUN_ARGV)( int, char ** );

#ifndef CHAIN_CALLBACK

static IDEBool IDEAPI stubPrintMsgFn( IDECBHdl hdl, char const *msg )
{
    hdl = hdl;
#ifndef NDEBUG
    fputs( "stubPrintMsgFn called!\n", errout );
    fputs( msg, errout );
    fputc( '\n', errout );
#else
    msg = msg;
#endif
    return( FALSE );
}

#ifndef NDEBUG
static void IDEAPI printProgressIndex( IDECBHdl hdl, unsigned index )
{
    hdl = hdl;
    fprintf( errout, "progress: %u\n", index );
}
#else
#define printProgressIndex      NULL
#endif

static IDEBool IDEAPI printMessage( IDECBHdl hdl, char const *msg )
{
    hdl = hdl;
    fputs( msg, errout );
    fputc( '\n', errout );
    return( FALSE );
}

static IDEBool IDEAPI printWithInfo( IDECBHdl hdl, IDEMsgInfo *inf )
{
    FILE    *fp;
    char    prt_buffer[ 512 ];

    hdl = hdl;
    IdeMsgFormat( hdl
                , inf
                , prt_buffer
                , sizeof( prt_buffer )
                , &printWithInfo );
    switch( inf->severity ) {
    case IDEMSGSEV_BANNER:
    case IDEMSGSEV_DEBUG:
    case IDEMSGSEV_NOTE_MSG:
        fp = stdout;
        break;
    case IDEMSGSEV_WARNING:
    case IDEMSGSEV_ERROR:
    case IDEMSGSEV_NOTE:
    default:
        fp = errout;
    }
    fputs( prt_buffer, fp );
    fputc( '\n', fp );
    fflush( fp );
    return( FALSE );
}

static IDEBool IDEAPI printWithCrLf( IDECBHdl hdl, const char *message )
{
    hdl = hdl;
    fputs( message, errout );
    fflush( errout );
    return( FALSE );
}

static IDEBool IDEAPI getInfoCB( IDECBHdl hdl, IDEInfoType type, IDEGetInfoWParam extra, IDEGetInfoLParam lparam )
{
    IDEBool retn;

    extra = extra;
    hdl = hdl;
    switch( type ) {
    default:
        retn = TRUE;
        break;
    case IDE_GET_ENV_VAR:
        {
            char const  *env_var;
            char const  *env_val;
            char const  **p_env_val;

            env_var = (char const *)extra;
            env_val = getenv( env_var );
            p_env_val = (char const **)lparam;
            *p_env_val = env_val;
            retn = ( env_val == NULL );
        }
        break;
    }
    return( retn );
}


static IDECallBacks callbacks = {     // CALL-BACK STRUCTURE
    // building functions
    NULL,                       // RunBatch
    printMessage,               // PrintMessage
    printWithCrLf,              // PrintWithCRLF
    printWithInfo,              // PrintWithInfo

    // Query functions
    getInfoCB,                  // GetInfo

    stubPrintMsgFn,             // ProgressMessage
    NULL,                       // RunDll
    NULL,                       // RunBatchCWD
    NULL,                       // OpenJavaSource
    NULL,                       // OpenClassFile
    NULL,                       // PackageExists
    NULL,                       // GetSize
    NULL,                       // GetTimeStamp
    NULL,                       // IsReadOnly
    NULL,                       // ReadData
    NULL,                       // Close
    NULL,                       // ReceiveOutput

    printProgressIndex,         // ProgressIndex
};
static IDECallBacks *CBPtr = &callbacks;
#else
static IDECallBacks *CBPtr = NULL;
#endif

static IDEInitInfo info =           // INFORMATION STRUCTURE
{   IDE_CUR_INFO_VER                // - ver
,   0                               // - ignore_env
,   1                               // - cmd_line_has_files
,   0                               // - console_output
,   0                               // - progress messages
,   0                               // - progress index
};

static IDEInitInfo  *InfoPtr = NULL;

#ifndef STATIC_LINKAGE
static IDEDRV   *Inf;
#endif

#ifdef __OSI__
#define NO_CTRL_HANDLERS
#endif

#ifndef NO_CTRL_HANDLERS
static void StopRunning( void )
{
// Provide static and dynamic linking
#ifdef STATIC_LINKAGE
    IDEStopRunning();
#else
    StopRunFn   idestopdll;

    if( 0 == sysdepDLLgetProc( Inf, IDETOOL_STOPRUN, (P_FUN*)&idestopdll ) ) {
       idestopdll();
    }
#endif
}
#endif // NO_CTRL_HANDLERS

#ifndef NO_CTRL_HANDLERS
static void intHandler( int sig_num )
{
    sig_num = sig_num;
    StopRunning();
}
#endif // NO_CTRL_HANDLERS

static void initInterrupt( void )
{
#ifndef NO_CTRL_HANDLERS
    signal( SIGINT, intHandler );
#ifndef __UNIX__
    signal( SIGBREAK, intHandler );
#endif // __UNIX__
#endif // NO_CTRL_HANDLERS
}

static void finiInterrupt( void )
{
#ifndef NO_CTRL_HANDLERS
    signal( SIGINT, SIG_DFL );
#ifndef __UNIX__
    signal( SIGBREAK, SIG_DFL );
#endif // __UNIX__
#endif // NO_CTRL_HANDLERS
}

#ifndef NDEBUG
#define _SET_PROGRESS \
    if( getenv( "__idedrv_progress_messages" ) ) { info.progress_messages = 1; } \
    if( getenv( "__idedrv_progress_index" ) ) { info.progress_index = 1; }
#else
#define _SET_PROGRESS
#endif

#ifdef STATIC_LINKAGE
static int ensureLoaded( IDEDRV *inf, int *p_runcode )
{
    int runcode = 0;
    int retcode = IDEDRV_SUCCESS;

    if( !inf->loaded ) {
        if( NULL == inf->ent_name ) {
            runcode = IDEInitDLL( NULL, CBPtr, &inf->ide_handle );
            if( 0 == runcode ) {
                if( NULL == InfoPtr ) {
                    InfoPtr = &info;
                    info.console_output = ( isatty( fileno( stdout ) ) != 0 );
                }
                _SET_PROGRESS;
                runcode = IDEPassInitInfo( inf->ide_handle, InfoPtr );
                if( 0 != runcode ) {
                    retcode = IDEDRV_ERR_INFO_EXEC;
                }
            } else {
                retcode = IDEDRV_ERR_INIT_EXEC;
            }
        }
        if( IDEDRV_SUCCESS == retcode ) {
            inf->loaded = TRUE;
        }
    }
    *p_runcode = runcode;
    return( retcode );
}
#else
static int ensureLoaded( IDEDRV *inf, int *p_runcode )
{
    int runcode = 0;
    int retcode = IDEDRV_SUCCESS;

    if( ! inf->loaded ) {
        runcode = sysdepDLLLoad( inf );
        if( 0 == runcode ) {
            if( NULL == inf->ent_name ) {
                InitDllFn initdll;
                runcode = sysdepDLLgetProc( inf, IDETOOL_INITDLL, 
                                                          (P_FUN *)&initdll );
                if( 0 == runcode ) {
                    runcode = initdll( NULL , CBPtr, &inf->ide_handle );
                    if( 0 == runcode ) {
                        PassInitInfo initinfo;
                        runcode = sysdepDLLgetProc( inf, IDETOOL_INITINFO, 
                                                          (P_FUN *)&initinfo );
                        if( 0 == runcode ) {
                            if( NULL == InfoPtr ) {
                                InfoPtr = &info;
                                info.console_output = ( isatty( fileno(stdout) ) != 0 );
                            }
                            _SET_PROGRESS;
                            runcode = initinfo( inf->ide_handle, InfoPtr );
                            if( 0 != runcode ) {
                                retcode = IDEDRV_ERR_INFO_EXEC;
                            }
                        } else {
                            retcode = IDEDRV_ERR_INFO;
                        }
                    } else {
                        retcode = IDEDRV_ERR_INIT_EXEC;
                    }
                } else {
                    retcode = IDEDRV_ERR_INIT;
                }
                if( IDEDRV_SUCCESS != retcode ) {
                    sysdepDLLUnload( inf );
                }
            }
        } else {
            retcode = IDEDRV_ERR_LOAD;
        }
        if( IDEDRV_SUCCESS == retcode ) {
            inf->loaded = TRUE;
        }
    }
    *p_runcode = runcode;
    return( retcode );
}
#endif

static int retcodeFromFatal( IDEBool fatal, int runcode, int retcode )
{
    if( fatal ) {
        retcode = IDEDRV_ERR_RUN_FATAL;
    } else if( 0 != runcode ) {
        retcode = IDEDRV_ERR_RUN_EXEC;
    }
    return( retcode );
}

static void initConsole( void )
{
    if( info.console_output ) {
        initInterrupt();
    }
}

static void finiConsole( void )
{
    if( info.console_output ) {
        finiInterrupt();
    }
}

#define stashCodes( _inf, _run, _ret ) \
    (_inf)->dll_status = (_run); (_inf)->drv_status = (_ret);

int IdeDrvExecDLL               // EXECUTE THE DLL ONE TIME (LOAD IF REQ'D)
    ( IDEDRV *inf               // - driver control information
    , char const *cmd_line )    // - command line
#ifdef STATIC_LINKAGE
// Execute DLL
//
// One mode (with static linkage):
//
//  (1) WATCOM IDE interface is used.
//
{
    int runcode;
    int retcode;

    retcode = ensureLoaded( inf, &runcode );
    if( retcode == IDEDRV_SUCCESS ) {
        IDEBool fatal = FALSE;
        initConsole();
        runcode = IDERunYourSelf( inf->ide_handle, cmd_line, &fatal );
        finiConsole();
        retcode = retcodeFromFatal( fatal, runcode, retcode );
    }
    stashCodes( inf, runcode, retcode );
    return( retcode );
}
#else
// Execute DLL
//
// Two modes (both with dynamic linkage):
//
//  (1) when second parameter is NULL, WATCOM IDE interface is used.
//
//  (2) otherwise, the dll is called at the entry name
//
{
    int runcode;
    int retcode;

    Inf = inf;
    retcode = ensureLoaded( inf, &runcode );
    if( retcode == IDEDRV_SUCCESS ) {
        if( NULL == inf->ent_name ) {
            RunSelfFn fun;
            runcode = sysdepDLLgetProc( inf, IDETOOL_RUNSELF, (P_FUN *)&fun );
            if( 0 == runcode ) {
                IDEBool fatal = FALSE;
                initConsole();
                runcode = fun( inf->ide_handle, cmd_line, &fatal );
                finiConsole();
                retcode = retcodeFromFatal( fatal, runcode, retcode );
                if( retcode == IDEDRV_ERR_RUN_FATAL ) {
                    sysdepDLLUnload( inf );
                    inf->loaded = FALSE;
                }
            } else {
                retcode = IDEDRV_ERR_RUN;
            }
        } else {
            USER_DLL_FUN fun;
            runcode = sysdepDLLgetProc( inf, inf->ent_name, (P_FUN *)&fun );
            if( 0 == runcode ) {
                initConsole();
                runcode = fun( cmd_line );
                finiConsole();
                retcode = retcodeFromFatal( FALSE, runcode, retcode );
            } else {
                retcode = IDEDRV_ERR_RUN;
            }
        }
    }
    stashCodes( inf, runcode, retcode );
    return( retcode );
}
#endif

#ifdef __UNIX__

int IdeDrvExecDLLArgv           // EXECUTE THE DLL ONE TIME (LOAD IF REQ'D)
    ( IDEDRV *inf               // - driver control information
    , int argc                  // - # of arguments
    , char **argv )             // - argument vector
#ifdef STATIC_LINKAGE
// Execute DLL
//
// One mode (with static linkage):
//
//  (1) WATCOM IDE interface is used.
//
{
    int runcode;
    int retcode;

    retcode = ensureLoaded( inf, &runcode );
    if( retcode == IDEDRV_SUCCESS ) {
        IDEBool fatal = FALSE;
        initConsole();
        runcode = IDERunYourSelfArgv( inf->ide_handle, argc, argv, &fatal );
        finiConsole();
        retcode = retcodeFromFatal( fatal, runcode, retcode );
    }
    stashCodes( inf, runcode, retcode );
    return( retcode );
}
#else
// Execute DLL
//
// Two modes (both with dynamic linkage):
//
//  (1) when second parameter is NULL, WATCOM IDE interface is used.
//
//  (2) otherwise, the dll is called at the entry name
//
{
    int runcode;
    int retcode;

    Inf = inf;
    retcode = ensureLoaded( inf, &runcode );
    if( retcode == IDEDRV_SUCCESS ) {
        if( NULL == inf->ent_name ) {
            RunSelfFnArgv fun;
            runcode = sysdepDLLgetProc( inf, IDETOOL_RUNSELF_ARGV, (P_FUN *)&fun );
            if( 0 == runcode ) {
                IDEBool fatal = FALSE;
                initConsole();
                runcode = fun( inf->ide_handle, argc, argv, &fatal );
                finiConsole();
                retcode = retcodeFromFatal( fatal, runcode, retcode );
                if( retcode == IDEDRV_ERR_RUN_FATAL ) {
                    sysdepDLLUnload( inf );
                    inf->loaded = FALSE;
                }
            } else {
                retcode = IDEDRV_ERR_RUN;
            }
        } else {
            USER_DLL_FUN_ARGV fun;
            runcode = sysdepDLLgetProc( inf, inf->ent_name, (P_FUN *)&fun );
            if( 0 == runcode ) {
                initConsole();
                runcode = fun( argc, argv );
                finiConsole();
                retcode = retcodeFromFatal( FALSE, runcode, retcode );
            } else {
                retcode = IDEDRV_ERR_RUN;
            }
        }
    }
    stashCodes( inf, runcode, retcode );
    return( retcode );
}
#endif

#endif


int IdeDrvUnloadDLL             // UNLOAD THE DLL
    ( IDEDRV *inf )             // - driver control information
#ifdef STATIC_LINKAGE
// Static Linkage: nothing to unload
{
    if( inf->loaded ) {
        inf->loaded = FALSE;
        IDEFiniDLL( inf->ide_handle );
    }
    return( IDEDRV_SUCCESS );
}
#else
// Dynamic Linkage: unload the DLL
{
    int         retcode;                // - return code
    FiniDllFn   fini;

    if( inf->loaded ) {
        if( 0 == sysdepDLLgetProc( inf, IDETOOL_FINIDLL, (P_FUN*)&fini ) ) {
            fini( inf->ide_handle );
        }
        inf->loaded = FALSE;
        if( 0 == sysdepDLLUnload( inf ) ) {
            retcode = IDEDRV_SUCCESS;
        } else {
            retcode = IDEDRV_ERR_UNLOAD;
        }
    } else {
        retcode = IDEDRV_SUCCESS;
    }
    return( retcode );
}
#endif


int IdeDrvStopRunning           // SIGNAL A BREAK
    ( IDEDRV *inf )             // - driver control information
#ifdef STATIC_LINKAGE
// Static Linkage: direct call
{
    if( inf->loaded ) {
        inf->loaded = FALSE;
        IDEStopRunning();
    }
    return( IDEDRV_SUCCESS );
}
#else
// Dynamic Linkage: indirect call
{
    StopRunFn   idestopdll;

    if( inf->loaded ) {
        if( 0 != sysdepDLLgetProc( inf, IDETOOL_STOPRUN, (P_FUN*)&idestopdll ) ) {
            return( IDEDRV_ERR_STOP );
        }
        idestopdll();
    }
    return( IDEDRV_SUCCESS );
}
#endif


static char const *msgs[] =
{
#define _IDEDRV(e,m) m
__IDEDRV
#undef _IDEDRV
};


int IdeDrvPrintError            // UNLOAD THE DLL
    ( IDEDRV *inf )             // - driver control information
{
    char const  *msg;
    int         retcode = inf->drv_status;

    if( retcode != IDEDRV_SUCCESS ) {
        if( retcode <= 0 || retcode >= IDEDRV_ERR_MAXIMUM ) {
            msg = "impossible error";
        } else {
            msg = msgs[ retcode ];
        }
        fprintf( errout, "ERROR with dll: %s\n    %s", inf->dll_name, msg );
        if( inf->dll_status != 0 ) {
            fprintf( errout, "    return code: %ld", inf->dll_status );
        }
        fputc( '\n', errout );
        fflush( errout );
    }
    return( inf->drv_status );
}

void IdeDrvChainCallbacks       // SET CALLBACKS FOR DLL CALLLING A DLL
    ( void *cb                  // - parent dll callbacks
    , void *info )              // - parent dll initialization
{
    CBPtr   = (IDECallBacks *)cb;
    InfoPtr = (IDEInitInfo *)info;
}

void *IdeDrvGetCallbacks                // GET CALLBACKS
    ( void )
{
    return( (void *)CBPtr );
}

void IdeDrvSetCallbacks         // GET CALLBACKS
    ( void *cb )
{
    CBPtr = (IDECallBacks *)cb;
}


void IdeDrvInit                 // INITIALIZE IDEDRV INFORMATION
    ( IDEDRV *inf               // - information
    , char const *dll_name      // - dll name
    , char const *ent_name )    // - entry name
{
    inf->dll_name   = dll_name;
    inf->ent_name   = ent_name;
    inf->ide_handle = 0;
    inf->dll_handle = 0;
    inf->drv_status = 0;
    inf->dll_status = 0;
    inf->loaded     = 0;
}


#else
    #error IDE not implemented for target platform
#endif // DLLS_IMPLEMENTED
