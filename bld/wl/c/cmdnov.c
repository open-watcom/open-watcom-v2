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


/*
 *  CMDNOV : command line parsing for novell netware file formats.
 *
*/

#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "msg.h"
#include "exenov.h"
#include "loadnov.h"
#include "wlnkmsg.h"
#include "cmdnov.h"

static bool             GetNovImport( void );
static bool             GetNovExport( void );
extern bool             ProcNLM( void );

extern bool ProcNovImport( void )
/*******************************/
{
    return( ProcArgList( GetNovImport, TOK_INCLUDE_DOT ) );
}

extern bool ProcNovExport( void )
/*******************************/
{
    return( ProcArgList( GetNovExport, TOK_INCLUDE_DOT ) );
}

static bool GetNovImport( void )
/******************************/
{
    symbol *        sym;

    sym = SymXOp( ST_DEFINE_SYM, Token.this, Token.len );
    if( sym == NULL || sym->p.import != NULL ) {
        return( TRUE );
    }
    SET_SYM_TYPE( sym, SYM_IMPORTED );
    sym->info |= SYM_DCE_REF;   // make sure we don't try to get rid of these.
    SetNovImportSymbol( sym );
    return( TRUE );
}

extern void SetNovImportSymbol( symbol * sym )
/********************************************/
{
    sym->p.import = DUMMY_IMPORT_PTR;
}

static bool GetNovExport( void )
/******************************/
{
    symbol *    sym;

    sym = SymXOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len );
    sym->info |= SYM_DCE_REF | SYM_EXPORTED;
    AddNameTable( Token.this, Token.len, TRUE, &FmtData.u.nov.exp.export );
    return( TRUE );
}

extern bool ProcScreenName( void )
/********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    }
    if( Token.len > MAX_SCREEN_NAME_LENGTH ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "screenname" );
    } else {
        if( FmtData.u.nov.screenname != NULL ) {
            _LnkFree( FmtData.u.nov.screenname );  // assume second is correct.
        }
        FmtData.u.nov.screenname = tostring();
    }
    return( TRUE );
}

extern bool ProcCheck( void )
/***************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    }
    FmtData.u.nov.checkfn = tostring();
    return( TRUE );
}

extern bool ProcMultiLoad( void )
/*******************************/
{
    FmtData.u.nov.exeflags |= NOV_MULTIPLE;
    return( TRUE );
}

extern bool ProcReentrant( void )
/*******************************/
{
    FmtData.u.nov.exeflags |= NOV_REENTRANT;
    return( TRUE );
}

extern bool ProcSynch( void )
/***************************/
{
    FmtData.u.nov.exeflags |= NOV_SYNCHRONIZE;
    return( TRUE );
}

extern bool ProcPseudoPreemption( void )
/**************************************/
{
    FmtData.u.nov.exeflags |= NOV_PSEUDOPREEMPTION;
    return( TRUE );
}

extern bool ProcNLMFlags( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) ) return FALSE;
    FmtData.u.nov.exeflags |= value;
    return TRUE;
}

extern bool ProcCustom( void )
/****************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( FALSE );
    }
    FmtData.u.nov.customdata = tostring();         // no default extension.
    return( TRUE );
}

extern bool ProcMessages( void )
/******************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( FALSE );
    }
    FmtData.u.nov.messages = tostring();           // no default extension.
    return( TRUE );
}

extern bool ProcHelp( void )
/**************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( FALSE );
    }
    FmtData.u.nov.help = tostring();       // no default extension.
    return( TRUE );
}

extern bool ProcXDCData( void )
/*****************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( FALSE );
    }
    FmtData.u.nov.rpcdata = tostring();    // no default extension.
    return( TRUE );
}

extern bool ProcSharelib( void )
/******************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( FALSE );
    }
    FmtData.u.nov.sharednlm = FileName( Token.this, Token.len, E_NLM, FALSE );
    return( TRUE );
}

extern bool ProcExit( void )
/**************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    }
    FmtData.u.nov.exitfn = tostring();
    return( TRUE );
}

extern bool ProcThreadName( void )
/*******************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    } else if( Token.len > MAX_THREAD_NAME_LENGTH ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "threadname" );
    } else {
        FmtData.u.nov.threadname = tostring();
    }
    return( TRUE );
}

#define COPYRIGHT_START     "Copyright 1991"
#define DEFAULT_COPYRIGHT COPYRIGHT_START " Novell, Inc. All rights reserved"
#define DEFAULT_COPYRIGHT_LENGTH (sizeof( DEFAULT_COPYRIGHT ) - 1)
#define YEAR_OFFSET (sizeof( COPYRIGHT_START ) - 1)

extern bool ProcCopyright( void )
/*******************************/
{
    struct tm *     currtime;
    time_t          thetime;
    unsigned        year;
    char *          copy_year;

    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT )
                && !GetToken( SEP_NO, TOK_INCLUDE_DOT) ) {
        if( FmtData.u.nov.copyright != NULL ) {
            _LnkFree( FmtData.u.nov.copyright );  // assume second is correct.
        }
        _ChkAlloc( FmtData.u.nov.copyright, DEFAULT_COPYRIGHT_LENGTH + 1 );
        memcpy(FmtData.u.nov.copyright,DEFAULT_COPYRIGHT,DEFAULT_COPYRIGHT_LENGTH+1);
        copy_year = FmtData.u.nov.copyright + YEAR_OFFSET;
        thetime = time( NULL );
        currtime = localtime( &thetime );
        year = currtime->tm_year + 1900;
        while( year > 0 ) {
            *copy_year = '0' + (year % 10);
            year /= 10;
            copy_year--;
        }
    } else {
        if( Token.len > MAX_COPYRIGHT_LENGTH ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "copyright" );
        } else {
            if( FmtData.u.nov.copyright != NULL ) {
                _LnkFree( FmtData.u.nov.copyright );  // assume second is correct.
            }
            FmtData.u.nov.copyright = tostring();
        }
    }
    return( TRUE );
}

