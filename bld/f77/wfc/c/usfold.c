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
* Description:  Constant folding routines.
*
****************************************************************************/


#include "ftnstd.h"
#include "optr.h"
#include "errcod.h"
#include "global.h"
#include "rtenv.h"
#include "ferror.h"
#include "usfold.h"
#include "lexcmp.h"
#include "ftextfun.h"
#include "upscan.h"
#include "binops.h"


extern  void            (* const __FAR GenOprTable[])(TYPE, TYPE, OPTR);


static void    MulIOFlow ( ftn_type *arg1, ftn_type *arg2 ) {
//===========================================================

    MulIOFlo(&arg1->intstar4, &arg2->intstar4);
}

//--------------------------------------------- integer arithmetic


void    AddI( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    if( AddIOFlo( &opnd1->intstar4, &opnd2->intstar4 ) ) {
        Warning( KO_IOVERFLOW );
    }
//  opnd1->intstar4 = opnd1->intstar4 + opnd2->intstar4;
}


void    SubI( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    if( SubIOFlo( &opnd1->intstar4, &opnd2->intstar4 ) ) {
        Warning( KO_IOVERFLOW );
    }
//  opnd1->intstar4 = opnd1->intstar4 - opnd2->intstar4;
}


void    MulI( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    if( MulIOFlo( &opnd1->intstar4, &opnd2->intstar4 ) ) {
        Warning( KO_IOVERFLOW );
    }
//  opnd1->intstar4 = opnd1->intstar4 * opnd2->intstar4;
}


void    DivI( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    if( opnd2->intstar4 != 0 ) {
        opnd1->intstar4 /= opnd2->intstar4;
    } else {
        Warning( KO_IDIV_ZERO );
    }
}


//--------------------------------------------- real arithmetic


void    AddR( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->single = opnd1->single + opnd2->single;
}


void    SubR( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->single = opnd1->single - opnd2->single;
}


void    MulR( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->single = opnd1->single * opnd2->single;
}


void    DivR( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->single = opnd1->single / opnd2->single;
}


//------------------------------------------- double arithmetic


void    AddD( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->dble = opnd1->dble + opnd2->dble;
}


void    SubD( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->dble = opnd1->dble - opnd2->dble;
}


void    MulD( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->dble = opnd1->dble * opnd2->dble;
}


void    DivD( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->dble = opnd1->dble / opnd2->dble;
}


//------------------------------------------- Extended arithmetic


void    AddE( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->extended = opnd1->extended + opnd2->extended;
}


void    SubE( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->extended = opnd1->extended - opnd2->extended;
}


void    MulE( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->extended = opnd1->extended * opnd2->extended;
}


void    DivE( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->extended = opnd1->extended / opnd2->extended;
}


//-------------------------------------------- complex arithmetic


void    AddC( ftn_type *x, ftn_type *y ) {
//========================================

    scomplex    result;

    result.realpart = x->scomplex.realpart + y->scomplex.realpart;
    result.imagpart = x->scomplex.imagpart + y->scomplex.imagpart;
    x->scomplex = result;
}


void    SubC( ftn_type *x, ftn_type *y ) {
//======================================

    scomplex    result;

    result.realpart = x->scomplex.realpart - y->scomplex.realpart;
    result.imagpart = x->scomplex.imagpart - y->scomplex.imagpart;
    x->scomplex = result;
}


void    MulC( ftn_type *x, ftn_type *y ) {
//======================================

    scomplex    result;

    result.realpart = x->scomplex.realpart * y->scomplex.realpart - x->scomplex.imagpart * y->scomplex.imagpart;
    result.imagpart = x->scomplex.realpart * y->scomplex.imagpart + x->scomplex.imagpart * y->scomplex.realpart;
    x->scomplex = result;
}


void    DivC( ftn_type *x, ftn_type *y ) {
//======================================

    single      bottom;
    scomplex    result;

    bottom = y->scomplex.realpart * y->scomplex.realpart + y->scomplex.imagpart * y->scomplex.imagpart;
    result.realpart = x->scomplex.realpart * y->scomplex.realpart + x->scomplex.imagpart * y->scomplex.imagpart;
    result.imagpart = x->scomplex.imagpart * y->scomplex.realpart - x->scomplex.realpart * y->scomplex.imagpart;
    result.realpart /= bottom;
    result.imagpart /= bottom;
    x->scomplex = result;
}


