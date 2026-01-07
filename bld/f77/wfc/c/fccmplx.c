/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
// FCCMPLX  : F-Code processor for complex arithmetic.
//

#include "ftnstd.h"
#include "global.h"
#include "wf77aux.h"
#include "wf77cg.h"
#include "tmpdefs.h"
#include "cpopt.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cnvd2s.h"
#include "fcjmptab.h"
#include "fccmplx.h"
#include "fcstruct.h"
#include "fctemp.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


/* Forward declarations */
static void    XCmplxMixOp( RTCODE rtn_id, bool cmplx_scalar );
static void    InLineMulCC( uint_16 typ_info );


cg_name ImagPtr( cg_name dest, cg_type cgtyp ) {
//============================================

// Get pointer to imaginary part of complex number.

    dest = StructRef( dest, BETypeLength( cgtyp ) );
    if( OZOpts & OZOPT_O_VOLATILE ) {
        dest = CGVolatile( dest );
    }
    return( dest );
}


cg_type         CmplxBaseType( cg_type cgtyp ) {
//============================================

    if( cgtyp == TY_COMPLEX )
        return( TY_SINGLE );
    if( cgtyp == TY_DCOMPLEX )
        return( TY_DOUBLE );
    return( TY_LONGDOUBLE );
}


static cg_type MapCmplxType( cg_type cgtyp )
//========================================
{
    if( cgtyp == TY_DOUBLE )
        return( TY_DCOMPLEX );
    if( cgtyp == TY_LONGDOUBLE )
        return( TY_XCOMPLEX );
    return( TY_COMPLEX );
}


static RTCODE MapCmplxRtCode( RTCODE rtc, cg_type cgtyp )
//=====================================================
{
    if( cgtyp == TY_DOUBLE ) {
        switch( rtc ) {
        case RT_CMPLXMUL:
            return( RT_C16MUL );
        case RT_CMPLXDIV:
            return( RT_C16DIV );
        case RT_CMPLXPOW:
            return( RT_C16POW );
        case RT_CMPLXPOWI:
            return( RT_C16POWI );
        }
    } else if( cgtyp == TY_LONGDOUBLE ) {
        switch( rtc ) {
        case RT_CMPLXMUL:
            return( RT_C32MUL );
        case RT_CMPLXDIV:
            return( RT_C32DIV );
        case RT_CMPLXPOW:
            return( RT_C32POW );
        case RT_CMPLXPOWI:
            return( RT_C32POWI );
        }
    }
    return( rtc );
}


void    SplitCmplx( cg_name cmplx_addr, cg_type cgtyp ) {
//=====================================================

// Split real and imaginary parts of complex number.

    cg_name     cmplx_1;
    cg_name     cmplx_2;

    cgtyp = CmplxBaseType( cgtyp );
    CloneCGName( cmplx_addr, &cmplx_1, &cmplx_2 );
    XPush( CGUnary( O_POINTS, ImagPtr( cmplx_1, cgtyp ), cgtyp ) );
    XPush( CGUnary( O_POINTS, cmplx_2, cgtyp ) );
}


static void DoCmplxOp( RTCODE rtn_id, cg_name a, cg_name b, cg_name c, cg_name d )
//================================================================================
// Do a complex operation.
{
    call_handle call;
    cg_type     cgtyp;

    cgtyp = ResCGType( CGType( a ), CGType( c ) );
    call = InitCall( MapCmplxRtCode( rtn_id, cgtyp ) );
    CGAddParm( call, a, cgtyp );
    CGAddParm( call, b, cgtyp );
    CGAddParm( call, c, cgtyp );
    CGAddParm( call, d, cgtyp );
    SplitCmplx( CGCall( call ), MapCmplxType( cgtyp ) );
}


void            XPopCmplx( cg_cmplx *z, cg_type cgtyp ) {
//=====================================================

// Get complex value.

    cg_name     opn;

    opn = XPop();
    if( IsCGPointer( CGType( opn ) ) ) {
        SplitCmplx( opn, cgtyp );
        z->realpart = XPop();
    } else {
        z->realpart = opn;
    }
    z->imagpart = XPop();
}


static void    XCmplxOp( RTCODE rtn_id ) {
//=================================

// F-Code processor for binary complex number operations involving
// runtime routines.
// ( a, b ) OP ( c, d ).

    uint_16     typ_info;
    cg_cmplx    x;
    cg_cmplx    y;

    typ_info = GetU16();
    XPopCmplx( &x, GetCGTypes1( typ_info ) );
    XPopCmplx( &y, GetCGTypes2( typ_info ) );
    DoCmplxOp( rtn_id, x.realpart, x.imagpart, y.realpart, y.imagpart );
}


