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
* Description:  Generate F-Code for binary operations
*
****************************************************************************/


#include "ftnstd.h"
#include "optr.h"
#include "opn.h"
#include "fcodes.h"
#include "global.h"
#include "types.h"
#include "emitobj.h"
#include "types.h"

extern  bool            TypeCmplx(TYPE);

/* Forward declarations */
void    BinOp( TYPE typ1, TYPE typ2, OPTR opr );


static bool UnaryMul( TYPE typ1, TYPE typ2 ) {
//============================================

    if( typ1 > FT_EXTENDED )
        return( FALSE );
    if( !_IsTypeInteger( typ2 ) )
        return( FALSE );
    if( CITNode->link->opn.us != USOPN_CON )
        return( FALSE );
    if( ITIntValue( CITNode->link ) < 0 )
        return( FALSE );
    if( ITIntValue( CITNode->link ) > 8 )
        return( FALSE );
    return( TRUE );
}


void    ExpOp( TYPE typ1, TYPE typ2, OPTR opr ) {
//===============================================

// Generate code to perform exponentiation.

    if( UnaryMul( typ1, typ2 ) ) {
        PushOpn( CITNode );
        EmitOp( FC_UNARY_MUL );
        GenType( CITNode );
        OutU16( ITIntValue( CITNode->link ) );
        SetOpn( CITNode, USOPN_SAFE );
    } else {
        BinOp( typ1, typ2, opr );
    }
}


static void Unary( TYPE typ, OPTR opr ) {
//=======================================

// Generate code for unary plus or unary minus.

    PushOpn( CITNode->link );
    if( opr == OPTR_SUB ) {             // unary minus
        if( TypeCmplx( typ ) ) {
            EmitOp( FC_CUMINUS );
        } else {
            EmitOp( FC_UMINUS );
        }
        GenType( CITNode->link );
    } else if( ( _IsTypeInteger( CITNode->link->typ ) ) &&
               ( CITNode->link->size < sizeof( intstar4 ) ) ) {
        // convert INTEGER*1 or INTEGER*2 to INTEGER*4
        EmitOp( FC_CONVERT );
        DumpTypes( CITNode->link->typ, CITNode->link->size,
                             FT_INTEGER, sizeof( intstar4 ) );
    }
    SetOpn( CITNode, USOPN_SAFE );
}


static void Binary( TYPE typ1, TYPE typ2, OPTR opr ) {
//====================================================

// Generate code for binary operations.

    bool    flip;
    bool    associative;
    FCODE   op_code;

    associative = FALSE;
    if( ( opr == OPTR_ADD ) || ( opr == OPTR_MUL ) ) {
        associative = TRUE;
    }
    flip = FALSE;
    if( ( ( CITNode->opn.us & USOPN_WHERE ) == USOPN_SAFE ) &&
        ( ( CITNode->link->opn.us & USOPN_WHERE ) != USOPN_SAFE ) ) {
        flip = TRUE;
    }
    op_code = opr - OPTR_FIRST_ARITHOP;
    PushOpn( CITNode->link );
    PushOpn( CITNode );
    if( TypeCmplx( typ1 ) && TypeCmplx( typ2 ) ) {
        op_code += FC_CC_BINOPS;
        if( flip && !associative ) {
            EmitOp( FC_CMPLX_FLIP );
        }
    } else if( TypeCmplx( typ1 ) ) {
        if( flip ) {
            if( associative ) {
                op_code += FC_XC_BINOPS;
            } else {
                EmitOp( FC_XC_FLIP );
                op_code += FC_CX_BINOPS;
            }
        } else {
            op_code += FC_CX_BINOPS;
        }
    } else if( TypeCmplx( typ2 ) ) {
        if( flip ) {
            if( associative ) {
                op_code += FC_CX_BINOPS;
            } else {
                EmitOp( FC_CX_FLIP );
                op_code += FC_XC_BINOPS;
            }
        } else {
            op_code += FC_XC_BINOPS;
        }
    } else {
        op_code += FC_BINOPS;
        if( flip && !associative ) {
            EmitOp( FC_FLIP );
        }
    }
    EmitOp( op_code );
    if( flip && associative ) {
        GenTypes( CITNode->link, CITNode );
    } else {
        GenTypes( CITNode, CITNode->link );
    }
}


void    BinOp( TYPE typ1, TYPE typ2, OPTR opr ) {
//===============================================

// Generate code to perform a binary operation.

    if( typ1 != FT_NO_TYPE ) {                // binary operator
        Binary( typ1, typ2, opr );
    } else {                                  // unary operator.
        Unary( typ2, opr );
    }
}
