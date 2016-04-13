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
* Description:  Command line parsing for the ELF load file format.
*
****************************************************************************/


#include "linkstd.h"
#include "loadelf.h"
#include "command.h"
#include "cmdelf.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "exeelf.h"


bool ProcELF( void )
/*************************/
{
    ProcOne( ELFFormatKeywords, SEP_NO, false );
    return true;
}

bool ProcELFDLL( void )
/****************************/
{
    FmtData.dll = true;
    return true;
}

void SetELFFmt( void )
/***************************/
{
    Extension = E_ELF;
    FmtData.u.elf.exp.export = NULL;
    FmtData.u.elf.exp.module = NULL;
    FmtData.u.elf.extrasects = 0;
    FmtData.u.elf.abitype    = 0;
    FmtData.u.elf.abiversion = 0;
}

void FreeELFFmt( void )
/****************************/
{
/*  FreeList( FmtData.u.elf.exp.export );
    FreeList( FmtData.u.elf.exp.module ); Permalloc'd now */
}

void SetELFImportSymbol( symbol * sym )
/********************************************/
{
    sym = sym;
}

bool ProcExportAll( void )
/*******************************/
{
    FmtData.u.elf.exportallsyms = true;
    return true;
}

static bool GetELFImport( void )
/******************************/
{
    symbol      *sym;

    sym = SymOp( ST_DEFINE_SYM, Token.this, Token.len );
    if( sym == NULL ) {
        return( true );
    }
    SET_SYM_TYPE( sym, SYM_IMPORTED );
    sym->info |= SYM_DCE_REF;
    SetELFImportSymbol( sym );
    return( true );
}

bool ProcELFImport( void )
/*******************************/
{
    return( ProcArgList( GetELFImport, TOK_INCLUDE_DOT ) );
}

bool ProcELFAlignment( void )
/**********************************/
{
    ord_state           ret;
    unsigned_32         value;
    unsigned_32         lessone;        // value without the lowest bit.

    if( !HaveEquals( TOK_NORMAL ) ) return( false );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( false );
    }
    for( ;; ) {
        lessone = value & (value - 1);  // remove the low order bit.
        if( lessone == 0 ) break;       // until we are at a power of 2.
        value = lessone;
    }
    FmtData.objalign = value;
    return( true );
}

bool ProcExtraSections( void )
/***********************************/
{
    return GetLong( &FmtData.u.elf.extrasects );
}

bool ProcELFNoRelocs( void )
/*********************************/
{
    LinkState &= ~MAKE_RELOCS;
    return true;
}

static void ParseABITypeAndVersion( void )
/****************************************/
{
    ord_state   retval;
    unsigned_16 type;
    unsigned_16 version;

    FmtData.u.elf.abitype    = 0;
    FmtData.u.elf.abiversion = 0;
    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) )
        return;
    FmtData.u.pe.subminor = 0;
    retval = getatoi( &type );
    if( retval != ST_IS_ORDINAL || type > 255 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ELF ABI type" );
        return;
    }
    FmtData.u.elf.abitype = type;
    if( !GetToken( SEP_PERIOD, TOK_NORMAL ) ) { /* if we don't get ABI version */
       return;                                  /* that's OK */
    }
    retval = getatoi( &version );
    if( retval != ST_IS_ORDINAL || version > 255 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ELF ABI version" );
        return;
    }
    FmtData.u.elf.abiversion = version;
}

static void ParseABIVersion( void )
/*********************************/
{
    ord_state   retval;
    unsigned_16 version;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) )
        return;
    FmtData.u.elf.abiversion = 0;
    retval = getatoi( &version );
    if( retval != ST_IS_ORDINAL || version > 255 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ELF ABI version" );
    } else {
        FmtData.u.elf.abiversion = version;
    }
}

bool ProcELFRNumber( void )
/********************************/
{
    ParseABITypeAndVersion();
    return( true );
}

bool ProcELFRSVR4( void )
/******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_NONE;
    ParseABIVersion();
    return( true );
}

bool ProcELFRNetBSD( void )
/********************************/
{
    FmtData.u.elf.abitype = ELFOSABI_NETBSD;
    ParseABIVersion();
    return( true );
}

bool ProcELFRLinux( void )
/*******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_LINUX;
    ParseABIVersion();
    return( true );
}

bool ProcELFRSolrs( void )
/*******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_SOLARIS;
    ParseABIVersion();
    return( true );
}

bool ProcELFRFBSD( void )
/******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_FREEBSD;
    ParseABIVersion();
    return( true );
}
