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


#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include "rcmem.h"
#include "param.h"
#include "rcdll.h"
#include "global.h"
#include "rcio.h"
#include "tmpctl.h"
#include "errors.h"
#include "layer0.h"
#include "ldstr.h"
#include "scan.h"
#include "autodep.h"
#include "dbtable.h"
#include "loadstr.h"
#include "write.h"
#include "yydriver.h"
#include "wres.h"
#include "preproc.h"
#include "rclayer0.h"
#include "depend.h"
#include "errprt.h"
#include "yydriver.h"

static IDECBHdl         cbHandle;
static IDECallBacks    *cbList;
static IDEInitInfo     *initInfo;
jmp_buf                 DLL_JumpPt;
char                    ImageName[ _MAX_PATH ];

/* forward declaration */
static void flushPrintf( void );

unsigned IDEDLL_EXPORT IDEGetVersion( void ) {
/*********************************************/
    return( IDE_CUR_DLL_VER );
}

IDEBool IDEDLL_EXPORT IDEInitDLL( IDECBHdl hdl, IDECallBacks *cb, IDEDllHdl *info )
/*********************************************************************************/
{
    cbHandle = hdl;
    cbList = cb;
    *info = 0;
    initInfo = NULL;
    return( FALSE );
}

IDEBool IDEDLL_EXPORT IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info ) {
/**************************************************************************/
    initInfo = info;
    return( FALSE );
}

void IDEDLL_EXPORT IDEStopRunning( void )
/*****************************************/
{
    StopInvoked = TRUE;
}

IDEBool IDEDLL_EXPORT IDERunYourSelf( IDEDllHdl hdl, const char *opts, IDEBool *fatalerr )
/****************************************************************************************/
{
    int         argc;
    char      **argv;
    char       *cmdbuf;
    const char *str;
    int         ret;
    char        infile[ _MAX_PATH + 2 ];  // +2 for quotes
    char        outfile[ _MAX_PATH + 6 ]; // +6 for -fo="" or -fe=""
    IDEBool     pass1;
    unsigned    i;

    StopInvoked = FALSE;
    if( fatalerr != NULL ) {
        *fatalerr = FALSE;
    }
    RcMemInit();
    InitGlobs();
    str = opts;
    argc = ParseEnvVar( str, NULL, NULL );
    argv = RcMemMalloc( ( argc + 4 ) * sizeof( char * ) );
    cmdbuf = RcMemMalloc( strlen( str ) + argc + 1 );
    ParseEnvVar( str, argv, cmdbuf );
    pass1 = FALSE;
    for( i=0; i < argc; i++ ) {
        if( argv[i] != NULL && !stricmp( argv[i], "-r" ) ) {
            pass1 = TRUE;
            break;
        }
    }
    argv[0] = ImageName;
    if( initInfo == NULL || initInfo->ver == 1
        || !initInfo->cmd_line_has_files ) {
        infile[0] = '\"';
        cbList->GetInfo( cbHandle, IDE_GET_SOURCE_FILE, 0,
                         (unsigned long)(infile + 1) );
        strcat( infile, "\"" );
        argv[argc] = infile;
        if( pass1 ) {
            strcpy( outfile, "-fo=\"" );
        } else {
            strcpy( outfile, "-fe=\"" );
        }
        argc++;
        cbList->GetInfo( cbHandle, IDE_GET_TARGET_FILE, 0,
                         (unsigned long)outfile + 5 );
        strcat( outfile, "\"" );
        argv[argc] = outfile;
        argc++;
    }
    if( initInfo != NULL && initInfo->ignore_env ) {
        argv[ argc ] = "-x";
        argc ++;
    }
    argv[ argc ] = NULL;        // last element of the array must be NULL
    ret = setjmp( DLL_JumpPt );
    if( ret == 0 ) {
        ret = Dllmain( argc, argv );
    }

    flushPrintf();
    FreeGlobs();
    RcMemFree( argv );
    RcMemFree( cmdbuf );
    RcMemShutdown();

    return( ret );
}


