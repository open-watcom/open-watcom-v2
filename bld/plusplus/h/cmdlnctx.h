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


#ifndef __CMDLNCTX_H__
#define __CMDLNCTX_H__

typedef enum                    // CTX_CLTYPE -- type of context
{ CTX_CLTYPE_PGM                // - program's command line
, CTX_CLTYPE_FC                 // - processing -fc file
, CTX_CLTYPE_ENV                // - processing environment variable
, CTX_CLTYPE_FILE               // - processing a file of command lines
} CTX_CLTYPE;

// PROTOTYPES

void CmdLnCtxFini(              // COMPLETE CMD-LN CONTEXT
    void )
;
void CmdLnCtxInfo(              // PRINT CONTEXT INFO
    void )
;
void CmdLnCtxInit(              // INITIALIZE CMD-LN CONTEXT
    void )
;
void CmdLnCtxPop(               // POP A CONTEXT
    void )
;
void CmdLnCtxPush(              // PUSH NEW CONTEXT
    CTX_CLTYPE type )           // - type of context
;
void CmdLnCtxPushCmdFile(       // PUSH FOR FILE PROCESSING
    SRCFILE cmdfile )           // - command file
;
void CmdLnCtxPushEnv(           // PUSH FOR ENVIRONMENT-VARIABLE PROCESSING
    char const *var )           // - environment variable
;
void CmdLnCtxSwitch(            // RECORD SWITCH POSITION
    char const * sw_ptr )       // - current switch location
;


#endif // __CMDLNCTX_H__
