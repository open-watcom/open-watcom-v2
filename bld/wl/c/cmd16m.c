/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command line parsing for the DOS/16M load file format.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "cmdutils.h"
#include "msg.h"
#include "exe16m.h"
#include "load16m.h"
#include "cmd16m.h"
#include "wlnkmsg.h"

#include "clibext.h"


#ifdef _DOS16M

void SetD16MFmt( void )
/*********************/
{
    Extension = E_PROTECT;
    LinkState &= ~LS_MAKE_RELOCS;           // assume none being produced.
    FmtData.u.d16m.options = 0;
    FmtData.u.d16m.flags = 0;
    FmtData.u.d16m.strategy = MNoStrategy;
    FmtData.u.d16m.buffer = 0;
    FmtData.u.d16m.gdtsize = 0xFFFF;        // 64 K - 1.
    FmtData.u.d16m.selstart = D16M_USER_SEL;
    FmtData.u.d16m.extended = 0x7FFF;
    FmtData.u.d16m.datasize = 0x1000;
    FmtData.u.d16m.exp_name = NULL;
    FmtData.u.d16m.stub = NULL;
}

void FreeD16MFmt( void )
/**********************/
{
    _LnkFree( FmtData.u.d16m.exp_name );
    _LnkFree( FmtData.u.d16m.stub );
}

void CmdD16MFini( void )
/**********************/
{
    const char          *base_name;
    size_t              base_name_len;

    if( FmtData.u.d16m.exp_name == NULL && Name != NULL ) {
        base_name = GetBaseName( Name, strlen( Name ), &base_name_len );
        FmtData.u.d16m.exp_name = FileName( base_name, base_name_len, E_PROTECT, true );
    }
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcBuffer( void )
/****************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( value < _1KB || value > _32KB ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "BUFFER" );
    } else {
        FmtData.u.d16m.buffer = value;
    }
    return( true );
}

static bool ProcGDTSize( void )
/*****************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( (value % 8) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "GDTSIZE" );
        value &= -8;
    }
    if( value > 0x10000 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "GDTSIZE" );
    } else {
        FmtData.u.d16m.gdtsize = --value;
    }
    return( true );
}

static bool ProcRelocs( void )
/****************************/
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    LinkState |= LS_MAKE_RELOCS;
    return( true );
}

static bool ProcSelStart( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( (value % 8) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "SELSTART" );
        value &= -8;
    }
    if( value >= 0x10000 || value < D16M_USER_SEL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "SELSTART" );
    } else {
        FmtData.u.d16m.selstart = value;
    }
    return( true );
}

static bool ProcExtended( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    value >>= 10;      // value should be in K.
    if( value >= 0x10000 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "EXTENDED" );
    } else {
        FmtData.u.d16m.extended = value;
    }
    return( true );
}

static bool ProcExpName( void )
/*****************************/
{
    if( !HaveEquals( TOK_INCLUDE_DOT | TOK_IS_FILENAME ) )
        return( false );
    if( FmtData.u.d16m.exp_name != NULL ) {
        _LnkFree( FmtData.u.d16m.exp_name );
    }
    FmtData.u.d16m.exp_name = FileName( Token.this, Token.len, E_PROTECT, true );     // just keep the name around for now.
    strupr( FmtData.u.d16m.exp_name );
    return( true );
}

static bool ProcDataSize( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( value > _64KB ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "DATASIZE" );
    } else {
        FmtData.u.d16m.datasize = (value + 15) >> 4;
        FmtData.u.d16m.flags |= DATASIZE_SPECD;
    }
    return( true );
}

static parse_entry  MainOptions[] = {
    "BUFfer",       ProcBuffer,         MK_DOS16M, 0,
    "GDTsize",      ProcGDTSize,        MK_DOS16M, 0,
    "RELocs",       ProcRelocs,         MK_DOS16M, 0,
    "SELstart",     ProcSelStart,       MK_DOS16M, 0,
    "DATASize",     ProcDataSize,       MK_DOS16M, 0,
    "EXTended",     ProcExtended,       MK_DOS16M, 0,
    "EXPName",      ProcExpName,        MK_DOS16M, 0,
    NULL
};

bool Proc16MOptions( void )
/*************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}

bool Proc16MNoRelocs( void )
/**************************/
{
    if( LinkState & LS_MAKE_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    } else {
        FmtData.u.d16m.flags |= FORCE_NO_RELOCS;
    }
    return( true );
}