static  void    XCmplx( int op ) {
//================================

// Binary operator F-Code processor for complex addition and subtraction.

    uint_16     typ_info;
    cg_type     cgtyp1;
    cg_type     cgtyp2;
    cg_cmplx    x;
    cg_cmplx    y;

    typ_info = GetU16();
    cgtyp1 = GetCGTypes1( typ_info );
    cgtyp2 = GetCGTypes2( typ_info );
    XPopCmplx( &x, cgtyp1 );
    XPopCmplx( &y, cgtyp2 );
    cgtyp1 = CmplxBaseType( cgtyp1 );
    cgtyp2 = CmplxBaseType( cgtyp2 );
    XPush( CGBinary( op, x.imagpart, y.imagpart, ResCGType( cgtyp1, cgtyp2 ) ) );
    XPush( CGBinary( op, x.realpart, y.realpart, ResCGType( cgtyp1, cgtyp2 ) ) );
}


static void    XMixed( int op, bool cmplx_scalar ) {
//===========================================

// Binary F-Code processor for cmplx-scalar addition & subtraction.
// cx   - true if complex OP scalar, false if scalar OP complex.

    cg_cmplx    z;
    cg_name     x;
    uint_16     typ_info;
    cg_type     z_cgtyp;
    cg_type     x_cgtyp;

    typ_info = GetU16();
    if( cmplx_scalar ) {
        z_cgtyp = GetCGTypes1( typ_info );
        x_cgtyp = GetCGTypes2( typ_info );
        XPopCmplx( &z, z_cgtyp );
        x = XPopValue( x_cgtyp );
    } else {
        x_cgtyp = GetCGTypes1( typ_info );
        z_cgtyp = GetCGTypes2( typ_info );
        x = XPopValue( x_cgtyp );
        XPopCmplx( &z, z_cgtyp );
    }
    z_cgtyp = CmplxBaseType( z_cgtyp );
    if( cmplx_scalar ) {
        XPush( z.imagpart );
        XPush( CGBinary( op, z.realpart, x, ResCGType( z_cgtyp, x_cgtyp ) ) );
    } else {
        if( op == O_MINUS ) {
            XPush( CGUnary( O_UMINUS, z.imagpart, z_cgtyp ) );
        } else {
            XPush( z.imagpart );
        }
        XPush( CGBinary( op, x, z.realpart, ResCGType( x_cgtyp, z_cgtyp ) ) );
    }
}


static void    XMulDivMix( int op, bool cmplx_scalar, uint_16 typ_info ) {
//=====================================================================

// Binary F-Code processor for mixed multiplication and division.

    cg_cmplx    z;
    cg_name     s;
    cg_type     s_cgtyp;
    cg_type     z_cgtyp;
    cg_name     s_1;
    cg_name     s_2;

    if( cmplx_scalar ) {
        z_cgtyp = GetCGTypes1( typ_info );
        s_cgtyp = GetCGTypes2( typ_info );
        XPopCmplx( &z, z_cgtyp );
        s = XPopValue( s_cgtyp );
    } else {
        s_cgtyp = GetCGTypes1( typ_info );
        z_cgtyp = GetCGTypes2( typ_info );
        s = XPopValue( s_cgtyp );
        XPopCmplx( &z, z_cgtyp );
    }
    z_cgtyp = ResCGType( s_cgtyp, CmplxBaseType( z_cgtyp ) );
    CloneCGName( s, &s_1, &s_2 );
    XPush( CGBinary( op, z.imagpart, s_1, z_cgtyp ) );
    XPush( CGBinary( op, z.realpart, s_2, z_cgtyp ) );
}


void    FCCmplxFlip( void ) {
//=====================

// Flip 2 complex operands.

    cg_name     rp_1;
    cg_name     ip_1 = NULL;
    cg_name     rp_2;
    cg_name     ip_2 = NULL;

    rp_1 = XPop();
    if( !IsCGPointer( CGType( rp_1 ) ) ) {
        ip_1 = XPop();
    }
    rp_2 = XPop();
    if( !IsCGPointer( CGType( rp_2 ) ) ) {
        ip_2 = XPop();
    }
    if( !IsCGPointer( CGType( rp_1 ) ) ) {
        XPush( ip_1 );
    }
    XPush( rp_1 );
    if( !IsCGPointer( CGType( rp_2 ) ) ) {
        XPush( ip_2 );
    }
    XPush( rp_2 );
}


