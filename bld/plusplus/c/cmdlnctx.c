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
#include "cmdlnctx.h"
#include "cmdscan.h"
#include "context.h"
#include "errdefns.h"
#include "memmgr.h"
#include "vstk.h"


typedef struct ctx_cl_base      CTX_CL_BASE;
typedef struct ctx_cl_file      CTX_CL_FILE;
typedef struct ctx_cl_env       CTX_CL_ENV;
typedef union  ctx_cl           CTX_CL;

struct ctx_cl_base              // CTX_CL_BASE -- base for all contexts
{   CTX_CLTYPE ctx_type;        // - type of context
    unsigned :0;                // - alignment
    char const *sw_ptr;         // - switch pointer
    char const *cmd_line;       // - command line
    char const *cmd_scan;       // - scan position
};

struct ctx_cl_env               // CTX_CL_ENV -- environment variable
{   CTX_CL_BASE base;           // - base
    char const *var;            // - environment variable
};

struct ctx_cl_file              // CTX_CL_FILE -- file of commands
{   CTX_CL_BASE base;           // - base
    SRCFILE source;             // - source file
};

union ctx_cl                    // CTX_CL -- one of:
{   CTX_CL_BASE base;           // - base
    CTX_CL_FILE file;           // - file
    CTX_CL_ENV env;             // - environment variable
};

static VSTK_CTL cmdLnContexts;  // ACTIVE CONTEXTS


void CmdLnCtxInit(              // INITIALIZE CMD-LN CONTEXT
    void )
{
    VstkOpen( &cmdLnContexts, sizeof( CTX_CL ), 8 );
}


void CmdLnCtxFini(              // COMPLETE CMD-LN CONTEXT
    void )
{
    VstkClose( &cmdLnContexts );
}


static CTX_CL* cmdLnCtxAlloc(   // ALLOCATE NEW CONTEXT
    CTX_CLTYPE type )           // - type of context
{
    CTX_CL* entry;

    entry = VstkPush( &cmdLnContexts );
    entry->base.ctx_type = type;
    entry->base.sw_ptr = NULL;
    if( type == CTX_CLTYPE_PGM ) {
        entry->base.cmd_line = NULL;
        entry->base.cmd_scan = NULL;
    } else {
        entry->base.cmd_line = CmdScanAddr();
        entry->base.cmd_scan = CtxGetSwitchAddr();
    }
    return entry;
}


void CmdLnCtxPush(              // PUSH NEW CONTEXT
    CTX_CLTYPE type )           // - type of context
{
    cmdLnCtxAlloc( type );
}


void CmdLnCtxPushEnv(           // PUSH FOR ENVIRONMENT-VARIABLE PROCESSING
    char const *var )           // - environment variable
{
    CTX_CL* entry;              // - new entry
    size_t size;                // - variable size

    entry = cmdLnCtxAlloc( CTX_CLTYPE_ENV );
    size = strlen( var ) + 1;
    entry->env.var = CMemAlloc( size );
    memcpy( (void*)entry->env.var, var, size );
}


void CmdLnCtxPushCmdFile(       // PUSH FOR FILE PROCESSING
    SRCFILE cmdfile )           // - command file
{
    CTX_CL* entry;              // - new entry

    entry = cmdLnCtxAlloc( CTX_CLTYPE_FILE );
    entry->file.source = cmdfile;
}


void CmdLnCtxSwitch(            // RECORD SWITCH POSITION
    char const * sw_ptr )       // - current switch location
{
    CTX_CL* entry;              // - new entry

    entry = VstkTop( &cmdLnContexts );
    entry->base.sw_ptr = (char *)sw_ptr;
}


void CmdLnCtxPop(               // POP A CONTEXT
    void )
{
    CTX_CL* entry;              // - new entry

    entry = VstkPop( &cmdLnContexts );
    if( CTX_CLTYPE_ENV == entry->base.ctx_type ) {
        CMemFree( (void*)entry->env.var );
    }
    if( CTX_CLTYPE_PGM != entry->base.ctx_type ) {
        CmdScanInit( entry->base.cmd_line );
        CtxSwitchAddr( entry->base.cmd_scan );
    }
}


void CmdLnCtxInfo(              // PRINT CONTEXT INFO
    void )
{
    CTX_CL* entry;              // - current entry
    VBUF buf;                   // - buffer

    VbufInit( &buf );
    for( entry = VstkTop( &cmdLnContexts )
       ; entry != NULL
       ; entry = VstkNext( &cmdLnContexts, entry ) ) {
        VbufRewind( &buf );
        switch( entry->base.ctx_type ) {
          case CTX_CLTYPE_ENV :
            VbufConcChr( &buf, '@' );
            VbufConcStr( &buf, entry->env.var );
            break;
          case CTX_CLTYPE_FC :
            VbufConcStr( &buf, "batch file of commands, line " );
            VbufConcDecimal( &buf, CompInfo.fc_file_line );
            break;
          case CTX_CLTYPE_PGM :
            VbufConcStr( &buf, "command line" );
            break;
          case CTX_CLTYPE_FILE :
            VbufConcStr( &buf, SrcFileFullName( entry->file.source ) );
            break;
          DbgDefault( "bad command-line context" );
        }
        if( entry->base.sw_ptr != NULL ) {
            size_t size;
            char const * not_used;
            char const* old = CmdScanAddr();
            CmdScanInit( entry->base.sw_ptr );
            CmdScanChar();
            size = CmdScanOption( &not_used ) + 1;
            CmdScanInit( old );
            VbufConcStr( &buf, ", switch: " );
            for( old = entry->base.sw_ptr; size > 0; ++old, --size ) {
                VbufConcChr( &buf, *old );
            }
        }
        InfMsgPtr( INF_SWITCH, VbufString( &buf ) );
    }
    VbufFree( &buf );
}