//------------------------------------------- dcomplex arithmetic


void    AddQ( ftn_type *x, ftn_type *y ) {
//========================================

    dcomplex    result;

    result.realpart = x->dcomplex.realpart + y->dcomplex.realpart;
    result.imagpart = x->dcomplex.imagpart + y->dcomplex.imagpart;
    x->dcomplex = result;
}


void    SubQ( ftn_type *x, ftn_type *y ) {
//========================================

    dcomplex    result;

    result.realpart = x->dcomplex.realpart - y->dcomplex.realpart;
    result.imagpart = x->dcomplex.imagpart - y->dcomplex.imagpart;
    x->dcomplex = result;
}


void    MulQ( ftn_type *x, ftn_type *y ) {
//========================================

    dcomplex    result;

    result.realpart = x->dcomplex.realpart * y->dcomplex.realpart - x->dcomplex.imagpart * y->dcomplex.imagpart;
    result.imagpart = x->dcomplex.realpart * y->dcomplex.imagpart + x->dcomplex.imagpart * y->dcomplex.realpart;
    x->dcomplex = result;
}


void    DivQ( ftn_type *x, ftn_type *y ) {
//========================================

    single      bottom;
    dcomplex    result;

    bottom = y->dcomplex.realpart * y->dcomplex.realpart + y->dcomplex.imagpart * y->dcomplex.imagpart;
    result.realpart = x->dcomplex.realpart * y->dcomplex.realpart + x->dcomplex.imagpart * y->dcomplex.imagpart;
    result.imagpart = x->dcomplex.imagpart * y->dcomplex.realpart - x->dcomplex.realpart * y->dcomplex.imagpart;
    result.realpart /= bottom;
    result.imagpart /= bottom;
    x->dcomplex = result;
}


//------------------------------------------- xcomplex arithmetic


void    AddX( ftn_type *x, ftn_type *y ) {
//========================================

    xcomplex    result;

    result.realpart = x->xcomplex.realpart + y->xcomplex.realpart;
    result.imagpart = x->xcomplex.imagpart + y->xcomplex.imagpart;
    x->xcomplex = result;
}


void    SubX( ftn_type *x, ftn_type *y ) {
//========================================

    xcomplex    result;

    result.realpart = x->xcomplex.realpart - y->xcomplex.realpart;
    result.imagpart = x->xcomplex.imagpart - y->xcomplex.imagpart;
    x->xcomplex = result;
}


void    MulX( ftn_type *x, ftn_type *y ) {
//========================================

    xcomplex    result;

    result.realpart = x->xcomplex.realpart * y->xcomplex.realpart - x->xcomplex.imagpart * y->xcomplex.imagpart;
    result.imagpart = x->xcomplex.realpart * y->xcomplex.imagpart + x->xcomplex.imagpart * y->xcomplex.realpart;
    x->xcomplex = result;
}


void    DivX( ftn_type *x, ftn_type *y ) {
//========================================

    single      bottom;
    xcomplex    result;

    bottom = y->xcomplex.realpart * y->xcomplex.realpart + y->xcomplex.imagpart * y->xcomplex.imagpart;
    result.realpart = x->xcomplex.realpart * y->xcomplex.realpart + x->xcomplex.imagpart * y->xcomplex.imagpart;
    result.imagpart = x->xcomplex.imagpart * y->xcomplex.realpart - x->xcomplex.realpart * y->xcomplex.imagpart;
    result.realpart /= bottom;
    result.imagpart /= bottom;
    x->xcomplex = result;
}

//------------------------------------------- unary plus/minus


void    XINeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->intstar4 = -opnd2->intstar4;
}


void    XRNeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->single = -opnd2->single;
}


void    XDNeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->dble = -opnd2->dble;
}


void    XENeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->extended = -opnd2->extended;
}


void    XCNeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->scomplex.realpart = -opnd2->scomplex.realpart;
    opnd1->scomplex.imagpart = -opnd2->scomplex.imagpart;
}


void    XQNeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->dcomplex.realpart = -opnd2->dcomplex.realpart;
    opnd1->dcomplex.imagpart = -opnd2->dcomplex.imagpart;
}


