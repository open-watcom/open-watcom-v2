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


// CONTEXT.H -- compiler context processing
//
// Compiler context is used to generate headers for error messages, as well as
// for debugging.
//
// 92/07/08 -- J.W.Welch        -- defined

#include "srcfile.h"

#ifndef __CONTEXT_H_DEFNS
#define __CONTEXT_H_DEFNS
typedef struct nested_context NESTED_POST_CONTEXT;
struct nested_context {
    NESTED_POST_CONTEXT *next;
    void                (*call_back)( NESTED_POST_CONTEXT * );
};
#endif

#ifdef __CONTEXT_H__
    #define CT( code, text ) text
    static char *ctx_names[] =
#else
    #define CT( code, text ) code
    typedef enum
#endif


{   CT( CTX_INIT        , "Compiler Initialization"         )
,   CT( CTX_FINI        , "Compiler Completion"             )
,   CT( CTX_CMDLN_ENV   , "Environment command line switch" )
,   CT( CTX_CMDLN_PGM   , "Program command line switch"     )
,   CT( CTX_CMDLN_VALID , "Command line validation"         )
,   CT( CTX_FORCED_INCS , "Compiling forced includes"       )
,   CT( CTX_SOURCE      , "Compiling source"                )
,   CT( CTX_FUNC_GEN    , "Compiling generated function"    )
,   CT( CTX_CG_FUNC     , "Code generation for"             )
,   CT( CTX_CG_OPT      , "Code optimization"               )
,   CT( CTX_ENDFILE     , "End of file"                     )
,   CT( CTX_START       , "Start-up"                        )
}

#undef CT

#ifdef __CONTEXT_H__
    ;
#else
    CTX;
    #define __CONTEXT_H__

// PROTOTYPES : always called

bool CtxCurrent(                // GET CURRENT CONTEXT
    CTX *a_context,             // - addr[ current context ]
    void **a_data,              // - addr[ ptr to data for context ]
    char **a_prefix )           // - addr[ prefix for header line in error ]
;
void CtxFunction(               // SET FUNCTION BEING PROCESSED
    SYMBOL curr )               // - current function SYMBOL
;
char const *CtxGetSwitchAddr(   // GET CURRENT SWITCH CONTEXT
    void )
;
void CtxSetContext(             // SET THE CURRENT CONTEXT
    CTX curr )                  // - new context
;
void CtxSwitchAddr(             // SET ADDRESS OF CURRENT SWITCH
    char const *sw_addr )       // - address of switch
;

// PROTOTYPES : debugging only (stubbed otherwise)

#ifndef NDEBUG
void CtxLine(                   // SET LINE BEING PROCESSED
    LINE_NO curr )              // - current line no.
;
void CtxScanToken(              // SET TOKEN LOCATION FOR SCANNED TOKEN
    void )
;
void CtxTokenLocn(              // SET TOKEN LOCATION
    TOKEN_LOCN *locn )          // - token location
;
void* CtxWhereAreYou(           // SET DEBUGGING BUFFER
    void )
;
#else
    #define CtxLine( curr )
    #define CtxScanToken()
    #define CtxTokenLocn( curr )
    #define CtxWhereAreYou()
#endif

void CtxRegisterPostContext(    // REGISTER A Note! MONITOR
    NESTED_POST_CONTEXT *blk )  // - registration block
;
void CtxPostContext(            // PERFORM ANY POST-CONTEXT MESSAGING
    void )
;

#endif
