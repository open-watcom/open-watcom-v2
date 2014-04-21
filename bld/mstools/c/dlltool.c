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


#include <windows.h>
#include "bool.h"
#include "dlltool.h"
#include "error.h"
#include "memory.h"

#define WLIB_DLL_FILENAME       "wlibd.dll"
#if defined( __NT__ ) && defined( __386__ )
#define DLL_GETVER              "_IDEGetVersion@0"
#define DLL_INITDLL             "_IDEInitDLL@12"
#define DLL_RUNSELF             "_IDERunYourSelf@12"
#define DLL_FINIDLL             "_IDEFiniDLL@4"
#define DLL_STOPRUN             "_IDEStopRunning@0"
#define DLL_INITINFO            "_IDEPassInitInfo@8"
#else
#define DLL_GETVER              "IDEGetVersion"
#define DLL_INITDLL             "IDEInitDLL"
#define DLL_RUNSELF             "IDERunYourSelf"
#define DLL_FINIDLL             "IDEFiniDLL"
#define DLL_STOPRUN             "IDEStopRunning"
#define DLL_INITINFO            "IDEPassInitInfo"
#endif


/*
 * Types.
 */
typedef char                    WBool;          /* really a C++ 'bool' */
typedef unsigned IDEAPI  (*GetVerFn)( void );
typedef WBool IDEAPI     (*InitDllFn)( IDECBHdl hdl, IDECallBacks *cb,
                                              IDEDllHdl *info );
typedef WBool IDEAPI     (*InitInfoFn)( IDEDllHdl hdl,
                                               IDEInitInfo *info );
typedef WBool IDEAPI     (*RunSelfFn)( IDEDllHdl hdl, const char *opts,
                                              WBool *fatalerr );
typedef void IDEAPI      (*FiniDllFn)( IDEDllHdl hdl );
typedef void IDEAPI      (*StopRunFn)( void );

typedef struct {
    HINSTANCE           dllhandle;
    GetVerFn            getversion;
    InitDllFn           initdll;
    InitInfoFn          initinfo;
    RunSelfFn           runyourself;
    FiniDllFn           finidll;
    StopRunFn           stoprunning;
    IDECallBacks1       callbacks_ver1;
    IDECallBacks        callbacks;
    IDEDllHdl           dllHdl;
    IDEInitInfo         initdata;
} DllTool;


/*
 * If a callback function isn't given to InitDllTool, the appropriate one
 * of these do-nothing functions will be used.
 */
static IDEBool __stdcall print_message( IDECBHdl hdl, const char *text )
{
    hdl = hdl; text = text;
    return( 1 );
}
static IDEBool __stdcall print_message_crlf( IDECBHdl hdl, const char *text )
{
    hdl = hdl; text = text;
    return( 1 );
}
static IDEBool __stdcall print_with_info2( IDECBHdl hdl, IDEMsgInfo2 *info )
{
    hdl = hdl; info = info;
    return( 1 );
}
static IDEBool __stdcall print_with_info( IDECBHdl hdl, IDEMsgInfo *info )
{
    hdl = hdl; info = info;
    return( 1 );
}


/*
 * Load a DLL.  Returns NULL on failure.
 */
