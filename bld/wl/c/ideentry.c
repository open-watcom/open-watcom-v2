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
* Description:  IDE interface entry points for wlink.
*
****************************************************************************/

#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ )
#include <malloc.h>
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

#define PREFIX_SIZE 8

typedef struct {
    IDEInfoType type;
    char        prefix[PREFIX_SIZE + 1];
    bool        retry;
} extra_cmd_info;

static IDECBHdl         IdeHdl;
static IDEInitInfo      InitInfo;
static IDECallBacks     *IdeCB;

#if defined( __OS2__ )
//extern int InitMsg( void );
//extern int FiniMsg( void );

static bool     RunOnce;
#endif

static extra_cmd_info ExtraCmds[] = {
    IDE_GET_TARGET_FILE,"name    ",     FALSE,
    IDE_GET_OBJ_FILE,   "file    ",     TRUE,
    IDE_GET_LIB_FILE,   "lib     ",     TRUE,
    IDE_GET_RES_FILE,   "opt res=",     FALSE,
    0,                  "\0",           FALSE
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
bool ExecDLLPgm( char *pname, char *cmdline )
/********************************************/
// return TRUE if an error
{
    IDEDRV              inf;
    IDEDRV_STATUS       status;

    status = IDEDRV_ERR_LOAD;
    IdeDrvInit( &inf, pname, NULL );
    IdeDrvChainCallbacks( IdeCB, &InitInfo );
    status = IdeDrvExecDLL( &inf, cmdline );
    IdeDrvUnloadDLL( &inf );
    return( status != IDEDRV_SUCCESS );
}
#endif

/* routines which are called by the linker core */

void WriteStdOut( char *str )
/**********************************/
{
    CheckBreak();
    if( IdeCB != NULL ) {
        IdeCB->PrintWithCRLF( IdeHdl, str );
    }
}

void WriteStdOutNL( void )
/*******************************/
{
    if( IdeCB != NULL ) {
        IdeCB->PrintWithCRLF( IdeHdl, "\n" );
    }
}

void WriteStdOutInfo( char *str, unsigned level, char *symbol )
/********************************************************************/
{
    IDEMsgInfo  info;
    unsigned    msgclass;

    CheckBreak();
    if( IdeCB != NULL ) {
        IdeMsgInit( &info, SeverityMap[(level & CLASS_MSK) >> NUM_SHIFT], str );
        msgclass = level & CLASS_MSK;
        if( msgclass != BANNER && msgclass >= (WRN & CLASS_MSK) ) {
            IdeMsgSetMsgNo( &info, CalcMsgNum( level ) );
            IdeMsgSetHelp( &info, "wlnkerrs.hlp", level & NUM_MSK );
        }
        if( symbol != NULL ) {
            IdeMsgSetLnkSymbol( &info, symbol );
        }
        IdeCB->PrintWithInfo( IdeHdl, &info );
    }
}

char * GetEnvString( char *envname )
/*****************************************/
{
    char *retval;

    if( IdeCB == NULL || InitInfo.ignore_env )
        return( NULL );
    IdeCB->GetInfo( IdeHdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)envname, (IDEGetInfoLParam)&retval );
    return( retval );
}

bool IsStdOutConsole( void )
/*********************************/
{
    return InitInfo.console_output;
}

static bool GetAddtlCommand( IDEInfoType cmd, char *buf )
/*******************************************************/
{
    cmd = cmd;
    buf = buf;
    return FALSE;
#if 0
    if( InitInfo.cmd_line_has_files ) return FALSE;
    return !IdeCB->GetInfo( IdeHdl, cmd, NULL, (IDEGetInfoLParam)&buf );
#endif
}

void GetExtraCommands( void )
/***************************/
{
    extra_cmd_info const    *cmd;
    char                    buff[_MAX_PATH + PREFIX_SIZE];

    for( cmd = ExtraCmds; cmd->prefix[0] != '\0'; ++cmd ) {
        for( ;; ) {
            memcpy( buff, cmd->prefix, PREFIX_SIZE );
            if( !GetAddtlCommand( cmd->type, buff + PREFIX_SIZE ) )
                break;
            if( DoBuffCmdParse( buff ) )
                break;
            if( !cmd->retry ) {
                break;
            }
        }
    }
}

/* routines which are called by the DLL driver */

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
/****************************************************************/
{
    hdl = hdl;
    if( info->ver < IDE_CUR_INFO_VER5 ) return TRUE;
    InitInfo = *info;
    return FALSE;
}

unsigned IDEAPI IDEGetVersion( void )
/***********************************/
{
    return IDE_CUR_DLL_VER;
}

void IDEAPI IDEStopRunning( void )
/********************************/
{
    LinkState |= STOP_WORKING|LINK_ERROR;
}

void IDEAPI IDEFreeHeap( void )
/*****************************/
{
#if defined( __WATCOMC__ )
    _heapshrink();
#endif
}

IDEBool IDEAPI IDEInitDLL( IDECBHdl hdl, IDECallBacks *cb, IDEDllHdl *info )
/**************************************************************************/
{
    info = info;
    IdeHdl = hdl;
    IdeCB = cb;
    InitSubSystems();
#if defined( __OS2__ )
    RunOnce = FALSE;
#endif
    return FALSE;
}

void IDEAPI IDEFiniDLL( IDEDllHdl hdl )
/*************************************/
{
    hdl = hdl;
    FiniSubSystems();
}

IDEBool IDEAPI IDERunYourSelf( IDEDllHdl hdl, const char * opts, IDEBool *fatalerr )
/**********************************************************************************/
{
    hdl = hdl;
#if defined( __OS2__ )
    if( RunOnce ) {
        InitMsg();
    }
    RunOnce = TRUE;
#endif
    LinkMainLine( (char *) opts );
#if defined( __OS2__ )
    FiniMsg();
#endif
    *fatalerr = (LinkState & LINK_ERROR) != 0;
    return( *fatalerr );
}

IDEBool IDEAPI IDERunYourSelfArgv( IDEDllHdl hdl, int argc, char **argv, IDEBool *fatalerr )
/******************************************************************************************/
{
    argc = argc;        /* to avoid a warning */
    argv = argv;
    hdl = hdl;
#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif
    LinkMainLine( NULL );
    *fatalerr = (LinkState & LINK_ERROR) != 0;
    return( *fatalerr );
}