/****************************************************************
 * "RUntime" Directive
 ****************************************************************/

static bool ProcKeyboard( void )
/******************************/
{
    FmtData.u.d16m.options |= OPT_KEYBOARD;
    return( true );
}

static bool ProcOverload( void )
/******************************/
{
    FmtData.u.d16m.options |= OPT_OVERLOAD;
    return( true );
}

static bool ProcInt10( void )
/***************************/
{
    FmtData.u.d16m.options |= OPT_INT10;
    return( true );
}

static bool ProcInit00( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_INIT00;
    return( true );
}

static bool ProcInitFF( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_INITFF;
    return( true );
}

static bool ProcRotate( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_ROTATE;
    return( true );
}

static bool ProcSelectors( void )
/********************************
 * force selectors to be assigned at load time.
 */
{
    FmtData.u.d16m.options |= OPT_AUTO;
    return( true );
}

static bool ProcAuto( void )
/***************************
 * force selectors to be assigned at load time, and force relocs as well.
 */
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    FmtData.u.d16m.options |= OPT_AUTO;
    LinkState |= LS_MAKE_RELOCS;
    return( true );
}

static parse_entry  RunOptions[] = {
    "KEYboard",     ProcKeyboard,       MK_DOS16M, 0,
    "OVERload",     ProcOverload,       MK_DOS16M, 0,
    "INIT00",       ProcInit00,         MK_DOS16M, 0,
    "INITFF",       ProcInitFF,         MK_DOS16M, 0,
    "ROTate",       ProcRotate,         MK_DOS16M, 0,
    "AUTO",         ProcAuto,           MK_DOS16M, 0,
    "SELectors",    ProcSelectors,      MK_DOS16M, 0,
    "INT10",        ProcInt10,          MK_DOS16M, 0,
    NULL
};

bool Proc16MRuntime( void )
/*************************/
{
    return( ProcOne( RunOptions, SEP_NO ) );
}


/****************************************************************
 * "MEmory" Directive
 ****************************************************************/

static bool ProcTryExtended( void )
/*********************************/
{
    FmtData.u.d16m.strategy = MPreferExt;
    return( true );
}

static bool ProcTryLow( void )
/****************************/
{
    FmtData.u.d16m.strategy = MPreferLow;
    return( true );
}

static bool ProcForceExtended( void )
/***********************************/
{
    FmtData.u.d16m.strategy = MForceExt;
    return( true );
}

static bool ProcForceLow( void )
/******************************/
{
    FmtData.u.d16m.strategy = MForceLow;
    return( true );
}

static parse_entry  Strategies[] = {
    "TRYExtended",  ProcTryExtended,    MK_DOS16M, 0,
    "TRYLow",       ProcTryLow,         MK_DOS16M, 0,
    "FORCEExtended",ProcForceExtended,  MK_DOS16M, 0,
    "FORCELow",     ProcForceLow,       MK_DOS16M, 0,
    NULL
};

bool Proc16MMemory( void )
/************************/
{
    return( ProcOne( Strategies, SEP_NO ) );
}


/****************************************************************
 * "TRansparent" Directive
 ****************************************************************/

static bool ProcTStack( void )
/****************************/
{
    FmtData.u.d16m.flags |= TRANS_STACK;
    return( true );
}

static bool ProcTData( void )
/***************************/
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_TRANS_RELOCS_NEEDED, NULL );
    }
    FmtData.u.d16m.flags |= TRANS_DATA;
    LinkState |= LS_MAKE_RELOCS;
    return( true );
}

static parse_entry  TransTypes[] = {
    "STack",        ProcTStack,         MK_DOS16M, 0,
    "DAta",         ProcTData,          MK_DOS16M, 0,
    NULL
};

bool Proc16MTransparent( void )
/*****************************/
{
    if( FmtData.u.d16m.flags & TRANS_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_OPTION_MULTIPLY_DEFD, "s", "transparent" );
        return( true );
    } else {
        return( ProcOne( TransTypes, SEP_NO ) );
    }
}


/****************************************************************
 * "FORMat" Directive
 ****************************************************************/

bool Proc16MFormat( void )
/************************/
{
    return( true );
}

#endif