extern bool ProcNovell( void )
/****************************/
{
    if( !ProcOne( NovModels, SEP_NO, FALSE ) ) {  // get file type
        ProcNLM();
    }
    if( !GetToken( SEP_QUOTE, TOK_INCLUDE_DOT )  ) {    // get description
        FmtData.u.nov.description = NULL;
        return( TRUE );
    }
    if( Token.len > MAX_DESCRIPTION_LENGTH ) {
        LnkMsg( LOC+LINE+ERR+MSG_VALUE_TOO_LARGE, "s", "description" );
    } else {
        FmtData.u.nov.description = tostring();
    }
    return( TRUE );
}

extern void SetNovFmt( void )
/***************************/
{
    Extension = E_NLM;
    if( !(LinkState & FMT_SPECIFIED) && Name != NULL ) {
        FmtData.u.nov.description = FileName( Name, strlen( Name ), E_NLM,
                                                        CmdFlags & CF_UNNAMED );
    }
}

extern void FreeNovFmt( void )
/****************************/
{
    _LnkFree( FmtData.u.nov.screenname );
    _LnkFree( FmtData.u.nov.description );
    _LnkFree( FmtData.u.nov.checkfn );
    _LnkFree( FmtData.u.nov.exitfn );
    _LnkFree( FmtData.u.nov.customdata );
    _LnkFree( FmtData.u.nov.threadname );
    _LnkFree( FmtData.u.nov.copyright );
    _LnkFree( FmtData.u.nov.messages );
    _LnkFree( FmtData.u.nov.help );
    _LnkFree( FmtData.u.nov.rpcdata );
    _LnkFree( FmtData.u.nov.sharednlm );
/*  FreeList( FmtData.u.nov.exp.export );
    FreeList( FmtData.u.nov.exp.module );  Permalloc'd now */
}

extern bool ProcNLM( void )
/*************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = 0;
    return( TRUE );
}

extern bool ProcLAN( void )
/*************************/
{
    Extension = E_LAN;
    FmtData.u.nov.moduletype = 1;
    return( TRUE );
}

extern bool ProcDSK( void )
/*************************/
{
    Extension = E_DSK;
    FmtData.u.nov.moduletype = 2;
    return( TRUE );
}

extern bool ProcNAM( void )
/*************************/
{
    Extension = E_NAM;
    FmtData.u.nov.moduletype = 3;
    return( TRUE );
}

static bool GetNovModule( void )
/******************************/
{
    AddNameTable( Token.this, Token.len, FALSE, &FmtData.u.nov.exp.module );
    return( TRUE );
}

extern bool ProcModule( void )
/****************************/
{
    return( ProcArgList( GetNovModule, TOK_INCLUDE_DOT ) );
}

extern bool ProcOSDomain( void )
/******************************/
{
    FmtData.u.nov.exeflags |= NOV_OS_DOMAIN;
    return TRUE;
}

extern bool ProcNovDBIExports( void )
/***********************************/
{
    FmtData.u.nov.flags |= DO_NOV_EXPORTS;
    return( TRUE );
}

extern bool ProcNovDBIReferenced( void )
/**************************************/
{
    FmtData.u.nov.flags |= DO_NOV_REF_ONLY;
    return TRUE;
}

extern bool ProcNovDBI( void )
/****************************/
{
    LinkFlags |= NOVELL_DBI_FLAG;
    if( ProcOne( NovDBIOptions, SEP_NO, FALSE ) ) {
        while( ProcOne( NovDBIOptions, SEP_COMMA, FALSE ) ); /*null loop*/
    }
    return( TRUE );
}

extern bool ProcExportsDBI( void )
/********************************/
{
    DBIFlag |= DBI_EXPORTS;
    FmtData.u.nov.flags |= DO_WATCOM_EXPORTS;
    return( TRUE );
}

extern void CmdNovFini( void )
/****************************/
{
    if( FmtData.u.nov.description == NULL && Name != NULL ) {
        FmtData.u.nov.description = FileName( Name, strlen( Name ), Extension,
                                                        CmdFlags & CF_UNNAMED );
    }
}