void IDEDLL_EXPORT IDEFreeHeap( void )
/**************************************/
{
    _heapshrink();
}

void IDEDLL_EXPORT IDEFiniDLL( IDEDllHdl hdl )
/********************************************/
{
}

#define PRINTF_BUF_SIZE         2048
static char     formatBuffer[ PRINTF_BUF_SIZE ];
static char     *curBufPos;

static void flushPrintf( void ) {
/********************************/

    OutPutInfo          info;

    if( curBufPos != formatBuffer ) {
        InitOutPutInfo( &info );
        info.severity = SEV_WARNING;
        info.flags = 0;
        RcFprintf( NULL, &info, "\n" );
    }
}

void setPrintInfo( IDEMsgInfo *buf, OutPutInfo *src, char *msg ) {
/*****************************************************************/
    IDEMsgSeverity      sev;

    if( src == NULL ) {
        IdeMsgInit( buf, IDEMSGSEV_DEBUG, msg );
    } else {
        switch( src->severity ) {
        case SEV_BANNER:
            sev = IDEMSGSEV_BANNER;
            break;
        case SEV_DEBUG:
            sev = IDEMSGSEV_DEBUG;
            break;
        case SEV_WARNING:
            sev = IDEMSGSEV_WARNING;
            break;
        case SEV_ERROR:
        case SEV_FATAL_ERR:
            sev = IDEMSGSEV_ERROR;
            break;
        }
        IdeMsgInit( buf, sev, msg );
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

int RcFprintf( FILE *fp, OutPutInfo *info, const char *format, ... )
/*******************************************************************/
{
    int             err;
    va_list         args;
    char           *start;
    char           *end;
    unsigned        len;
    IDEMsgInfo      msginfo;

    va_start( args, format );
    err = _vbprintf( curBufPos, PRINTF_BUF_SIZE - ( curBufPos - formatBuffer),
                     format, args );
    va_end( args );
    start = formatBuffer;
    end = curBufPos;
    for( ;; ) {
        if( *end == '\n' ) {
            *end = '\0';
            setPrintInfo( &msginfo, info, start );
            cbList->PrintWithInfo( cbHandle, &msginfo );
            start = end + 1;
        } else if( *end == '\0' ) {
            len = strlen( start );
            memmove( formatBuffer, start, len + 1 );
            curBufPos = formatBuffer + len;
            break;
        }
        end++;
    }
    return( err );
}

static void InitGlobs( void )
/***************************/
{
    curBufPos = formatBuffer;
    memset( &CmdLineParms, 0, sizeof( struct RCParams ) );
    memset( &CurrResFile, 0, sizeof( RcResFileID ) );
    memset( &Pass2Info, 0, sizeof( RcPass2Info ) );
    NewIncludeDirs = NULL;
    ErrorHasOccured = false;
    memset( CharSet, 0, sizeof( CharSet ) );
    memset( &Instance, 0, sizeof( HANDLE_INFO ) );
    TmpCtlInitStatics();
    Layer0InitStatics();
    SemanticInitStatics();
    ErrorInitStatics();
    SharedIOInitStatics();
    ScanInitStatics();
    AutoDepInitStatics();
    DbtableInitStatics();
    LoadstrInitStatics();
    WriteInitStatics();
    PreprocVarInit();
    PPMacroVarInit();
    ParseInitStatics();
}

static void FreeGlobs( void )
/***************************/
{
    ScanParamShutdown();
    FiniRcMsgs();
}

extern char *RcGetEnv( const char *name )
/****************************************/
{
    char        *val;
    char        *ret;

    if( initInfo != NULL && initInfo->ignore_env ) {
        ret = NULL;
    } else if( !cbList->GetInfo( cbHandle, IDE_GET_ENV_VAR,
                (IDEGetInfoWParam)name, (IDEGetInfoLParam)&val ) ) {
        ret = val;
    } else {
        ret = NULL;
    }
    return( ret );
}