void    XXNeg( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->xcomplex.realpart = -opnd2->xcomplex.realpart;
    opnd1->xcomplex.imagpart = -opnd2->xcomplex.imagpart;
}


void    XIPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->intstar4 = opnd2->intstar4;
}


void    XRPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->single = opnd2->single;
}


void    XDPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->dble = opnd2->dble;
}


void    XEPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->extended = opnd2->extended;
}


void    XCPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->scomplex.realpart = opnd2->scomplex.realpart;
    opnd1->scomplex.imagpart = opnd2->scomplex.imagpart;
}


void    XQPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->dcomplex.realpart = opnd2->dcomplex.realpart;
    opnd1->dcomplex.imagpart = opnd2->dcomplex.imagpart;
}


void    XXPlus( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->xcomplex.realpart = opnd2->xcomplex.realpart;
    opnd1->xcomplex.imagpart = opnd2->xcomplex.imagpart;
}


//-------------------------------------------- logical operators


void    XLEqv( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->logstar1 = ( opnd1->logstar1 == opnd2->logstar1 );
}


void    XLNeqv( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->logstar1 = ( opnd1->logstar1 != opnd2->logstar1 );
}


void    XLAnd( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->logstar1 = ( opnd1->logstar1 && opnd2->logstar1 );
}


void    XLOr( ftn_type *opnd1, ftn_type *opnd2 ) {
//================================================

    opnd1->logstar1 = ( opnd1->logstar1 || opnd2->logstar1 );
}


void    XLNot( ftn_type *opnd1, ftn_type *opnd2 ) {
//=================================================

    opnd1->logstar1 = !opnd2->logstar1;
}


//-------------------------------------------- bitwise operators


void    XBitEqv( ftn_type *opnd1, ftn_type *opnd2 ) {
//===================================================

    opnd1->intstar4 = ~( opnd1->intstar4 ^ opnd2->intstar4 );
}


void    XBitNeqv( ftn_type *opnd1, ftn_type *opnd2 ) {
//====================================================

    opnd1->intstar4 = ( opnd1->intstar4 ^ opnd2->intstar4 );
}


void    XBitAnd( ftn_type *opnd1, ftn_type *opnd2 ) {
//===================================================

    opnd1->intstar4 = ( opnd1->intstar4 & opnd2->intstar4 );
}


void    XBitOr( ftn_type *opnd1, ftn_type *opnd2 ) {
//==================================================

    opnd1->intstar4 = ( opnd1->intstar4 | opnd2->intstar4 );
}


void    XBitNot( ftn_type *opnd1, ftn_type *opnd2 ) {
//===================================================

    opnd1->intstar4 = ~opnd2->intstar4;
}


//------------------------------------------- CMP routines


