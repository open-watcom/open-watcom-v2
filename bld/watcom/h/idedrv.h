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
* Description:  IDE driver library interface.
*
****************************************************************************/


#ifndef __IDEDRV_H__
#define __IDEDRV_H__

#if defined( CHAIN_CALLBACK )
#define IdeDrvExecDLL           _IdeDrvExecDLL
#define IdeDrvExecDLLArgv       _IdeDrvExecDLLArgv
#define IdeDrvInit              _IdeDrvInit
#define IdeDrvPrintError        _IdeDrvPrintError
#define IdeDrvUnloadDLL         _IdeDrvUnloadDLL
#define IdeDrvStopRunning       _IdeDrvStopRunning
#define IdeDrvChainCallbacks    _IdeDrvChainCallbacks
#define IdeDrvGetCallbacks      _IdeDrvGetCallbacks
#define IdeDrvSetCallbacks      _IdeDrvSetCallbacks
#endif

#define __IDEDRV \
  _IDEDRV( IDEDRV_SUCCESS       , 0 ) \
, _IDEDRV( IDEDRV_ERR_LOAD      , "cannot load dll" ) \
, _IDEDRV( IDEDRV_ERR_UNLOAD    , "cannot unload dll" ) \
, _IDEDRV( IDEDRV_ERR_INIT      , "cannot find init routine" ) \
, _IDEDRV( IDEDRV_ERR_INIT_EXEC , "error return from init routine" ) \
, _IDEDRV( IDEDRV_ERR_INFO      , "cannot find init-info routine" ) \
, _IDEDRV( IDEDRV_ERR_INFO_EXEC , "error return from init-info routine" ) \
, _IDEDRV( IDEDRV_ERR_RUN       , "cannot find run-self routine" ) \
, _IDEDRV( IDEDRV_ERR_RUN_EXEC  , "error return from run-self routine" ) \
, _IDEDRV( IDEDRV_ERR_RUN_FATAL , "fatal return from run-self routine" )

typedef enum                    // error codes
{
#define _IDEDRV(e,m) e
__IDEDRV
#undef _IDEDRV
, IDEDRV_ERR_MAXIMUM
} IDEDRV_STATUS;

typedef struct {                // IDEDRV structure
    char const *dll_name;       // * dll name
    char const *ent_name;       // * NULL or entry name
    void *ide_handle;           // # handle, when WATCOM IDE
    unsigned long drv_status;   // # status: from IDEDRV (IDEDRV_STATUS)
    unsigned long dll_status;   // # status: from DLL
    void *dll_handle;           // $ handle for a loaded DLL
    unsigned loaded         :1; // # TRUE ==> dll is loaded

                                // * filled in by caller
                                // # filled in by IDEDRV (public)
                                // $ filled in by IDEDRV (private)
} IDEDRV;

// PROTOTYPES

int IdeDrvExecDLL               // EXECUTE THE DLL ONE TIME (LOAD IF REQ'D)
    ( IDEDRV *inf               // - driver control information
    , char const *cmd_line )    // - command line
;
int IdeDrvExecDLLArgv           // EXECUTE THE DLL ONE TIME (LOAD IF REQ'D)
    ( IDEDRV *inf               // - driver control information
    , int argc                  // - # of arguments
    , char **argv )             // - argument vector
;
void IdeDrvInit                 // INITIALIZE IDEDRV INFORMATION
    ( IDEDRV *inf               // - information
    , char const *dll_name      // - dll name
    , char const *ent_name )    // - entry name
;
int IdeDrvPrintError            // UNLOAD THE DLL
    ( IDEDRV *inf )             // - driver control information
;
int IdeDrvUnloadDLL             // UNLOAD THE DLL
    ( IDEDRV *inf )             // - driver control information
;
int IdeDrvStopRunning           // SIGNAL A BREAK
    ( IDEDRV *inf )             // - driver control information
;

void IdeDrvChainCallbacks       // SET CALLBACKS FOR DLL CALLLING A DLL
    ( void *cb                  // - parent dll callbacks
    , void *info )              // - parent dll initialization
;

void *IdeDrvGetCallbacks        // GET CALLBACKS (TO FILL IN BLANKS)
    ( void )
;
void IdeDrvSetCallbacks         // SET CALLBACKS (TO FILL IN BLANKS)
    ( void *cb )
;
#endif
