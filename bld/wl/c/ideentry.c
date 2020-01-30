/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  IDE interface entry points for wlink.
*
****************************************************************************/

#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ )
    #include <malloc.h>     /* necessary for _heapshrink() */
#endif
#include "linkstd.h"
#include "msg.h"
#include "fileio.h"
#include "idedll.h"
#include "idedrv.h"
#include "ideentry.h"
#include "wlink.h"
#include "wlnkmsg.h"
#include "cmdline.h"

#include "clibext.h"

#define PREFIX_SIZE 8

#define IDEFN(x)        IdeCbs->x

typedef struct {
    IDEInfoType type;
    char        prefix[PREFIX_SIZE + 1];
    bool        retry;
} extra_cmd_info;

static IDECBHdl         IdeHdl;
static IDEInitInfo      InitInfo;
static IDECallBacks     *IdeCbs;

static extra_cmd_info ExtraCmds[] = {
    IDE_GET_TARGET_FILE,"name    ",     false,
    IDE_GET_OBJ_FILE,   "file    ",     true,
    IDE_GET_LIB_FILE,   "lib     ",     true,
    IDE_GET_RES_FILE,   "opt res=",     false,
    0,                  "\0",           false
};

static IDEMsgSeverity SeverityMap[] = {
//   IDE msg severity       Wlink msg class
    IDEMSGSEV_NOTE_MSG,     // INF
    IDEMSGSEV_NOTE_MSG,     // YELL
    IDEMSGSEV_WARNING,      // WRN
    IDEMSGSEV_ERROR,        // MILD_ERR
    IDEMSGSEV_ERROR,        // ERR
    IDEMSGSEV_ERROR,        // FTL
    IDEMSGSEV_BANNER        // BANNER
};

#if defined( DLLS_IMPLEMENTED )
bool ExecDLLPgm( const char *pname, const char *cmdline )
/*******************************************************/
// return true if an error
{
    IDEDRV              inf;
    IDEDRV_STATUS       status;

    status = IDEDRV_ERR_LOAD;
    IdeDrvInit( &inf, pname, NULL );
    IdeDrvChainCallbacks( IdeCbs, &InitInfo );
    status = IdeDrvExecDLL( &inf, cmdline );
    IdeDrvUnloadDLL( &inf );
    return( status != IDEDRV_SUCCESS );
}
#endif

/* routines which are called by the linker core */

void WriteStdOut( const char *str )
/*********************************/
{
    CheckBreak();
    if( IdeCbs != NULL ) {
        IDEFN( PrintWithCRLF )( IdeHdl, str );
    }
}

void WriteStdOutWithNL( const char *str )
/***************************************/
{
    CheckBreak();
    if( IdeCbs != NULL ) {
        IDEFN( PrintMessage )( IdeHdl, str );
    }
}

void WriteStdOutNL( void )
/************************/
{
    if( IdeCbs != NULL ) {
        IDEFN( PrintMessage )( IdeHdl, "" );
    }
}

void WriteStdOutInfo( const char *str, unsigned level, const char *symbol )
/*************************************************************************/
{
    IDEMsgInfo  info;
    unsigned    msgclass;

    CheckBreak();
    if( IdeCbs != NULL ) {
        IdeMsgInit( &info, SeverityMap[(level & CLASS_MSK) >> NUM_SHIFT], str );
        msgclass = level & CLASS_MSK;
        if( msgclass != BANNER && msgclass >= (WRN & CLASS_MSK) ) {
            IdeMsgSetMsgNo( &info, CalcMsgNum( level ) );
            IdeMsgSetHelp( &info, "wlnkerrs.hlp", level & NUM_MSK );
        }
        if( symbol != NULL ) {
            IdeMsgSetLnkSymbol( &info, symbol );
        }
        IDEFN( PrintWithInfo )( IdeHdl, &info );
    }
}

char * GetEnvString( const char *envname )
/****************************************/
{
    char *retval;

    if( IdeCbs == NULL || InitInfo.ignore_env )
        return( NULL );
    IDEFN( GetInfo )( IdeHdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)envname, (IDEGetInfoLParam)&retval );
    return( retval );
}

bool IsStdOutConsole( void )
/*********************************/
{
    return( InitInfo.console_output );
}

void GetExtraCommands( void )
/***************************/
{
    extra_cmd_info const    *cmd;
    char                    buff[_MAX_PATH + PREFIX_SIZE];
    char                    *p;

    if( !InitInfo.cmd_line_has_files ) {
        for( cmd = ExtraCmds; cmd->prefix[0] != '\0'; ++cmd ) {
            for( ;; ) {
                memcpy( buff, cmd->prefix, PREFIX_SIZE );
                p = buff + PREFIX_SIZE;
                if( IDEFN( GetInfo )( IdeHdl, cmd->type, (IDEGetInfoWParam)NULL, (IDEGetInfoLParam)&p ) )
                    break;
                if( DoBuffCmdParse( buff ) )
                    break;
                if( !cmd->retry ) {
                    break;
                }
            }
        }
    }
}

/* routines which are called by the DLL driver */

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
/****************************************************************/
{
    /* unused parameters */ (void)hdl;

    if( info->ver < IDE_CUR_INFO_VER5 )
        return( true );
    InitInfo = *info;
    return( false );
}

unsigned IDEAPI IDEGetVersion( void )
/***********************************/
{
    return( IDE_CUR_DLL_VER );
}

void IDEAPI IDEStopRunning( void )
/********************************/
{
    LinkState |= LS_STOP_WORKING | LS_LINK_ERROR;
}

void IDEAPI IDEFreeHeap( void )
/*****************************/
{
#if defined( __WATCOMC__ )
    _heapshrink();
#endif
}

IDEBool IDEAPI IDEInitDLL( IDECBHdl cbhdl, IDECallBacks *cb, IDEDllHdl *hdl )
/***************************************************************************/
{
    /* unused parameters */ (void)hdl;

    IdeHdl = cbhdl;
    IdeCbs = cb;
    InitSubSystems();
    return( false );
}

void IDEAPI IDEFiniDLL( IDEDllHdl hdl )
/*************************************/
{
    /* unused parameters */ (void)hdl;

    FiniSubSystems();
}

IDEBool IDEAPI IDERunYourSelf( IDEDllHdl hdl, const char * opts, IDEBool *fatalerr )
/**********************************************************************************/
{
    /* unused parameters */ (void)hdl;

    LinkMainLine( opts );
    *fatalerr = (LinkState & LS_LINK_ERROR) != 0;
    return( *fatalerr );
}

IDEBool IDEAPI IDERunYourSelfArgv( IDEDllHdl hdl, int argc, char **argv, IDEBool *fatalerr )
/******************************************************************************************/
{
    /* unused parameters */ (void)hdl;

#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc; (void)argv;
#endif

    LinkMainLine( NULL );
    *fatalerr = (LinkState & LS_LINK_ERROR) != 0;
    return( *fatalerr );
}
