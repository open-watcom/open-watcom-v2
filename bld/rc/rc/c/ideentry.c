/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <stdarg.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <malloc.h>            /* necessary for _heapshrink() */
    #include <process.h>
#endif
#include "global.h"
#include "errprt.h"
#include "idedll.h"
#include "rcmem.h"
#include "rcspawn.h"
#include "rcldstr.h"
#include "rcerrors.h"
#include "rc.h"
#include "rccore.h"
#include "pathgrp2.h"
#include "preproc.h"
#include "param.h"

#include "clibint.h"
#include "clibext.h"


#define PRINTF_BUF_SIZE         2048

#define IDEFN(x)        IdeCbs->x

extern void InitGlobs( void );
extern void FiniGlobs( void );
extern void RCmain( void );

jmp_buf     jmpbuf_RCFatalError;

static IDECBHdl         IdeHdl;
static IDECallBacks     *IdeCbs;
static IDEInitInfo      *initInfo;

static IDEMsgSeverity SeverityMap[] = {
//   IDE msg severity       WRC msg severity
    IDEMSGSEV_BANNER,       // SEV_BANNER
    IDEMSGSEV_DEBUG,        // SEV_DEBUG
    IDEMSGSEV_WARNING,      // SEV_WARNING
    IDEMSGSEV_ERROR,        // SEV_ERROR
    IDEMSGSEV_ERROR         // SEV_FATAL_ERR
};

static char     formatBuffer[PRINTF_BUF_SIZE];
static char     *curBufPos;

static void flushPrintf( void )
/*****************************/
{
    OutPutInfo          info;

    if( curBufPos != formatBuffer ) {
        InitOutPutInfo( &info );
        info.severity = SEV_WARNING;
        RcMsgFprintf( &info, "\n" );
    }
}

static void setPrintInfo( IDEMsgInfo *buf, OutPutInfo *src, char *msg )
/*********************************************************************/
{
    if( src == NULL ) {
        IdeMsgInit( buf, IDEMSGSEV_DEBUG, msg );
    } else {
        IdeMsgInit( buf, SeverityMap[src->severity], msg );
        if( src->flags & OUTFLAG_FILE ) {
            IdeMsgSetSrcFile( buf, src->file );
        }
        if( src->flags & OUTFLAG_ERRID ) {
            IdeMsgSetMsgNo( buf, src->errid ) ;
            IdeMsgSetHelp( buf, "wrcerrs.hlp", src->errid );
        }
        if( src->flags & OUTFLAG_LINE ) {
            IdeMsgSetSrcLine( buf, src->lineno );
        }
    }
}

void InitOutPutInfo( OutPutInfo *info )
/*************************************/
{
    info->flags = 0;
}

int RcMsgFprintf( OutPutInfo *info, const char *format, ... )
/***********************************************************/
{
    int             err;
    va_list         args;
    char            *start;
    char            *end;
    size_t          len;
    IDEMsgInfo      msginfo;

    va_start( args, format );
    err = vsnprintf( curBufPos, PRINTF_BUF_SIZE - ( curBufPos - formatBuffer ), format, args );
    va_end( args );
    start = formatBuffer;
    for( end = curBufPos; *end != '\0'; end++ ) {
        if( *end == '\n' ) {
            *end = '\0';
            setPrintInfo( &msginfo, info, start );
            IDEFN( PrintWithInfo )( IdeHdl, &msginfo );
            start = end + 1;
        }
    }
    len = strlen( start );
    memmove( formatBuffer, start, len + 1 );
    curBufPos = formatBuffer + len;
    return( err );
}

const char *RcGetEnv( const char *name )
/**************************************/
{
    const char  *val;

    if( IdeCbs != NULL
      && !initInfo->ignore_env ) {
        if( !IDEFN( GetInfo )( IdeHdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)name, (IDEGetInfoLParam)&val ) ) {
            return( val );
        }
    }
    return( NULL );
}