void    FCCXFlip( void ) {
//==================

// Flip complex and scalar operands.

    cg_name     rp;
    cg_name     ip = NULL;
    cg_name     scalar;

    rp = XPop();
    if( !IsCGPointer( CGType( rp ) ) ) {
        ip = XPop();
    }
    scalar = XPop();
    if( !IsCGPointer( CGType( rp ) ) ) {
        XPush( ip );
    }
    XPush( rp );
    XPush( scalar );
}


void    FCXCFlip( void ) {
//==================

// Flip scalar and complex operands.

    cg_name     rp;
    cg_name     ip = NULL;
    cg_name     scalar;

    scalar = XPop();
    rp = XPop();
    if( !IsCGPointer( CGType( rp ) ) ) {
        ip = XPop();
    }
    XPush( scalar );
    if( !IsCGPointer( CGType( rp ) ) ) {
        XPush( ip );
    }
    XPush( rp );
}


void    FCUMinusCmplx( void ) {
//=======================

// Unary minus (-) F-Code processor for complex numbers.

    cg_cmplx    op;
    cg_type     cgtyp;

    cgtyp = GetCGType( GetU16() );
    XPopCmplx( &op, cgtyp );
    cgtyp = CmplxBaseType( cgtyp );
    XPush( CGUnary( O_UMINUS, op.imagpart, cgtyp ) );
    XPush( CGUnary( O_UMINUS, op.realpart, cgtyp ) );
}


void    FCAddCmplx( void ) {
//====================

// Add one complex number to another.

    XCmplx( O_PLUS );
}


void    FCSubCmplx( void ) {
//====================

// Subtract one complex number from another.

    XCmplx( O_MINUS );
}


void    FCAddMixCX( void ) {
//====================

// Add a complex to a scalar.

    XMixed( O_PLUS, true );
}


void    FCAddMixXC( void ) {
//====================

// Add a scalar to a complex.

    XMixed( O_PLUS, false );
}


void    FCSubMixCX( void ) {
//====================

// Subtract a scalar from a complex.

    XMixed( O_MINUS, true );
}


void    FCSubMixXC( void ) {
//====================

// Subtract a scalar from a complex.

    XMixed( O_MINUS, false );
}


void    FCMulMixCX( void ) {
//====================

// Multiply a complex by a scalar.

    XMulDivMix( O_TIMES, true, GetU16() );
}


void    FCMulMixXC( void ) {
//====================

// Multiply a scalar by a complex.

    XMulDivMix( O_TIMES, false, GetU16() );
}


void    FCDivMixCX( void ) {
//====================

// Divide a complex by a scalar.

    XMulDivMix( O_DIV, true, GetU16() );
}


void    FCDivMixXC( void ) {
//====================

// Divide a scalar by a complex.

    XCmplxMixOp( RT_CMPLXDIV, false );
}


void    FCMulCmplx( void ) {
//====================

// Multiply one complex number by another.
#if _INTEL_CPU
    if( CPUOpts & CPUOPT_FPC ) {
        // generate call to runtime complex multiply
        XCmplxOp( RT_CMPLXMUL );
    } else {
        // do multiplication inline
        InLineMulCC( GetU16() );
    }
#else /* _RISC_CPU */
    // For risc we just inline the entire code
    InLineMulCC( GetU16() );
#endif
}


