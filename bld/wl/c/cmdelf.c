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
 *  CMDELF : command line parsing for the ELF load file format.
 *
*/

#include "linkstd.h"
#include "loadelf.h"
#include "command.h"
#include "cmdelf.h"

extern bool ProcELF( void )
/*************************/
{
    ProcOne( ELFFormatKeywords, SEP_NO, FALSE );
    return TRUE;
}

extern bool ProcELFDLL( void )
/****************************/
{
    FmtData.dll = TRUE;
    return TRUE;
}

extern void SetELFFmt( void )
/***************************/
{
    Extension = E_ELF;
    FmtData.u.elf.exp.export = NULL;
    FmtData.u.elf.exp.module = NULL;
    FmtData.u.elf.extrasects = 0;
}

extern void FreeELFFmt( void )
/****************************/
{
/*  FreeList( FmtData.u.elf.exp.export );
    FreeList( FmtData.u.elf.exp.module ); Permalloc'd now */
}

extern void SetELFImportSymbol( symbol * sym )
/********************************************/
{
    sym = sym;
}

extern bool ProcExportAll( void )
/*******************************/
{
    FmtData.u.elf.exportallsyms = TRUE;
    return TRUE;
}

static bool GetELFImport( void )
/******************************/
{
    symbol *        sym;

    sym = SymXOp( ST_DEFINE_SYM, Token.this, Token.len );
    if( sym == NULL ) {
        return( TRUE );
    }
    SET_SYM_TYPE( sym, SYM_IMPORTED );
    sym->info |= SYM_DCE_REF;
    SetELFImportSymbol( sym );
    return( TRUE );
}

extern bool ProcELFImport( void )
/*******************************/
{
    return( ProcArgList( GetELFImport, TOK_INCLUDE_DOT ) );
}

extern bool ProcELFAlignment( void )
/**********************************/
{
    ord_state           ret;
    unsigned long       value;
    unsigned long       lessone;        // value without the lowest bit.

    if( !HaveEquals(0) ) return( FALSE );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( FALSE );
    }
    for(;;) {
        lessone = value & (value - 1);  // remove the low order bit.
        if( lessone == 0 ) break;       // until we are at a power of 2.
        value = lessone;
    }
    FmtData.objalign = value;
    return( TRUE );
}

extern bool ProcExtraSections( void )
/***********************************/
{
    return GetLong( &FmtData.u.elf.extrasects );
}
