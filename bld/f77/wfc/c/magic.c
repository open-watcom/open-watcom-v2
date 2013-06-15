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
* Description:  temporary, static temporary manager
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fmemmgr.h"

#define TEMP_FLAGS      (SY_TYPE | SY_USAGE | SY_VARIABLE);


void    STTmpFree() {
//===================

// Free all temporaries after executing a statement.

    TempIndex = TempBase;
}


sym_id  NewMagSym( int class ) {
//==============================

// Allocate a "magic" symbol.

    sym_id      new;

    new = FMemAlloc( sizeof( symbol ) - STD_SYMLEN );
    new->ns.link = MList;
    MList = new;
    new->ns.reloc_chain.lr = NULL;
    _MgcSetClass( new, class );
    new->ns.flags = TEMP_FLAGS;
    new->ns.u1.s.xflags = 0;
    new->ns.xt.size = 0;
    new->ns.u1.s.typ = FT_NO_TYPE;
    new->ns.si.ms.sym = NULL;
    return( new );
}


sym_id  FindTempIndex( uint tmp_index, TYPE typ ) {
//=================================================

// Find a temporary with an offset the same as the given index.

    sym_id      ml;

    typ = typ;
    ml = MList;
    for(;;) {
        if( ml == NULL ) break;
        if(( _MgcClass( ml ) == MAGIC_TEMP ) &&
            ( ml->ns.si.ms.tmp_info.tmp_index == tmp_index ))
            return( ml );
        ml = ml->ns.link;
    }
    ml = NewMagSym( MAGIC_TEMP );
    ml->ns.si.ms.tmp_info.tmp_index = tmp_index;
    return( ml );
}


sym_id  TmpVar( TYPE typ, uint size ) {
//===================================

// Allocate a temporary and set the type.

    sym_id      ml;

    ml = FindTempIndex( TempIndex, typ );
    ml->ns.xt.size = size;
    ml->ns.u1.s.typ = typ;
    TempIndex += size;
    if( TempIndex > MaxTempIndex ) {
        MaxTempIndex = TempIndex;
    }
    return( ml );
}


sym_id  TmpAlloc( uint size ) {
//============================

// Allocate a temporary and set the type to FT_NO_TYPE.

    return( TmpVar( FT_NO_TYPE, size ) );
}


void    GSaveTemps() {
//====================

// Save temporaries before statement function.

    TempBase = MaxTempIndex;
    TempIndex = MaxTempIndex;
}


sym_id  StaticAlloc( uint size, TYPE typ ) {
//=========================================

// Allocate a static compiler variable.

    sym_id      ml;

    ml = NewMagSym( MAGIC_STATIC );
    ml->ns.si.ms.tmp_info.stat_off = NULL;
    ml->ns.xt.size = size;
    ml->ns.u1.s.typ = typ;
    return( ml );
}


#if 0

sym_id  StaticOffset( sym_id stat, int offset ) {
//===============================================

// Return an "mste" that points into an already allocated static.

    sym_id      ml;

    ml = FMemAlloc( sizeof( symbol ) - STD_SYMLEN );
    ml->ns.reloc_chain.lr = NULL;
    ml->ns.xt.size = offset;
    ml->ns.flags = TEMP_FLAGS;
    ml->ns.u1.s.typ = FT_NO_TYPE;
    ml->ns.link = stat->ns.si.ms.tmp_info.stat_off;
    stat->ns.si.ms.tmp_info.stat_off = ml;
    return( ml );
}

#endif


label_id        NextLabel() {
//===========================

// Create a new label.

    return( NewMagSym( MAGIC_LABEL ) );
}
