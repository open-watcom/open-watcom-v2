/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "cmdutils.h"
#include "cmdelf.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "exeelf.h"


#ifdef _ELF

void SetELFFmt( void )
/********************/
{
    Extension = E_ELF;
    FmtData.u.elf.exp.export = NULL;
    FmtData.u.elf.exp.module = NULL;
    FmtData.u.elf.extrasects = 0;
    FmtData.u.elf.abitype    = 0;
    FmtData.u.elf.abiversion = 0;
}

void FreeELFFmt( void )
/*********************/
{
/*  FreeList( FmtData.u.elf.exp.export );
    FreeList( FmtData.u.elf.exp.module ); Permalloc'd now */
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcExtraSections( void )
/***********************************/
{
    return( GetLong( &FmtData.u.elf.extrasects ) );
}

static bool ProcExportAll( void )
/*******************************/
{
    FmtData.u.elf.exportallsyms = true;
    return( true );
}

static parse_entry  MainOptions[] = {
    "EXTRASections",    ProcExtraSections,  MK_ELF, 0,
    "EXPORTAll",        ProcExportAll,      MK_ELF, 0,
    NULL
};

bool ProcELFOptions( void )
/*************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}

bool ProcELFAlignment( void )
/***************************/
{
    ord_state           ret;
    unsigned_32         value;
    unsigned_32         lessone;        // value without the lowest bit.

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( false );
    }
    for( ;; ) {
        lessone = value & (value - 1);  // remove the low order bit.
        if( lessone == 0 )
            break;                      // until we are at a power of 2.
        value = lessone;
    }
    FmtData.objalign = value;
    return( true );
}

bool ProcELFNoRelocs( void )
/**************************/
{
    LinkState &= ~LS_MAKE_RELOCS;
    return( true );
}


/****************************************************************
 * "IMPort" Directive
 ****************************************************************/

void SetELFImportSymbol( symbol * sym )
/*************************************/
{
    /* unused parameters */ (void)sym;
}

static bool GetELFImport( void )
/******************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE_DEFINE_NOALIAS, Token.this, Token.len );
    if( sym == NULL ) {
        return( true );
    }
    SET_SYM_TYPE( sym, SYM_IMPORTED );
    sym->info |= SYM_DCE_REF;
    SetELFImportSymbol( sym );
    return( true );
}

bool ProcELFImport( void )
/************************/
{
    return( ProcArgList( GetELFImport, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "EXPort" Directive
 ****************************************************************/

static void SetELFExportSymbol( symbol * sym )
/********************************************/
{
    /* unused parameters */ (void)sym;
}

static bool GetELFExport( void )
/******************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE_DEFINE_NOALIAS, Token.this, Token.len );
    if( sym == NULL ) {
        return( true );
    }
    SET_SYM_TYPE( sym, SYM_EXPORTED );
    sym->info |= SYM_DCE_REF;
    SetELFExportSymbol( sym );
    return( true );
}

bool ProcELFExport( void )
/************************/
{
    return( ProcArgList( GetELFExport, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "MODule" Directive
 ****************************************************************/

static bool GetELFModule( void )
/******************************/
{
    AddNameTable( Token.this, Token.len, false, &FmtData.u.elf.exp.module );
    return( true );
}

bool ProcELFModule( void )
/************************/
{
    return( ProcArgList( GetELFModule, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "RUntime" Directive
 ****************************************************************/

static void ParseABITypeAndVersion( void )
/****************************************/
{
    version_state   result;
    version_block   vb;

    FmtData.u.elf.abitype = 0;
    FmtData.u.elf.abiversion = 0;
    /* set required limits, 0 = no limit */
    vb.major = 255;
    vb.minor = 255;
    vb.revision = 0;
    vb.message = "ABIVER";
    result = GetGenVersion( &vb, GENVER_MAJOR | GENVER_MINOR, false );
    if( result != GENVER_ERROR ) {
        FmtData.u.elf.abitype = vb.major;
        FmtData.u.elf.abiversion = vb.minor;
    }
}

static void ParseABIVersion( const char *message )
/************************************************/
{
    ord_state   retval;
    unsigned_16 version;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) )
        return;
    FmtData.u.elf.abiversion = 0;
    retval = getatoi( &version );
    if( retval != ST_IS_ORDINAL || version > 255 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", message );
    } else {
        FmtData.u.elf.abiversion = version;
    }
}

static bool ProcELFRNumber( void )
/********************************/
{
    ParseABITypeAndVersion();
    return( true );
}

static bool ProcELFRSVR4( void )
/******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_NONE;
    ParseABIVersion( "SVR4" );
    return( true );
}

static bool ProcELFRNetBSD( void )
/********************************/
{
    FmtData.u.elf.abitype = ELFOSABI_NETBSD;
    ParseABIVersion( "NETBSD" );
    return( true );
}

static bool ProcELFRLinux( void )
/*******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_LINUX;
    ParseABIVersion( "LINUX" );
    return( true );
}

static bool ProcELFRSolrs( void )
/*******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_SOLARIS;
    ParseABIVersion( "FREEBSD" );
    return( true );
}

static bool ProcELFRFBSD( void )
/******************************/
{
    FmtData.u.elf.abitype = ELFOSABI_FREEBSD;
    ParseABIVersion( "SOLARIS" );
    return( true );
}

static parse_entry  RunOptions[] = {
    "ABIver",       ProcELFRNumber,     MK_ELF, 0,
    "SVR4",         ProcELFRSVR4,       MK_ELF, 0,
    "NETbsd",       ProcELFRNetBSD,     MK_ELF, 0,
    "LINux",        ProcELFRLinux,      MK_ELF, 0,
    "FREebsd",      ProcELFRFBSD,       MK_ELF, 0,
    "SOLaris",      ProcELFRSolrs,      MK_ELF, 0,
    NULL
};

bool ProcELFRuntime( void )
/*************************/
{
    return( ProcOne( RunOptions, SEP_NO ) );
}


/****************************************************************
 * "Format" Directive
 ****************************************************************/

static bool ProcELFDLL( void )
/****************************/
{
    FmtData.dll = true;
    return( true );
}

static parse_entry  ELFFormats[] = {
    "DLl",          ProcELFDLL,         MK_ELF, 0,
    NULL
};

bool ProcELFFormat( void )
/************************/
{
    ProcOne( ELFFormats, SEP_NO );
    return( true );
}

#endif