static void    InLineMulCC( uint_16 typ_info ) {
//===========================================

// Do complex multiplication in-line.
// (c,d) * (a,b).

    cg_name     d_1;
    cg_name     d_2;
    cg_name     c_1;
    cg_name     c_2;
    cg_name     b_1;
    cg_name     b_2;
    cg_name     a_1;
    cg_name     a_2;
    cg_type     cgtyp1;
    cg_type     cgtyp2;
    cg_cmplx    x;
    cg_cmplx    y;

    cgtyp1 = GetCGTypes1( typ_info );
    cgtyp2 = GetCGTypes2( typ_info );
    XPopCmplx( &x, cgtyp1 );
    XPopCmplx( &y, cgtyp2 );
    cgtyp1 = CmplxBaseType( cgtyp1 );
    cgtyp2 = CmplxBaseType( cgtyp2 );
    CloneCGName( x.realpart, &a_1, &a_2 );
    CloneCGName( x.imagpart, &b_1, &b_2 );
    CloneCGName( y.realpart, &c_1, &c_2 );
    CloneCGName( y.imagpart, &d_1, &d_2 );
    cgtyp1 = ResCGType( cgtyp1, cgtyp2 );
    XPush( CGBinary( O_PLUS,
                     CGBinary( O_TIMES, a_1, d_1, cgtyp1 ),
                     CGBinary( O_TIMES, b_1, c_1, cgtyp1 ),
                     cgtyp1 ) );
    XPush( CGBinary( O_MINUS,
                     CGBinary( O_TIMES, a_2, c_2, cgtyp1 ),
                     CGBinary( O_TIMES, b_2, d_2, cgtyp1 ),
                     cgtyp1 ) );
}


void    FCDivCmplx( void ) {
//====================

// Binary division for complex numbers.

    XCmplxOp( RT_CMPLXDIV );
}


void    FCExpCmplx( void ) {
//====================

// Binary exponentiation for complex numbers.

    XCmplxOp( RT_CMPLXPOW );
}


void    FCExpMixCX( void ) {
//====================

// Binary exponentiation for complex**non-complex.

    XCmplxMixOp( RT_CMPLXPOW, true );
}


void    FCExpMixXC( void ) {
//====================

// Binary exponentiation for non-complex**complex.

    XCmplxMixOp( RT_CMPLXPOW, false );
}


static cg_type PromoteIntType( cg_type cgtyp ) {
//============================================

    if( ( cgtyp == TY_INT_1 ) || ( cgtyp == TY_INT_2 ) ) {
        cgtyp = TY_INT_4;
    }
    return( cgtyp );
}


static void    DoCmplxScalarOp( RTCODE rtn_id, cg_name a, cg_name b, cg_name s )
//==============================================================================
// Do a complex operation.
{
    call_handle call;
    cg_type     cgtyp;

    cgtyp = CGType( a );
    call = InitCall( MapCmplxRtCode( rtn_id, cgtyp ) );
    CGAddParm( call, a, cgtyp );
    CGAddParm( call, b, cgtyp );
    CGAddParm( call, s, PromoteIntType( CGType( s ) ) );
    SplitCmplx( CGCall( call ), MapCmplxType( cgtyp ) );
}


static void    XCmplxMixOp( RTCODE rtn_id, bool cmplx_scalar ) {
//=======================================================

// F-Code processor for binary complex number operations involving
// runtime routines.
// x / (c,d) or (c,d) / x

    uint_16     typ_info;
    cg_type     s_cgtyp;
    cg_type     x_cgtyp;
    cg_name     s;
    cg_cmplx    x;

    typ_info = GetU16();
    if( cmplx_scalar ) {
        x_cgtyp = GetCGTypes1( typ_info );
        s_cgtyp = GetCGTypes2( typ_info );
        XPopCmplx( &x, x_cgtyp );
        s = XPopValue( s_cgtyp );
    } else {
        s_cgtyp = GetCGTypes1( typ_info );
        x_cgtyp = GetCGTypes2( typ_info );
        s = XPopValue( s_cgtyp );
        XPopCmplx( &x, x_cgtyp );
    }
    x_cgtyp = ResCGType( s_cgtyp, CmplxBaseType( x_cgtyp ) );
    if( cmplx_scalar ) {
        // currently, the only time XCmplxMixOp() is called when the left
        // operand is complex and the right operand is a scalar, is for
        // exponentiation
        s_cgtyp = PromoteIntType( s_cgtyp );
        if( s_cgtyp == TY_INT_4 ) {
            DoCmplxScalarOp( RT_CMPLXPOWI, x.realpart, x.imagpart, s );
        } else {
            DoCmplxOp( rtn_id, x.realpart, x.imagpart, s, CGInteger( 0, x_cgtyp ) );
        }
    } else {
        DoCmplxOp( rtn_id, s, CGInteger( 0, x_cgtyp ), x.realpart, x.imagpart );
    }
}


static  void    CCCmp( cg_op op, cg_name a, cg_name b, cg_name c, cg_name d ) {
//=============================================================================

// Complex/Complex compare.

    cg_type     res_cgtyp;
    cg_op       flow_op;

    res_cgtyp = ResCGType( CGType( a ), CGType( c ) );
    if( op == O_EQ ) {
       flow_op = O_FLOW_AND;
    } else {
       flow_op = O_FLOW_OR;
    }
    XPush( CGFlow( flow_op, CGCompare( op, a, c, res_cgtyp ),
                            CGCompare( op, b, d, res_cgtyp ) ) );
}