static int RCMainLine( const char *opts, int argc, char **argv )
/**************************************************************/
{
    char        infile[_MAX_PATH];
    char        outfile[_MAX_PATH];
    int         i;
    int         rc;
    OPT_STORAGE data;

    rc = 1;
    curBufPos = formatBuffer;
    RcMemInit();
    InitGlobs();
    if( InitRcMsgs() ) {
        rc = setjmp( jmpbuf_RCFatalError );
        if( rc == 0 ) {
            PP_Init( '#', PPSPEC_RC );
            OPT_INIT( &data );
            ScanParamInit();
            if( opts != NULL ) {
                ProcOptions( &data, opts );
            }
            for( i = 1; i < argc; i++ ) {
                ProcOptions( &data, argv[i] );
            }
            *infile = '\0';
            *outfile = '\0';
            if( initInfo != NULL
              && initInfo->ver > 1
              && !initInfo->cmd_line_has_files ) {
                IDEFN( GetInfo )( IdeHdl, IDE_GET_SOURCE_FILE, (IDEGetInfoWParam)NULL, (IDEGetInfoLParam)infile );
                IDEFN( GetInfo )( IdeHdl, IDE_GET_TARGET_FILE, (IDEGetInfoWParam)NULL, (IDEGetInfoLParam)outfile );
            }
            rc = SetOptions( &data, infile, outfile );
            if( rc == 0 ) {
                rc = RCSpawn( RCmain );
            }
            ScanParamFini();
            OPT_FINI( &data );
            PP_Fini();
        }
    }
    flushPrintf();
    FiniRcMsgs();
    FiniGlobs();
    RcMemShutdown();
    return( rc );
}

unsigned IDEAPI IDEGetVersion( void )
/***********************************/
{
    return( IDE_CUR_DLL_VER );
}

IDEBool IDEAPI IDEInitDLL( IDECBHdl cbhdl, IDECallBacks *cb, IDEDllHdl *hdl )
/***************************************************************************/
{
    IdeHdl = cbhdl;
    IdeCbs = cb;
    *hdl = 0;
    initInfo = NULL;
    /*
     * init wrc
     */
    IgnoreINCLUDE = false;
    IgnoreCWD = false;
    return( false );
}

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
/****************************************************************/
{
    /* unused parameters */ (void)hdl;

    if( info->ver < 2 ) {
        return( true );
    }
    if( info->ignore_env ) {
        IgnoreINCLUDE = true;
        IgnoreCWD = true;
    }
    if( info->cmd_line_has_files ) {
//        CompFlags.ide_cmd_line_has_files = true;
    }
    if( info->ver > 2 ) {
        if( info->console_output ) {
            CmdLineParms.ConsoleTTY = true;
        }
        if( info->ver > 3 ) {
            if( info->progress_messages ) {
//                CompFlags.progress_messages = true;
            }
        }
    }
    initInfo = info;
    return( false );
}

void IDEAPI IDEStopRunning( void )
/********************************/
{
    StopInvoked = true;
}

int IDEAPI IDERunYourSelf( IDEDllHdl hdl, const char *opts, IDEBool *fatalerr )
/*****************************************************************************/
{
    int         rc;

    /* unused parameters */ (void)hdl;

    StopInvoked = false;
    if( fatalerr != NULL )
        *fatalerr = false;
    rc = RCMainLine( opts, 0, NULL );
    if( rc == -1
      && fatalerr != NULL )
        *fatalerr = true;
    return( rc );
}

int IDEAPI IDERunYourSelfArgv( IDEDllHdl hdl, int argc, char **argv, IDEBool *fatalerr )
/**************************************************************************************/
{
    int         rc;

    /* unused parameters */ (void)hdl;

#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#endif

    StopInvoked = false;
    if( fatalerr != NULL )
        *fatalerr = false;
    rc = RCMainLine( NULL, argc, argv );
    if( rc == -1
      && fatalerr != NULL )
        *fatalerr = true;
    return( rc );
}

void IDEAPI IDEFreeHeap( void )
/*****************************/
{
#if defined( __WATCOMC__ )
    _heapshrink();
#endif
}

void IDEAPI IDEFiniDLL( IDEDllHdl hdl )
/*************************************/
{
    /* unused parameters */ (void)hdl;

    /*
     * fini wrc
     */
}
