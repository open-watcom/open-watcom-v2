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


//
// GALLOC :     Generate call to dynamically allocate/deallocate array
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "falloc.h"
#include "emitobj.h"

extern  void            DimArray(sym_id);
extern  void            LoadSCB(sym_id);

static  unsigned_16     allocFlags;
static  obj_ptr         flagsLabel;


void    GBegAllocate( void ) {
//======================

    allocFlags = ALLOC_NONE;
    EmitOp( FC_ALLOCATE );
}


void    GAllocate( sym_id sym ) {
//===============================

    OutPtr( sym );
    DimArray( sym );
    EmitOp( FC_END_OF_SEQUENCE );
}


void    GAllocateString( sym_id sym ) {
//=====================================

    OutPtr( sym );
    LoadSCB( sym );
    EmitOp( FC_END_OF_SEQUENCE );
}


void    GAllocEOL( void ) {
//=========================

    OutPtr( NULL );
    flagsLabel = ObjTell();
    OutU16( 0 );
}


void    GEndAllocate( void ) {
//============================

    // Support for multiple settings for allocate
    obj_ptr     cur_offset;

    cur_offset = ObjSeek( flagsLabel );
    OutU16( allocFlags );
    ObjSeek( cur_offset );
}


void    GBegDeAllocate( void ) {
//========================

    allocFlags = ALLOC_NONE;
    EmitOp( FC_DEALLOCATE );
}


void    GDeAllocate( sym_id sym ) {
//=================================

    OutPtr( sym );
}


void    GDeAllocateString( sym_id sym ) {
//=======================================

    OutPtr( sym );
}


void    GEndDeAllocate( void ) {
//==============================

    GEndAllocate();
}


static void GAllocOpts( void ) {
//==============================

    PushOpn( CITNode );
    EmitOp( FC_END_OF_SEQUENCE );
}


void    GAllocLoc( void ) {
//===================

    allocFlags &= ~ALLOC_NONE;
    allocFlags |= ALLOC_LOC;
    GAllocOpts();
}


void    GAllocStat( void ) {
//====================

    allocFlags &= ~ALLOC_NONE;
    allocFlags |= ALLOC_STAT;
    GAllocOpts();
}


void    GAllocated( void ) {
//====================

// Generate code for ALLOCATED intrinsic function.

    PushOpn( CITNode );
    EmitOp( FC_ALLOCATED );
    if( CITNode->sym_ptr->ns.flags & SY_SUBSCRIPTED ) {
        OutU16( 0 );
    } else {
        OutU16( ALLOC_STRING );
    }
}


void            GSCBLength( sym_id sym ) {
//========================================

// Generate code to fill in SCB Length and set ptr to NULL

    // push string length
    PushOpn( CITNode );
    EmitOp( FC_SET_SCB_LEN );
    // general information
    OutPtr( sym );
    GenType( CITNode );
}