static void    CCCompare( int op ) {
//===========================

// Complex/Complex compare.

    cg_cmplx    x;
    cg_cmplx    y;
    uint_16     typ_info;

    typ_info = GetU16();
    XPopCmplx( &x, GetCGTypes1( typ_info ) );
    XPopCmplx( &y, GetCGTypes2( typ_info ) );
    CCCmp( op, x.realpart, x.imagpart, y.realpart, y.imagpart );
}


static void    XCCompare( int op ) {
//===========================

// Scalar/Complex compare.

    cg_name     x;
    cg_cmplx    z;
    uint_16     typ_info;
    cg_type     cgtyp;

    typ_info = GetU16();
    cgtyp = GetCGTypes1( typ_info );
    x = XPopValue( cgtyp );
    XPopCmplx( &z, GetCGTypes2( typ_info ) );
    CCCmp( op, x, CGInteger( 0, cgtyp ), z.realpart, z.imagpart );
}


static  void    CXCompare( int op ) {
//===================================

// Complex/Scalar compare.

    cg_name     x;
    cg_cmplx    z;
    uint_16     typ_info;
    cg_type     cgtyp;

    typ_info = GetU16();
    cgtyp = GetCGTypes2( typ_info );
    XPopCmplx( &z, GetCGTypes1( typ_info ) );
    x = XPopValue( cgtyp );
    CCCmp( op, z.realpart, z.imagpart, x, CGInteger( 0, cgtyp ) );
}


void    FCCCCmpEQ( void ) {
//===================

// Complex/Complex compare for equality.

    CCCompare( O_EQ );
}


void    FCCCCmpNE( void ) {
//===================

// Complex/Complex compare for non-equality.

    CCCompare( O_NE );
}


void    FCXCCmpEQ( void ) {
//===================

// Scalar/Complex compare for equality.

    XCCompare( O_EQ );
}


void    FCXCCmpNE( void ) {
//===================

// Scalar/Complex compare for non-equality.

    XCCompare( O_NE );
}


void    FCCXCmpEQ( void ) {
//===================

// Complex/Scalar compare for equality.

    CXCompare( O_EQ );
}


void    FCCXCmpNE( void ) {
//===================

// Complex/Scalar compare for non-equality.

    CXCompare( O_NE );
}


void    PushComplex( sym_id sym ) {
//=================================

// Push a complex number.

    XPush( SymAddr( sym ) );
}


void            PushCmplxConst( sym_id sym ) {
//============================================

// Push a complex constant.

    char        fmt_buff[80];

    if( sym->u.cn.typ == FT_COMPLEX ) {
        CnvS2S( &sym->u.cn.value.scomplex.imagpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_SINGLE ) );
        CnvS2S( &sym->u.cn.value.scomplex.realpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_SINGLE ) );
    } else if( sym->u.cn.typ == FT_DCOMPLEX ) {
        CnvD2S( &sym->u.cn.value.dcomplex.imagpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_DOUBLE ) );
        CnvD2S( &sym->u.cn.value.dcomplex.realpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_DOUBLE ) );
    } else {
        CnvX2S( &sym->u.cn.value.xcomplex.imagpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_LONGDOUBLE ) );
        CnvX2S( &sym->u.cn.value.xcomplex.realpart, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_LONGDOUBLE ) );
    }
}


