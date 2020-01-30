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
#include "cmdutils.h"
#include "exephar.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "cmdphar.h"
#include "cmdall.h"


#ifdef _PHARLAP

void SetPharFmt( void )
/*********************/
{
    Extension = E_PROTECT;
    LinkState &= ~LS_MAKE_RELOCS;   // do not generate relocations.
    FmtData.u.phar.mindata = 0;
    FmtData.u.phar.maxdata = 0xFFFFFFFF;
    FmtData.u.phar.breaksym = NULL;
    FmtData.u.phar.stub = NULL;
    FmtData.u.phar.minreal = 0;
    FmtData.u.phar.maxreal = 0;
    FmtData.u.phar.callbufs = 0;
    FmtData.u.phar.realbreak = 0;
    FmtData.u.phar.minibuf = 1;
    FmtData.u.phar.maxibuf = 4;
    FmtData.u.phar.nistack = 4;
    FmtData.u.phar.istksize = 1;
    FmtData.u.phar.extender_flags = 0;
    FmtData.u.phar.unpriv = false;
//    FmtData.u.phar.pack = false;
    ChkBase( _4KB );
}

void FreePharFmt( void )
/**********************/
{
    _LnkFree( FmtData.u.phar.breaksym );
    _LnkFree( FmtData.u.phar.stub );
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

#if 0
 .exp packing executables implemented yet.

static bool ProcPackExp( void )
/*****************************/
{
    FmtData.u.phar.pack = true;
    return( true );
}
#endif

static bool ProcMinData( void )
/*****************************/
{
    return( GetLong( &FmtData.u.phar.mindata ) );
}

static bool ProcMaxData( void )
/*****************************/
{
    return( GetLong( &FmtData.u.phar.maxdata ) );
}

static parse_entry  MainOptions[] = {
//    "PACKExp",      ProcPackExp,        MK_PHAR_FLAT, 0,
    "MINData",      ProcMinData,        MK_PHAR_LAP, 0,
    "MAXData",      ProcMaxData,        MK_PHAR_LAP, 0,
    NULL
};

bool ProcPharOptions( void )
/**************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}


/****************************************************************
 * "RUntime" Directive
 ****************************************************************/

static bool ProcUnpriv( void )
/****************************/
{
    FmtData.u.phar.unpriv = true;
    return( true );
}

static bool ProcPriv( void )
/**************************/
{
    return( true );
}

static bool ProcFlags( void )
/***************************/
{
    bool            ret;
    unsigned_32     value;

    ret = GetLong( &value );
    FmtData.u.phar.extender_flags = value;
    return( ret );
}

static bool ProcMinReal( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= FmtData.SegShift;       // value specified in paragraphs
        if( value > 0xffff ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "MINREAL" );
        } else {
            FmtData.u.phar.minreal = value;
        }
    }
    return( ret );
}

static bool ProcMaxReal( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= FmtData.SegShift;       // value specified in paragraphs
        if( value > 0xffff ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "MAXREAL" );
        } else {
            FmtData.u.phar.maxreal = value;
        }
    }
    return( ret );
}

static bool ProcRealBreak( void )
/*******************************/
{
    unsigned_32     value;
    ord_state       ok;

    if( !HaveEquals( TOK_INCLUDE_DOT ) )
        return( false );
    ok = getatol( &value );
    if( FmtData.u.phar.breaksym != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_REALBREAKS, NULL );
        _LnkFree( FmtData.u.phar.breaksym );
        FmtData.u.phar.breaksym = NULL;
    }
    if( ok != ST_IS_ORDINAL ) {   // must be a symbol name.
        FmtData.u.phar.breaksym = tostring();
    } else {
        FmtData.u.phar.realbreak = value;
    }
    return( true );
}

static bool ProcCallBufs( void )
/******************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "CALLBUFS" );
        } else {
            FmtData.u.phar.callbufs = value;
        }
    }
    return( ret );
}

static bool ProcMiniBuf( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "MINIBUF" );
        } else {
            FmtData.u.phar.minibuf = value;
        }
    }
    return( ret );
}

static bool ProcMaxiBuf( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "MAXIBUF" );
        } else {
            FmtData.u.phar.maxibuf = value;
        }
    }
    return( ret );
}

static bool ProcNIStack( void )
/*****************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        if( value < 4 || value > 0xFFFF ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "NISTACK" );
        } else {
            FmtData.u.phar.nistack = value;
        }
    }
    return( ret );
}

static bool ProcIStkSize( void )
/******************************/
{
    unsigned_32 value;
    bool        ret;

    ret = GetLong( &value );
    if( ret ) {
        value >>= 10;       // value specified in kilobytes
        if( value > 64 || value < 1 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ISTKSIZE" );
        } else {
            FmtData.u.phar.istksize = value;
        }
    }
    return( ret );
}

static parse_entry  RunOptions[] = {
    "MINReal",      ProcMinReal,        MK_PHAR_FLAT, 0,
    "MAXReal",      ProcMaxReal,        MK_PHAR_FLAT, 0,
    "REALBreak",    ProcRealBreak,      MK_PHAR_FLAT, CF_HAVE_REALBREAK,
    "CALLBufs",     ProcCallBufs,       MK_PHAR_FLAT, 0,
    "MINIBuf",      ProcMiniBuf,        MK_PHAR_FLAT, 0,
    "MAXIBuf",      ProcMaxiBuf,        MK_PHAR_FLAT, 0,
    "NISTack",      ProcNIStack,        MK_PHAR_FLAT, 0,
    "ISTKsize",     ProcIStkSize,       MK_PHAR_FLAT, 0,
    "UNPRIVileged", ProcUnpriv,         MK_PHAR_FLAT, 0,
    "PRIVileged",   ProcPriv,           MK_PHAR_FLAT, 0,
    /* WARNING: do not document the following directive -- for internal use only */
    "FLAGs",        ProcFlags,          MK_PHAR_FLAT, 0,
    NULL
};

bool ProcPharRuntime( void )
/**************************/
{
    return( ProcOne( RunOptions, SEP_NO ) );
}


/****************************************************************
 * "Format" Directive
 ****************************************************************/

static bool ProcPharFlat( void )
/******************************/
{
    return( true );
}

static bool ProcRex( void )
/*************************/
{
    Extension = E_REX;
    LinkState |= LS_MAKE_RELOCS;    // make relocations;
    return( true );
}

static bool ProcPharSegmented( void )
/***********************************/
{
    LinkState |= LS_MAKE_RELOCS;    // make relocations;
    return( true );
}

static parse_entry  PharLapFormats[] = {
    "EXTended",     ProcPharFlat,       MK_PHAR_FLAT,     0,
    "REX",          ProcRex,            MK_PHAR_REX,      0,
    "SEGmented",    ProcPharSegmented,  MK_PHAR_MULTISEG, 0,
    NULL
};

bool ProcPharFormat( void )
/*************************/
{
    ProcOne( PharLapFormats, SEP_NO );
    return( true );
}

#endif
