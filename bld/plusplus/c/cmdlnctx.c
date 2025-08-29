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


#include "plusplus.h"
#include "cmdlnctx.h"
#include "cmdscan.h"
#include "context.h"
#include "memmgr.h"
#include "vstk.h"
#include "compinfo.h"


typedef struct ctx_cl_base      CTX_CL_BASE;
typedef struct ctx_cl_file      CTX_CL_FILE;
typedef struct ctx_cl_env       CTX_CL_ENV;
typedef union  ctx_cl           CTX_CL;

/*
 * CTX_CL_BASE -- base for all contexts�
 */
struct ctx_cl_base
{   CTX_CLTYPE ctx_type;        // - type of context
    unsigned    :0;             // - alignment
    char const  *sw_ptr;        // - switch pointer
    char const  *cmd_line;      // - command line
    char const  *cmd_scan;      // - scan position
};
/*
 * CTX_CL_ENV -- environment variable
 */
struct ctx_cl_env
{   CTX_CL_BASE base;           // - base
    char const *var;            // - environment variable
};
/*
 * CTX_CL_FILE -- file of commands
 */
struct ctx_cl_file
{   CTX_CL_BASE base;           // - base
    SRCFILE source;             // - source file
};
/*
 * CTX_CL -- one of:
 */
union ctx_cl
{   CTX_CL_BASE base;           // - base
    CTX_CL_FILE file;           // - file
    CTX_CL_ENV env;             // - environment variable
};
/*
 * ACTIVE CONTEXTS
 */
static VSTK_CTL cmdLnContexts;

void CmdLnCtxInit( void )
/************************
 * INITIALIZE CMD-LN CONTEXT
 */
{
    VstkOpen( &cmdLnContexts, sizeof( CTX_CL ), 8 );
}


void CmdLnCtxFini( void )
/************************
 * COMPLETE CMD-LN CONTEXT
 */
{
    VstkClose( &cmdLnContexts );
}


static CTX_CL* cmdLnCtxAlloc( CTX_CLTYPE type )
/**********************************************
 * ALLOCATE NEW CONTEXT
 * type - type of context
 */
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
    return( entry );
}


void CmdLnCtxPush( CTX_CLTYPE type )
/***********************************
 * PUSH NEW CONTEXT
 * type - type of context
 */
{
    cmdLnCtxAlloc( type );
}


void CmdLnCtxPushEnv( char const *var )
/**************************************
 * PUSH FOR ENVIRONMENT-VARIABLE PROCESSING
 * var - environment variable
 */
{
    CTX_CL* entry;              // - new entry
    size_t size;                // - variable size

    entry = cmdLnCtxAlloc( CTX_CLTYPE_ENV );
    size = strlen( var ) + 1;
    entry->env.var = CMemAlloc( size );
    memcpy( (void*)entry->env.var, var, size );
}


void CmdLnCtxPushCmdFile( SRCFILE cmdfile )
/******************************************
 * PUSH FOR FILE PROCESSING
 * cmdfile - command file
 */
{
    CTX_CL* entry;              // - new entry

    entry = cmdLnCtxAlloc( CTX_CLTYPE_FILE );
    entry->file.source = cmdfile;
}


void CmdLnCtxSwitch( char const * sw_ptr )
/*****************************************
 * RECORD SWITCH POSITION
 * sw_ptr - current switch location
 */
{
    CTX_CL* entry;              // - new entry

    entry = VstkTop( &cmdLnContexts );
    entry->base.sw_ptr = sw_ptr;
}


void CmdLnCtxPop( void )
/***********************
 * POP A CONTEXT
 */
{
    CTX_CL* entry;              // - new entry

    entry = VstkPop( &cmdLnContexts );
    if( CTX_CLTYPE_ENV == entry->base.ctx_type ) {
        CMemFree( (void*)entry->env.var );
    }
    if( CTX_CLTYPE_PGM != entry->base.ctx_type ) {
        CmdScanLineInit( entry->base.cmd_line );
        CtxSetSwitchAddr( entry->base.cmd_scan );
    }
}


void CmdLnCtxInfo( void )
/************************
 * PRINT CONTEXT INFO
 */
{
    CTX_CL* entry;              // - current entry
    VBUF buf;                   // - buffer

    VbufInit( &buf );

    VstkIterBeg( &cmdLnContexts, entry ) {
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
            char const *not_used;
            bool quoted;
            char const *old = CmdScanAddr();
            CmdScanLineInit( entry->base.sw_ptr );
            CmdScanChar();
            CmdScanOption( &not_used, &quoted );
            size = CmdScanAddr() - entry->base.sw_ptr;
            CmdScanLineInit( old );
            VbufConcStr( &buf, ", switch: " );
            VbufConcVector( &buf, size, entry->base.sw_ptr );
        }
        InfMsgPtr( INF_SWITCH, VbufString( &buf ) );
    }
    VbufFree( &buf );
}