void            CmplxAssign( sym_id sym, cg_type dst_cgtyp, cg_type src_cgtyp ) {
//===========================================================================

// Do complex assignment.

    cg_type     cgtyp;
    cg_name     dest;
    cg_name     dest_1;
    cg_name     dest_2;
    cg_cmplx    z;
    uint_16     flags;
    temp_handle tr;
    temp_handle ti;

    flags = sym->u.ns.flags;
    dest = NULL;
    if( (flags & SY_CLASS) == SY_SUBPROGRAM ) {
        // assigning to statement function
        if( (OZOpts & OZOPT_O_INLINE) == 0 ) {
            dest = SymAddr( sym );
        }
    } else {
        // check for structure type before checking for array
        // Consider:    A(1).X = A(2).X
        //    where A is an array of structures containing complex field X
        if( sym->u.ns.u1.s.typ == FT_STRUCTURE ) {
            dest = XPop();
            GetU16(); // ignore structure information
        } else if( flags & SY_SUBSCRIPTED ) {
            dest = XPop();
        } else {
            dest = SymAddr( sym );
        }
    }
    cgtyp = CmplxBaseType( dst_cgtyp );
    if( ( src_cgtyp != TY_COMPLEX ) && ( src_cgtyp != TY_DCOMPLEX ) &&
                                                ( src_cgtyp != TY_XCOMPLEX ) ) {
        z.realpart = XPopValue( src_cgtyp );
        z.imagpart = CGInteger( 0, cgtyp );
    } else {
        XPopCmplx( &z, src_cgtyp );
        z.imagpart = CGEval( z.imagpart );
    }
    z.realpart = CGEval( z.realpart );
    // Before assigning the real and imaginary parts, force evaluation of each.
    // Consider:    Z = Z * Z
    // The above expression will be evaluated as follows.
    //     z.r = z.r*z.r - z.i*z.i
    //     z.i = z.r*z.i + z.r*z.i
    // In the expression that evaluates the imaginary part, the value of "z.r"
    // must be the original value and not the new value.
    if( ((flags & SY_CLASS) == SY_SUBPROGRAM) && (OZOpts & OZOPT_O_INLINE) ) {
        XPush( z.imagpart );
        XPush( z.realpart );
        return;
    }

    // Code to avoid the criss cross problem
    // i.e. z = complx(imag(z), real(z))
    // or similar problems due to overwriting of one part with the other
    // before accessing it.
    // This should not affect efficiency (for optimized code) very much
    // because the temps will not be used when they are not required
    tr = CGTemp( cgtyp );
    ti = CGTemp( cgtyp );
    CGDone( CGAssign( CGTempName( tr, cgtyp ), z.realpart, cgtyp ) );
    CGDone( CGAssign( CGTempName( ti, cgtyp ), z.imagpart, cgtyp ) );

    CloneCGName( dest, &dest_1, &dest_2 );
    XPush( CGAssign( ImagPtr( dest_2, cgtyp ),
                CGUnary( O_POINTS, CGTempName( ti, cgtyp ), cgtyp ), cgtyp ) );
    XPush( CGAssign( dest_1, CGUnary( O_POINTS, CGTempName( tr, cgtyp ), cgtyp ),
                        cgtyp ) );
}


cg_name         CmplxAddr( cg_name real, cg_name imag ) {
//=======================================================

// Pass a complex value to a subprogram.

    tmp_handle  tmp;
    cg_type     cgtyp;
    cg_type     c_cgtyp;

    cgtyp = CGType( real );
    if( cgtyp == TY_SINGLE ) {
        c_cgtyp = TY_COMPLEX;
    } else if( cgtyp == TY_DOUBLE ) {
        c_cgtyp = TY_DCOMPLEX;
    } else {
        c_cgtyp = TY_XCOMPLEX;
    }
    tmp = AllocTmp( c_cgtyp );
    CGTrash( CGAssign( TmpPtr( tmp, c_cgtyp ), real, cgtyp ) );
    CGTrash( CGAssign( ImagPtr( TmpPtr( tmp, c_cgtyp ), cgtyp ), imag, cgtyp ) );
    return( TmpPtr( tmp, c_cgtyp ) );
}


void            Cmplx2Scalar( void ) {
//==============================

// Convert complex to scalar.

    cg_name     opn;

    opn = XPop();
    if( !IsCGPointer( CGType( opn ) ) ) {
        CGTrash( XPop() );
    }
    XPush( opn );
}


void    FCImag( void ) {
//================

    cg_name     opn;
    cg_type     cgtyp;

    cgtyp = CmplxBaseType( GetCGType( GetU16() ) );
    opn = XPop();
    if( IsCGPointer( CGType( opn ) ) ) {
        XPush( CGUnary( O_POINTS, ImagPtr( opn, cgtyp ), cgtyp ) );
    } else {
        CGTrash( opn );
    }
}


void    FCConjg( void ) {
//=================

    cg_cmplx    z;
    cg_type     cgtyp;

    cgtyp = GetCGType( GetU16() );
    XPopCmplx( &z, cgtyp );
    XPush( CGUnary( O_UMINUS, z.imagpart, CmplxBaseType( cgtyp ) ) );
    XPush( z.realpart );
}