void *InitDllTool( int whichtool, const DllToolCallbacks *callbacks )
/*******************************************************************/
{
    DllTool *           tool = AllocMem( sizeof( DllTool ) );
    unsigned            dllversion;
    IDECallBacks *      dllcallbacks;

    /*** Load the DLL ***/
    switch( whichtool ) {
      case DLLTOOL_WLIB:
        tool->dllhandle = LoadLibrary( WLIB_DLL_FILENAME );
        break;
      default:
        Zoinks();
    };
    if( tool->dllhandle == NULL ) {
        return( NULL );
    }

    /*** Grab the entry points ***/
    tool->getversion = (GetVerFn)GetProcAddress( tool->dllhandle, DLL_GETVER );
    tool->initdll = (InitDllFn)GetProcAddress( tool->dllhandle, DLL_INITDLL );
    tool->initinfo = (InitInfoFn)GetProcAddress( tool->dllhandle, DLL_INITINFO );
    tool->runyourself = (RunSelfFn)GetProcAddress( tool->dllhandle, DLL_RUNSELF );
    tool->finidll = (FiniDllFn)GetProcAddress( tool->dllhandle, DLL_FINIDLL );
    tool->stoprunning = (StopRunFn)GetProcAddress( tool->dllhandle, DLL_STOPRUN );
    if( tool->getversion == NULL  ||  tool->initdll == NULL  ||
        tool->runyourself == NULL  || tool->finidll == NULL  ||
        tool->stoprunning == NULL ) {
        FreeLibrary( tool->dllhandle );
        return( NULL );
    }

    /*** Set up the callbacks ***/
    dllversion = tool->getversion();
    if( dllversion == 1 ) {
        memset( &tool->callbacks_ver1, 0, sizeof( IDECallBacks1 ) );
        if( callbacks->printmessage != NULL ) {
            tool->callbacks_ver1.PrintMessage = callbacks->printmessage;
        } else {
            tool->callbacks_ver1.PrintMessage = print_message;
        }
        if( callbacks->printmessageCRLF != NULL ) {
            tool->callbacks_ver1.PrintWithCRLF = callbacks->printmessageCRLF;
        } else {
            tool->callbacks_ver1.PrintWithCRLF = print_message_crlf;
        }
        tool->callbacks_ver1.GetInfo = NULL;
        tool->callbacks_ver1.RunBatch = NULL;
        dllcallbacks = (IDECallBacks*) &tool->callbacks_ver1;
    } else if( dllversion > 1 ) {
        memset( &tool->callbacks, 0, sizeof( IDECallBacks ) );
        if( callbacks->printmessage != NULL ) {
            tool->callbacks.PrintMessage = callbacks->printmessage;
        } else {
            tool->callbacks.PrintMessage = print_message;
        }
        if( callbacks->printmessageCRLF != NULL ) {
            tool->callbacks.PrintWithCRLF = callbacks->printmessageCRLF;
        } else {
            tool->callbacks.PrintWithCRLF = print_message_crlf;
        }
        if( dllversion == 2 ) {
            if( callbacks->printwithinfo != NULL ) {
                tool->callbacks.PrintWithInfo = (IDEMsgInfoFn)callbacks->printwithinfo2;
            } else {
                tool->callbacks.PrintWithInfo = (IDEMsgInfoFn)print_with_info2;
            }
        } else {                        /* dllversion >= 3 */
            if( callbacks->printwithinfo != NULL ) {
                tool->callbacks.PrintWithInfo = callbacks->printwithinfo;
            } else {
                tool->callbacks.PrintWithInfo = print_with_info;
            }
        }
        tool->callbacks.GetInfo = NULL;
        tool->callbacks.RunBatch = NULL;
        tool->callbacks.ProgressMessage = NULL;
        dllcallbacks = &tool->callbacks;
    } else {
        FreeLibrary( tool->dllhandle );
        return( NULL );
    }

    /*** Tell the DLL to initialize itself ***/
    if( tool->initdll( (IDECBHdl)callbacks->cookie, dllcallbacks,
                       &tool->dllHdl ) ) {
        FreeLibrary( tool->dllhandle );
        return( NULL );
    }
    if( tool->initinfo != NULL ) {
        memset( &tool->initdata, 0, sizeof( tool->initdata ) );
        tool->initdata.ver = IDE_CUR_INFO_VER;
        tool->initdata.ignore_env = 1;
        tool->initdata.cmd_line_has_files = 1;
        tool->initinfo( tool->dllHdl, &tool->initdata );
    }

    return( (void*)tool );
}


/*
 * Unload a DLL.
 */
void FiniDllTool( void *_tool )
/*****************************/
{
    DllTool *           tool = (DllTool*)_tool;

    tool->finidll( tool->dllHdl );
    FreeLibrary( tool->dllhandle );
}


/*
 * Run a DLL.  Returns 0 on failure.
 */
int RunDllTool( void *_tool, const char *cmdline )
/************************************************/
{
    DllTool *           tool = (DllTool*)_tool;
    WBool               fatal = 0;
    WBool               rc;

    rc = tool->runyourself( tool->dllHdl, cmdline, &fatal );
    if( rc || fatal ) {
        return( 0 );
    } else {
        return( 1 );
    }
}