void    XICmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    if( opnd1->intstar4 < opnd2->intstar4 ) {
        cmp = 0;
    } else if( opnd1->intstar4 == opnd2->intstar4 ) {
        cmp = 1;
    } else {
        cmp = 2;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XRCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    if( opnd1->single < opnd2->single ) {
        cmp = 0;
    } else if( opnd1->single == opnd2->single ) {
        cmp = 1;
    } else {
        cmp = 2;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XDCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    if( opnd1->dble < opnd2->dble ) {
        cmp = 0;
    } else if( opnd1->dble == opnd2->dble ) {
        cmp = 1;
    } else {
        cmp = 2;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XECmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    if( opnd1->extended < opnd2->extended ) {
        cmp = 0;
    } else if( opnd1->extended == opnd2->extended ) {
        cmp = 1;
    } else {
        cmp = 2;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XCCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    cmp = 2;
    if( ( opnd1->scomplex.realpart == opnd2->scomplex.realpart ) &&
        ( opnd1->scomplex.imagpart == opnd2->scomplex.imagpart ) ) {
        cmp = 1;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XQCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    cmp = 2;
    if( ( opnd1->dcomplex.realpart == opnd2->dcomplex.realpart ) &&
        ( opnd1->dcomplex.imagpart == opnd2->dcomplex.imagpart ) ) {
        cmp = 1;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XXCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//============================================================================

    int         cmp;

    cmp = 2;
    if( ( opnd1->xcomplex.realpart == opnd2->xcomplex.realpart ) &&
        ( opnd1->xcomplex.imagpart == opnd2->xcomplex.imagpart ) ) {
        cmp = 1;
    }
    opnd1->logstar4 = res[ cmp ];
}


void    XChCmp( ftn_type *opnd1, ftn_type *opnd2, const logstar1 __FAR *res ) {
//=============================================================================

    opnd1->logstar4 = res[ 1 + LexStrCmp( opnd1->cstring.strptr,
                                          opnd1->cstring.len,
                                          opnd2->cstring.strptr,
                                          opnd2->cstring.len ) ];
}


//-------------------------------------------- exponentiation


void    GenExp( TYPE typ ) {
//==========================

    OPTR    op;

    AddConst( CITNode );
    AddConst( CITNode->link );
    op = OPTR_EXP;
    GenOprTable[ op ]( typ, typ, op );
}


static  void    InitOne( TYPE typ, ftn_type *one ) {
//==================================================

    switch( typ ) {
    case FT_INTEGER_1:
    case FT_INTEGER_2:
    case FT_INTEGER:
        one->intstar4 = 1;
        break;
    case FT_REAL:
        one->single = 1;
        break;
    case FT_DOUBLE:
        one->dble = 1;
        break;
    case FT_TRUE_EXTENDED:
        one->extended = 1;
        break;
    case FT_COMPLEX:
        one->scomplex.realpart = 1;
        one->scomplex.imagpart = 0;
        break;
    case FT_DCOMPLEX:
        one->dcomplex.realpart = 1;
        one->dcomplex.imagpart = 0;
    case FT_TRUE_XCOMPLEX:
        one->xcomplex.realpart = 1;
        one->xcomplex.imagpart = 0;
    }
}


static  bool    ZeroBase( TYPE typ, ftn_type *base ) {
//====================================================

    switch( typ ) {
    case FT_INTEGER_1:
    case FT_INTEGER_2:
    case FT_INTEGER:
        return( base->intstar4 == 0 );
    case FT_REAL:
        return( base->single == 0 );
    case FT_DOUBLE:
        return( base->dble == 0 );
    case FT_TRUE_EXTENDED:
        return( base->extended == 0 );
    case FT_COMPLEX:
        return( ( base->scomplex.realpart == 0 ) &&
                ( base->scomplex.imagpart == 0 ) );
    case FT_DCOMPLEX:
        return( ( base->dcomplex.realpart == 0 ) &&
                ( base->dcomplex.imagpart == 0 ) );
    default: // case FT_XCOMPLEX:
        return( ( base->xcomplex.realpart == 0 ) &&
                ( base->xcomplex.imagpart == 0 ) );
    }
}


static  void            (* const __FAR Mul[])( ftn_type *, ftn_type * ) =
                                    { &MulIOFlow, &MulIOFlow, &MulIOFlow,
                                      &MulR, &MulD, &MulE,
                                      &MulC, &MulQ, &MulX };
static  void            (* const __FAR Div[])( ftn_type *_x, ftn_type *_y ) =
                                    { &DivI, &DivI, &DivI,
                                      &DivR, &DivD, &DivE,
                                      &DivC, &DivQ, &DivX };


void    ExpI( TYPE typ, ftn_type *base, intstar4 exponent ) {
//===========================================================

    bool        odd;
    ftn_type    result;

    if( ZeroBase( typ, base ) ) {
        if( exponent <= 0 ) {
            Error( EX_Z_2_NOT_POS );
        }
    } else if( exponent == 0 ) {
        InitOne( typ, base );
    } else {
        InitOne( typ, &result );
        if( exponent < 0 ) {
            // base == 1 / base
            Div[ typ - FT_INTEGER_1 ]( &result, base );
            *base = result;
            InitOne( typ, &result );
            exponent = -exponent;
        }
        if( ( exponent & 1 ) != 0 ) {
            result = *base;
        }
        exponent /= 2;
        while( exponent != 0 ) {
            for(;;) {
                Mul[ typ - FT_INTEGER_1 ]( base, base );
                odd = ( ( exponent & 1 ) != 0 );
                exponent /= 2;
                if( odd ) break;
            }
            Mul[ typ - FT_INTEGER_1 ]( &result, base );
        }
        *base = result;
    }
}
