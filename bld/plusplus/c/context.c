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


#include "plusplus.h"
#include "preproc.h"
#include "ring.h"
#include "context.h"
#include "initdefs.h"

#ifndef NDEBUG
#include "dbg.h"
#endif

static const char *ctx_names[] = {
    #define CT( code, text ) text
    #include "_context.h"
    #undef CT
};

static CTX context;             // current context
static CTX last_context;        // last context returned
static void const *last_data;   // last data returned


                                // - CMDLN_ENV, CMDLN_PGM
static char const *switch_addr; // - - address of switch

                                // - FORCED_INCS, SOURCE, FUNC_GEN, CG_FUNC
static SYMBOL func;             // - - function being generated
#ifndef NDEBUG
static LINE_NO line;            // - - current line
#endif

                                // - FORCED_INCS, SOURCE
#ifndef NDEBUG
static TOKEN_LOCN location;     // - - location being analysed
#endif

static NESTED_POST_CONTEXT *registered_post_contexts;

void CtxRegisterPostContext(    // REGISTER A Note! MONITOR
    NESTED_POST_CONTEXT *blk )  // - registration block
{
    RingAppend( &registered_post_contexts, blk );
}

void CtxPostContext             // PERFORM ANY POST-CONTEXT MESSAGING
    ( void )
{
    NESTED_POST_CONTEXT *ctx;

    RingIterBeg( registered_post_contexts, ctx ) {
        ctx->call_back( ctx );
    } RingIterEnd( ctx )
}

void CtxSetCurrContext(         // SET THE CURRENT CONTEXT
    CTX curr )                  // - new context
{
    context = curr;
    CtxSetSwitchAddr( NULL );
    func = NULL;
#ifndef NDEBUG
    line = 0;
    location.src_file = NULL;
#endif
}


void CtxSetSwitchAddr(          // SET ADDRESS OF CURRENT SWITCH
    char const *sw_addr )       // - address of switch
{
    switch_addr = sw_addr;
}


char const *CtxGetSwitchAddr(   // GET CURRENT SWITCH CONTEXT
    void )
{
    return switch_addr;
}


#ifndef NDEBUG
void CtxTokenLocn(              // SET TOKEN LOCATION
    TOKEN_LOCN *locn )          // - token location
{
    location = *locn;
}
#endif


void CtxFunction(               // SET FUNCTION BEING PROCESSED
    SYMBOL curr )               // - current function SYMBOL
{
    func = curr;
}


#ifndef NDEBUG
void CtxLine(                   // SET LINE BEING PROCESSED
    LINE_NO curr )              // - current line no.
{
    line = curr;
}
#endif


#ifndef NDEBUG
void CtxScanToken(              // SET TOKEN LOCATION FOR SCANNED TOKEN
    void )
{
    location.src_file = SrcFileCurrent();
    location.line = TokenLine;
    location.column = TokenColumn;
}
#endif


bool CtxCurrent(                // GET CURRENT CONTEXT
    CTX *a_context,             // - addr[ current context ]
    void const **a_data,              // - addr[ ptr to data for context ]
    char const **a_prefix )     // - addr[ prefix for header line in error ]
{
    bool retb;                  // - true ==> context changed from last time
    void const *data;           // - current data

    *a_context = context;
    *a_prefix = ctx_names[ context ];
    switch( context ) {
      case CTX_INIT :
      case CTX_FINI :
      case CTX_CMDLN_VALID :
      case CTX_FORCED_INCS :
      case CTX_SOURCE :
      case CTX_CG_OPT :
      case CTX_ENDFILE:
        data = NULL;
        break;
      case CTX_CMDLN_ENV :
      case CTX_CMDLN_PGM :
        data = CtxGetSwitchAddr();
        break;
      case CTX_CG_FUNC :
      case CTX_FUNC_GEN :
        data = func;
        break;
    }
    retb = ( context != last_context ) || ( data != last_data );
    last_data = data;
    *a_data = data;
    last_context = context;
    return( retb );
}


#ifndef NDEBUG
static char CompilerContext[80]; // - buffer for debugging

void *CtxWhereAreYou(           // SET DEBUGGING BUFFER
    void )
{
    char *buf;                  // - points into buffer

    buf = stpcpy( CompilerContext, ctx_names[ context ] );
    switch( context ) {
      case CTX_CMDLN_ENV :
      case CTX_CMDLN_PGM :
        buf = stpcpy( buf, ": " );
        buf = stpcpy( buf, CtxGetSwitchAddr() );
        break;
      case CTX_FORCED_INCS :
      case CTX_SOURCE :
        if( location.src_file == NULL ) break;
        buf = stpcpy( buf, ": " );
        buf = stpcpy( buf, SrcFileName( location.src_file ) );
        buf = stpcpy( buf, "(" );
        buf = stdcpy( buf, location.line );
        buf = stpcpy( buf, "," );
        buf = stdcpy( buf, location.column );
        buf = stpcpy( buf, ")" );
        break;
      case CTX_CG_FUNC :
      case CTX_FUNC_GEN :
        buf = stpcpy( buf, ": " );
        if( func == NULL ) {
            buf = stpcpy( buf, "data generation" );
        } else {
            VBUF vbuf;
            buf = stpcpy( buf, DbgSymNameFull( func, &vbuf ) );
            VbufFree( &vbuf );
        }
        if( ( context == CTX_FUNC_GEN )
          &&( line != 0 ) ) {
            buf = stpcpy( buf, "(" );
            buf = stdcpy( buf, location.line );
            buf = stpcpy( buf, ")" );
        }
        break;
    }
    return CompilerContext;
}
#endif


static void init(               // MODULE INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    registered_post_contexts = NULL;
    last_context = CTX_START;
    last_data = NULL;
}


static void fini(               // MODULE COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
}


INITDEFN( context, init, fini )
