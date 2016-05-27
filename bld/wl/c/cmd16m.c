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
* Description:  Command line parsing for the DOS/16M load file format.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "msg.h"
#include "exe16m.h"
#include "load16m.h"
#include "cmd16m.h"
#include "wlnkmsg.h"


extern bool ProcMemory16M( void )
/*******************************/
{
    return( ProcOne( Strategies, SEP_NO, false ) );
}

extern bool ProcTryExtended( void )
/*********************************/
{
    FmtData.u.d16m.strategy = MPreferExt;
    return( true );
}

extern bool ProcTryLow( void )
/****************************/
{
    FmtData.u.d16m.strategy = MPreferLow;
    return( true );
}

extern bool ProcForceExtended( void )
/***********************************/
{
    FmtData.u.d16m.strategy = MForceExt;
    return( true );
}

extern bool ProcForceLow( void )
/******************************/
{
    FmtData.u.d16m.strategy = MForceLow;
    return( true );
}

extern bool ProcTransparent( void )
/*********************************/
{
    if( FmtData.u.d16m.flags & TRANS_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_OPTION_MULTIPLY_DEFD, "s", "transparent" );
        return( true );
    } else {
        return( ProcOne( TransTypes, SEP_NO, false ) );
    }
}

extern bool ProcTStack( void )
/****************************/
{
    FmtData.u.d16m.flags |= TRANS_STACK;
    return( true );
}

extern bool ProcTData( void )
/***************************/
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_TRANS_RELOCS_NEEDED, NULL );
    }
    FmtData.u.d16m.flags |= TRANS_DATA;
    LinkState |= MAKE_RELOCS;
    return( true );
}

extern bool ProcKeyboard( void )
/******************************/
{
    FmtData.u.d16m.options |= OPT_KEYBOARD;
    return( true );
}

extern bool ProcOverload( void )
/******************************/
{
    FmtData.u.d16m.options |= OPT_OVERLOAD;
    return( true );
}

extern bool ProcInt10( void )
/***************************/
{
    FmtData.u.d16m.options |= OPT_INT10;
    return( true );
}

extern bool ProcInit00( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_INIT00;
    return( true );
}

extern bool ProcInitFF( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_INITFF;
    return( true );
}

extern bool ProcRotate( void )
/****************************/
{
    FmtData.u.d16m.options |= OPT_ROTATE;
    return( true );
}

extern bool ProcSelectors( void )
/*******************************/
// force selectors to be assigned at load time.
{
    FmtData.u.d16m.options |= OPT_AUTO;
    return( true );
}

extern bool ProcAuto( void )
/**************************/
// force selectors to be assigned at load time, and force relocs as well.
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    FmtData.u.d16m.options |= OPT_AUTO;
    LinkState |= MAKE_RELOCS;
    return( true );
}

extern bool ProcBuffer( void )
/****************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( value < 1024 || value > 32768 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "buffer" );
    } else {
        FmtData.u.d16m.buffer = value;
    }
    return( true );
}

extern bool ProcGDTSize( void )
/*****************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( (value % 8) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "gdtsize" );
        value &= -8;
    }
    if( value > 65536 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "gdtsize" );
    } else {
        FmtData.u.d16m.gdtsize = --value;
    }
    return( true );
}

extern bool ProcRelocs( void )
/****************************/
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    LinkState |= MAKE_RELOCS;
    return( true );
}

extern bool Proc16MNoRelocs( void )
/*********************************/
{
    if( LinkState & MAKE_RELOCS ) {
        LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    } else {
        FmtData.u.d16m.flags |= FORCE_NO_RELOCS;
    }
    return( true );
}

extern bool ProcSelStart( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( (value % 8) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "selstart" );
        value &= -8;
    }
    if( value > 65536 || value < D16M_USER_SEL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "selstart" );
    } else {
        FmtData.u.d16m.selstart = value;
    }
    return( true );
}

extern bool ProcExtended( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    value >>= 10;      // value should be in K.
    if( value > 65535 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "extended" );
    } else {
        FmtData.u.d16m.extended = value;
    }
    return( true );
}

extern bool ProcDataSize( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    if( value > 65536 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "datasize" );
    } else {
        FmtData.u.d16m.datasize = (value + 15) >> 4;
        FmtData.u.d16m.flags |= DATASIZE_SPECD;
    }
    return( true );
}

extern void SetD16MFmt( void )
/****************************/
{
    LinkState &= ~MAKE_RELOCS;              // assume none being produced.
    Extension = E_PROTECT;
    FmtData.u.d16m.options = 0;
    FmtData.u.d16m.flags = 0;
    FmtData.u.d16m.strategy = MNoStrategy;
    FmtData.u.d16m.buffer = 0;
    FmtData.u.d16m.gdtsize = 0xFFFF;        // 64 K - 1.
    FmtData.u.d16m.selstart = D16M_USER_SEL;
    FmtData.u.d16m.extended = 0x7FFF;
    FmtData.u.d16m.datasize = 0x1000;
    FmtData.u.d16m.stub = NULL;
}

extern void FreeD16MFmt( void )
/*****************************/
{
    _LnkFree( FmtData.u.d16m.stub );
}

extern bool Proc16M( void )
/*************************/
{
    return( true );
}
