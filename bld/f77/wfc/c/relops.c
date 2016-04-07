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
* Description:  relational operators
*
****************************************************************************/


#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "fcodes.h"
#include "global.h"
#include "optr.h"
#include "emitobj.h"
#include "tcmplx.h"
#include "upscan.h"
#include "relops.h"
#include "declare.h"


static  int     CharLength( itnode *op ) {
//========================================

    USOPN       opn;

    if( op->typ != FT_CHAR )
        return( 0 );
    opn = op->opn.us;
    if( opn & USOPN_SS1 )
        return( op->value.st.ss_size );
    if( ( opn & USOPN_WHERE ) == USOPN_SAFE )
        return( 0 );
    if( ( opn & USOPN_WHAT ) == USOPN_NNL )
        return( op->sym_ptr->u.ns.xt.size );
    if( ( opn & USOPN_WHAT ) == USOPN_CON )
        return( op->sym_ptr->u.lt.length );
    return( 0 );
}


void    GenChar1Op( itnode *op ) {
//================================

    if( ( ( op->opn.us & USOPN_WHAT ) == USOPN_CON ) ) {
        OutPtr( op->sym_ptr );
        SetOpn( op, USOPN_SAFE );
    } else {
        OutPtr( NULL );
    }
    op->opn.us &= ~USOPN_SS1;
}


void    RelOp( TYPE typ1, TYPE typ2, OPTR optr ) {
//================================================

// Generate code for a relational operator.

    bool        flip;
    bool        associative;
    bool        char_1_cmp;
    uint        i;
    uint        j;
    OPR         opr_code;
    FCODE       op_code;

    optr = optr;
    // must check for "flip" before we call "CharLength" since they may
    // call "PushOpn"
    flip = FALSE;
    if( ( ( CITNode->opn.us & USOPN_WHERE ) == USOPN_SAFE ) &&
        ( ( CITNode->link->opn.us & USOPN_WHERE ) != USOPN_SAFE ) ) {
        flip = TRUE;
    }
    // must do "CITNode->link" first to get operands in the right order
    i = CharLength( CITNode->link );
    j = CharLength( CITNode );
    opr_code = CITNode->link->opr;
    if( ( opr_code == OPR_EQ ) || ( opr_code == OPR_NE ) ) {
        char_1_cmp = OptimalChSize( i ) && OptimalChSize( j ) && ( i == j );
        associative = TRUE;
    } else {
        char_1_cmp = (i == 1 ) && ( j == 1 );
        associative = FALSE;
    }
    PushOpn( CITNode->link );
    PushOpn( CITNode );
    op_code = opr_code - OPR_FIRST_RELOP;
    if( TypeCmplx( typ1 ) && TypeCmplx( typ2 ) ) {
        op_code += FC_CC_RELOPS;
    } else if( TypeCmplx( typ1 ) ) {
        if( flip ) {
            op_code += FC_XC_RELOPS;
        } else {
            op_code += FC_CX_RELOPS;
        }
    } else if( TypeCmplx( typ2 ) ) {
        if( flip ) {
            op_code += FC_CX_RELOPS;
        } else {
            op_code += FC_XC_RELOPS;
        }
    } else {
        if( flip && !associative ) {
            EmitOp( FC_FLIP );
        }
        if( typ1 == FT_CHAR ) {
            if( char_1_cmp ) {
                op_code += FC_CHAR_1_RELOPS;
            } else {
                op_code += FC_CHAR_RELOPS;
            }
        } else {
            op_code += FC_RELOPS;
        }
    }
    EmitOp( op_code );
    if( char_1_cmp ) {
        if( associative ) {
            DumpType( MapTypes( FT_INTEGER, i ), i );
        } else {
            // Assert: comparing CHARACTER*1 with LT, LE, GT, or GE
            // Consider: CHARACTER A/'a'/
            //           IF( A .lt. CHAR(159) ) PRINT *, 'OK'
            // we must generate an unsigned comparison
            DumpType( MapTypes( FT_LOGICAL, i ), i );
        }
        if( flip && associative ) {
            GenChar1Op( CITNode->link );
            GenChar1Op( CITNode );
        } else {
            GenChar1Op( CITNode );
            GenChar1Op( CITNode->link );
        }
    } else if( typ1 != FT_CHAR ) {
        if( flip && associative ) {
            GenTypes( CITNode->link, CITNode );
        } else {
            GenTypes( CITNode, CITNode->link );
        }
    }
}
