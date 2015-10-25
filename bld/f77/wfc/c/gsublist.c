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
// GSUBLIST  : array code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "emitobj.h"
#include "upscan.h"
#include "gsublist.h"

static  obj_ptr         WarpLabel;


void            GWarp( sym_id sym ) {
//===================================

// Generate warp to code to fill in ADV.

    EmitOp( FC_WARP );
    OutPtr( sym );
}


warp_label              GBegSList( void ) {
//===================================

// Generate code to start ADV initialization.

    EmitOp( FC_FCODE_SEEK );
    WarpLabel = ObjTell();
    OutU16( 0 );
    return( ObjTell() );
}


void            GSLoBound( int dim_no, sym_id sym ) {
//===================================================

// Generate code to fill in ADV subscript element (lo bound).

    PushOpn( CITNode );
    EmitOp( FC_ADV_FILL_LO );
    OutPtr( sym );
    OutU16( dim_no );
    GenType( CITNode );
}


void            GForceHiBound( int dim_no, sym_id sym ) {
//=======================================================

// Generate code to fill in ADV subscript element (hi bound).
// The hi bound is constant and the low bound is not.
// We have to force the filling in of the high bound so that the number of
// of elements gets computed.
//
// Scenario:   SUBROUTINE SAM( A, J )
//             DIMENSION A(J:3)
//
// GInitADV() fills in the lo bound and # of elements in the dimension at
// compile-time. The lo bound is unknown so the ADV does not contain the
// correct information. The lo bound gets filled in at run-time but
// since the hi bound is not dumped into the ADV at compile time we
// must fill it in at run-time and compute the correct number of elements
// in the dimension.

    AddConst( CITNode );
    PushOpn( CITNode );
    EmitOp( FC_ADV_FILL_HI );
    OutPtr( sym );
    OutU16( dim_no );
    GenType( CITNode );
}


void            GSHiBound( int dim_no, sym_id sym ) {
//===================================================

// Generate code to fill in ADV subscript element (hi bound).

    PushOpn( CITNode );
    EmitOp( FC_ADV_FILL_HI );
    OutPtr( sym );
    OutU16( dim_no );
    GenType( CITNode );
}


void            GSHiBoundLo1( int dim_no, sym_id sym ) {
//======================================================

// Generate code to fill in ADV subscript element (hi bound).
// Set the low bound to 1.

    // push high bound value
    PushOpn( CITNode );
    EmitOp( FC_ADV_FILL_HI_LO_1 );
    // general information
    OutPtr( sym );
    OutU16( dim_no );
    GenType( CITNode );
}


void            GEndSList( sym_id sym ) {
//=======================================

// Finish off ADV initialization.

    unsigned_16 warp_size;

    sym = sym;
    EmitOp( FC_WARP_RETURN );
    warp_size = ObjOffset( WarpLabel ) - sizeof( unsigned_16 );
    WarpLabel = ObjSeek( WarpLabel );
    OutU16( warp_size );
    ObjSeek( WarpLabel );
}


void            FreeWarpLabel( warp_label warp ) {
//================================================

    warp = warp;
}
