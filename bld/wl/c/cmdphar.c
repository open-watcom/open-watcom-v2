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
 *  CMDPHAR : command line parsing for phar lap file formats.
 *
*/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "exephar.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "cmdphar.h"
#include "cmdos2.h"


extern bool ProcPharLap( void )
/*****************************/
{
    ProcOne( PharModels, SEP_NO, FALSE );
    return( TRUE );
}

extern void SetPharFmt( void )
/****************************/
{
    Extension = E_PROTECT;
    LinkState &= ~MAKE_RELOCS;     // do not generate relocations.
    _ChkAlloc( FmtData.u.phar.params, sizeof( *FmtData.u.phar.params ) );
    FmtData.u.phar.mindata = 0;
    FmtData.u.phar.maxdata = 0xFFFFFFFF;
    FmtData.u.phar.breaksym = NULL;
    FmtData.u.phar.stub = NULL;
//  FmtData.u.phar.pack = FALSE;
    FmtData.u.phar.params->minreal = 0;
    FmtData.u.phar.params->maxreal = 0;
    FmtData.u.phar.params->callbufs = 0;
    FmtData.u.phar.params->realbreak = 0;
    FmtData.u.phar.params->minibuf = 1;
    FmtData.u.phar.params->maxibuf = 4;
    FmtData.u.phar.params->nistack = 4;
    FmtData.u.phar.params->istksize = 1;
    FmtData.u.phar.params->extender_flags = 0;
    FmtData.u.phar.params->unpriv = 0;
    ChkBase(4*1024);
}

extern void FreePharFmt( void )
/*****************************/
{
    _LnkFree( FmtData.u.phar.breaksym );
    _LnkFree( FmtData.u.phar.params );
    _LnkFree( FmtData.u.phar.stub );
}

extern bool ProcPharFlat( void )
/******************************/
{
    return( TRUE );
}

extern bool ProcRex( void )
/*************************/
{
    Extension = E_REX;
    LinkState |= MAKE_RELOCS;       // make relocations;
    return( TRUE );
}

extern bool ProcPharSegmented( void )
/***********************************/
{
    LinkState |= MAKE_RELOCS;       // make relocations;
    return TRUE;
}

#if 0
 .exp packing executables implemented yet.

extern bool ProcPackExp( void )
/*****************************/
{
    FmtData.u.phar.pack = TRUE;
    return( TRUE );
}
#endif

extern bool ProcMinData( void )
/*****************************/
{
    return( GetLong( &FmtData.u.phar.mindata ) );
}

extern bool ProcMaxData( void )
/*****************************/
{
    return( GetLong( &FmtData.u.phar.maxdata ) );
}

extern bool ProcUnpriv( void )
/****************************/
{
    FmtData.u.phar.params->unpriv = 1;
    return( TRUE );
}

extern bool ProcPriv( void )
/**************************/
{
    return( TRUE );
}

extern bool ProcFlags( void )
/****************************/
{
    bool    ret;
    unsigned long   num;

    ret = GetLong( &num );
    FmtData.u.phar.params->extender_flags = num;
    return( ret );
}

extern bool ProcMinReal( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 4;       // value specified in paragraphs
        if( value > 0xffff ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "minreal" );
        } else {
            FmtData.u.phar.params->minreal = value;
        }
    }
    return( ret );
}

extern bool ProcMaxReal( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 4;       // value specified in paragraphs
        if( value > 0xffff ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "maxreal" );
        } else {
            FmtData.u.phar.params->maxreal = value;
        }
    }
    return( ret );
}

extern bool ProcRealBreak( void )
/*******************************/
{
    unsigned_32     value;
    ord_state       ok;

    if( !HaveEquals( TOK_INCLUDE_DOT ) ) return( FALSE );
    ok = getatol( &value );
    if( FmtData.u.phar.breaksym != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_REALBREAKS, NULL );
        _LnkFree( FmtData.u.phar.breaksym );
        FmtData.u.phar.breaksym = NULL;
    }
    if( ok != ST_IS_ORDINAL ) {   // must be a symbol name.
        FmtData.u.phar.breaksym = tostring();
    } else {
        FmtData.u.phar.params->realbreak = value;
    }
    return( TRUE );
}

extern bool ProcCallBufs( void )
/******************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "callbufs" );
        } else {
            FmtData.u.phar.params->callbufs = value;
        }
    }
    return( ret );
}

extern bool ProcMiniBuf( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "minibuf" );
        } else {
            FmtData.u.phar.params->minibuf = value;
        }
    }
    return( ret );
}

extern bool ProcMaxiBuf( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "maxibuf" );
        } else {
            FmtData.u.phar.params->maxibuf = value;
        }
    }
    return( ret );
}

extern bool ProcNIStack( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        if( value < 4 || value > 0xFFFF ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "nistack" );
        } else {
            FmtData.u.phar.params->nistack = value;
        }
    }
    return( ret );
}

extern bool ProcIStkSize( void )
/******************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "istksize" );
        } else {
            FmtData.u.phar.params->istksize = value;
        }
    }
    return( ret );
}
