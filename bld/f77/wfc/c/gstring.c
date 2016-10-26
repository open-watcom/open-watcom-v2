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
// GSTRING   : string code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "opn.h"
#include "optr.h"
#include "emitobj.h"
#include "utility.h"
#include "upscan.h"
#include "cgmagic.h"
#include "gstring.h"
#include "relops.h"
#include "upcat.h"
#include "declare.h"


sym_id  GStartCat( uint num_args, uint size ) {
//===========================================

// Start cconcatenation into a temporary.

    num_args = num_args; size = size;
    return( NULL );
}


sym_id  GTempString( uint size ) {
//===============================

// Generate a static temporary string.

    sym_id     sym_ptr;

    sym_ptr = StaticAlloc( sizeof( string ), FT_CHAR );
    sym_ptr->u.ns.xt.size = size;
    return( sym_ptr );
}


void    GStopCat( uint num_args, sym_id result ) {
//===============================================

// Finish concatenation into a temporary.

    result = result;
    CITNode->sym_ptr = GTempString( CITNode->size );
    CITNode->opn.us = USOPN_VAL;
    // Push the address of a static SCB so that we can modify its
    // length to correspond to the length concatenated so that
    //      CHARACTER*5 WORD
    //      PRINT,LEN('1'//WORD(1:3))
    // prints 4 and not 6. The static SCB in this case initially
    // contains the length 6 ( len('1') + len(word) ) since generally
    // we don't know the length concatenated at compile time if WORD
    // was indexed as WORD(I:J).
    PushOpn( CITNode );
    EmitOp( FC_CAT );
    OutU16( (uint_16)( num_args | CAT_TEMP ) ); // indicate concatenating into a static temp
}


void    GCatArg( itnode *itptr ) {
//================================

// Emit a character string to be concatenated.

    itptr = itptr;
}


static  uint    SrcChar( itnode *op ) {
//=====================================

    if( op->opn.us & USOPN_SS1 )
        return( op->value.st.ss_size );
    if( ( op->opn.us & USOPN_WHAT ) == USOPN_CON ) { // character constant
        return( op->sym_ptr->u.lt.length );
    }
    if( ( op->opn.us & USOPN_WHAT ) == USOPN_NNL ) { // character variable
        if( op->sym_ptr->u.ns.u1.s.typ == FT_STRUCTURE ) {
            return( 0 );        // No mechanism exists for keeping the size.
        } else {
            return( op->sym_ptr->u.ns.xt.size );
        }
    }
    return( 0 );
}


static  uint    TargChar( itnode *op ) {
//======================================

    if( op->opn.us & USOPN_SS1 ) return( op->value.st.ss_size );
    if( ( op->opn.us & USOPN_WHAT ) == USOPN_NNL ) { // character variable
        if( op->sym_ptr->u.ns.u1.s.typ == FT_STRUCTURE ) {
            return( 0 );        // No mechanism exists for keeping the size.
        } else {
            return( op->sym_ptr->u.ns.xt.size );
        }
    }
    if( ( op->opn.us & USOPN_WHAT ) == USOPN_NWL ) { // character array
        if( op->sym_ptr->u.ns.u1.s.typ == FT_STRUCTURE ) {
            return( 0 );        // No mechanism exists for keeping the size.
        } else {
            return( op->sym_ptr->u.ns.xt.size );
        }
    }
    return( 0 );
}


void    AsgnChar( void ) {
//==================

// Perform character assignment.

    itnode      *save_cit;
    uint        num_args;
    uint        i;
    uint        j;

    save_cit = CITNode;
    AdvanceITPtr();
    num_args = AsgnCat();
    i = SrcChar( CITNode );
    j = TargChar( save_cit );
    if( ( num_args == 1 ) && ( i > 0 ) && ( j > 0 ) ) {
        if( OptimalChSize( i ) && OptimalChSize( j ) && ( i == j ) ) {
            PushOpn( save_cit );
            EmitOp( FC_CHAR_1_MOVE );
            DumpType( MapTypes( FT_INTEGER, i ), i );
            GenChar1Op( CITNode );
            if( ( CITNode->opn.us & USOPN_WHAT ) == USOPN_CON ) {
                CITNode->sym_ptr->u.lt.flags &= ~LT_SCB_TMP_REFERENCE;
            }
            CITNode = save_cit;
        } else {
#if ( _CPU == 386 || _CPU == 8086 )
            if( j < i ) {
                i = j;
            }
            CITNode = save_cit;
            PushOpn( CITNode );
            EmitOp( FC_CHAR_N_MOVE );
            OutInt( i );
            OutInt( j );
#else
            CatArgs( num_args );
            CITNode = save_cit;
            PushOpn( CITNode );
            EmitOp( FC_CAT );
            OutU16( (uint_16)num_args );
#endif
        }
    } else {
        CatArgs( num_args );
        CITNode = save_cit;
        PushOpn( CITNode );
        EmitOp( FC_CAT );
        OutU16( (uint_16)num_args );
    }
}
