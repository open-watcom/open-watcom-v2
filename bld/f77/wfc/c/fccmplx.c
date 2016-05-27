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
// FCCMPLX  : F-Code processor for complex arithmetic.
//

#include "ftnstd.h"
#include "rtconst.h"
#include "wf77defs.h"
#include "wf77cg.h"
#include "wf77auxd.h"
#include "tmpdefs.h"
#include "cpopt.h"
#include "global.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cnvd2s.h"
#include "fcjmptab.h"
#include "fccmplx.h"
#include "fcstruct.h"
#include "fctemp.h"
#include "fcrtns.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


/* Forward declarations */
static void    XCmplxMixOp( RTCODE rtn_id, bool cmplx_scalar );
static void    InLineMulCC( unsigned_16 typ_info );


cg_name ImagPtr( cg_name dest, cg_type typ ) {
//============================================

// Get pointer to imaginary part of complex number.

    dest = StructRef( dest, BETypeLength( typ ) );
    if( OZOpts & OZOPT_O_VOLATILE ) {
        dest = CGVolatile( dest );
    }
    return( dest );
}


cg_type         CmplxBaseType( cg_type typ ) {
//============================================

    if( typ == TY_COMPLEX ) return( TY_SINGLE );
    if( typ == TY_DCOMPLEX ) return( TY_DOUBLE );
    return( TY_LONGDOUBLE );
}


void    SplitCmplx( cg_name cmplx_addr, cg_type typ ) {
//=====================================================

// Split real and imaginary parts of complex number.

    cg_name     cmplx_1;
    cg_name     cmplx_2;

    typ = CmplxBaseType( typ );
    CloneCGName( cmplx_addr, &cmplx_1, &cmplx_2 );
    XPush( CGUnary( O_POINTS, ImagPtr( cmplx_1, typ ), typ ) );
    XPush( CGUnary( O_POINTS, cmplx_2, typ ) );
}


static void DoCmplxOp( RTCODE rtn_id, cg_name a, cg_name b, cg_name c, cg_name d ) {
//==================================================================================

// Do a complex operation.

    call_handle handle;
    cg_type     typ;
    cg_type     r_typ;

    typ = ResCGType( CGType( a ), CGType( c ) );
    if( typ == TY_DOUBLE ) {
        rtn_id += RT_C_DOUBLE;
        r_typ = TY_DCOMPLEX;
    } else if( typ == TY_LONGDOUBLE ) {
        rtn_id += RT_C_EXTENDED;
        r_typ = TY_XCOMPLEX;
    } else {
        r_typ = TY_COMPLEX;
    }
    handle = InitCall( rtn_id );
    CGAddParm( handle, a, typ );
    CGAddParm( handle, b, typ );
    CGAddParm( handle, c, typ );
    CGAddParm( handle, d, typ );
    SplitCmplx( CGCall( handle ), r_typ );
}


void            XPopCmplx( cg_cmplx *z, cg_type typ ) {
//=====================================================

// Get complex value.

    cg_name     opn;

    opn = XPop();
    if( TypePointer( CGType( opn ) ) ) {
        SplitCmplx( opn, typ );
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

    unsigned_16 typ_info;
    cg_cmplx    x;
    cg_cmplx    y;

    typ_info = GetU16();
    XPopCmplx( &x, GetType1( typ_info ) );
    XPopCmplx( &y, GetType2( typ_info ) );
    DoCmplxOp( rtn_id, x.realpart, x.imagpart, y.realpart, y.imagpart );
}


static  void    XCmplx( int op ) {
//================================

// Binary operator F-Code processor for complex addition and subtraction.

    unsigned_16 typ_info;
    int         typ1;
    int         typ2;
    cg_cmplx    x;
    cg_cmplx    y;

    typ_info = GetU16();
    typ1 = GetType1( typ_info );
    typ2 = GetType2( typ_info );
    XPopCmplx( &x, typ1 );
    XPopCmplx( &y, typ2 );
    typ1 = CmplxBaseType( typ1 );
    typ2 = CmplxBaseType( typ2 );
    XPush( CGBinary( op, x.imagpart, y.imagpart, ResCGType( typ1, typ2 ) ) );
    XPush( CGBinary( op, x.realpart, y.realpart, ResCGType( typ1, typ2 ) ) );
}


static void    XMixed( int op, bool cmplx_scalar ) {
//===========================================

// Binary F-Code processor for cmplx-scalar addition & subtraction.
// cx   - true if complex OP scalar, false if scalar OP complex.

    cg_cmplx    z;
    cg_name     x;
    unsigned_16 typ_info;
    cg_type     z_typ;
    cg_type     x_typ;

    typ_info = GetU16();
    if( cmplx_scalar ) {
        z_typ = GetType1( typ_info );
        x_typ = GetType2( typ_info );
        XPopCmplx( &z, z_typ );
        x = XPopValue( x_typ );
    } else {
        x_typ = GetType1( typ_info );
        z_typ = GetType2( typ_info );
        x = XPopValue( x_typ );
        XPopCmplx( &z, z_typ );
    }
    z_typ = CmplxBaseType( z_typ );
    if( cmplx_scalar ) {
        XPush( z.imagpart );
        XPush( CGBinary( op, z.realpart, x, ResCGType( z_typ, x_typ ) ) );
    } else {
        if( op == O_MINUS ) {
            XPush( CGUnary( O_UMINUS, z.imagpart, z_typ ) );
        } else {
            XPush( z.imagpart );
        }
        XPush( CGBinary( op, x, z.realpart, ResCGType( x_typ, z_typ ) ) );
    }
}


static void    XMulDivMix( int op, bool cmplx_scalar, unsigned_16 typ_info ) {
//=====================================================================

// Binary F-Code processor for mixed multiplication and division.

    cg_cmplx    z;
    cg_name     s;
    cg_type     s_typ;
    cg_type     z_typ;
    cg_name     s_1;
    cg_name     s_2;

    if( cmplx_scalar ) {
        z_typ = GetType1( typ_info );
        s_typ = GetType2( typ_info );
        XPopCmplx( &z, z_typ );
        s = XPopValue( s_typ );
    } else {
        s_typ = GetType1( typ_info );
        z_typ = GetType2( typ_info );
        s = XPopValue( s_typ );
        XPopCmplx( &z, z_typ );
    }
    z_typ = ResCGType( s_typ, CmplxBaseType( z_typ ) );
    CloneCGName( s, &s_1, &s_2 );
    XPush( CGBinary( op, z.imagpart, s_1, z_typ ) );
    XPush( CGBinary( op, z.realpart, s_2, z_typ ) );
}


void    FCCmplxFlip( void ) {
//=====================

// Flip 2 complex operands.

    cg_name     rp_1;
    cg_name     ip_1 = NULL;
    cg_name     rp_2;
    cg_name     ip_2 = NULL;

    rp_1 = XPop();
    if( !TypePointer( CGType( rp_1 ) ) ) {
        ip_1 = XPop();
    }
    rp_2 = XPop();
    if( !TypePointer( CGType( rp_2 ) ) ) {
        ip_2 = XPop();
    }
    if( !TypePointer( CGType( rp_1 ) ) ) {
        XPush( ip_1 );
    }
    XPush( rp_1 );
    if( !TypePointer( CGType( rp_2 ) ) ) {
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
    if( !TypePointer( CGType( rp ) ) ) {
        ip = XPop();
    }
    scalar = XPop();
    if( !TypePointer( CGType( rp ) ) ) {
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
    if( !TypePointer( CGType( rp ) ) ) {
        ip = XPop();
    }
    XPush( scalar );
    if( !TypePointer( CGType( rp ) ) ) {
        XPush( ip );
    }
    XPush( rp );
}


void    FCUMinusCmplx( void ) {
//=======================

// Unary minus (-) F-Code processor for complex numbers.

    cg_cmplx    op;
    cg_type     typ;

    typ = GetType( GetU16() );
    XPopCmplx( &op, typ );
    typ = CmplxBaseType( typ );
    XPush( CGUnary( O_UMINUS, op.imagpart, typ ) );
    XPush( CGUnary( O_UMINUS, op.realpart, typ ) );
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

    XCmplxMixOp( RT_C8DIV, false );
}


void    FCMulCmplx( void ) {
//====================

// Multiply one complex number by another.
#if _CPU == 8086 || _CPU == 386
    if( CPUOpts & CPUOPT_FPC ) {
        // generate call to runtime complex multiply
        XCmplxOp( RT_C8MUL );
    } else {
        // do multiplication inline
        InLineMulCC( GetU16() );
    }
#else
    // For risc we just inline the entire code
    InLineMulCC( GetU16() );
#endif
}


static void    InLineMulCC( unsigned_16 typ_info ) {
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
    cg_type     typ1;
    cg_type     typ2;
    cg_cmplx    x;
    cg_cmplx    y;

    typ1 = GetType1( typ_info );
    typ2 = GetType2( typ_info );
    XPopCmplx( &x, typ1 );
    XPopCmplx( &y, typ2 );
    typ1 = CmplxBaseType( typ1 );
    typ2 = CmplxBaseType( typ2 );
    CloneCGName( x.realpart, &a_1, &a_2 );
    CloneCGName( x.imagpart, &b_1, &b_2 );
    CloneCGName( y.realpart, &c_1, &c_2 );
    CloneCGName( y.imagpart, &d_1, &d_2 );
    typ1 = ResCGType( typ1, typ2 );
    XPush( CGBinary( O_PLUS,
                     CGBinary( O_TIMES, a_1, d_1, typ1 ),
                     CGBinary( O_TIMES, b_1, c_1, typ1 ),
                     typ1 ) );
    XPush( CGBinary( O_MINUS,
                     CGBinary( O_TIMES, a_2, c_2, typ1 ),
                     CGBinary( O_TIMES, b_2, d_2, typ1 ),
                     typ1 ) );
}


void    FCDivCmplx( void ) {
//====================

// Binary division for complex numbers.

    XCmplxOp( RT_C8DIV );
}


void    FCExpCmplx( void ) {
//====================

// Binary exponentiation for complex numbers.

    XCmplxOp( RT_C8POW );
}


void    FCExpMixCX( void ) {
//====================

// Binary exponentiation for complex**non-complex.

    XCmplxMixOp( RT_C8POW, true );
}


void    FCExpMixXC( void ) {
//====================

// Binary exponentiation for non-complex**complex.

    XCmplxMixOp( RT_C8POW, false );
}


static cg_type PromoteIntType( cg_type typ ) {
//============================================

    if( ( typ == TY_INT_1 ) || ( typ == TY_INT_2 ) ) {
        typ = TY_INT_4;
    }
    return( typ );
}


static void    DoCmplxScalarOp( RTCODE rtn_id, cg_name a, cg_name b, cg_name s ) {
//=========================================================================

// Do a complex operation.

    call_handle handle;
    cg_type     typ;
    cg_type     r_typ;

    typ = CGType( a );
    if( typ == TY_DOUBLE ) {
        rtn_id += RT_C_DOUBLE;
        r_typ = TY_DCOMPLEX;
    } else if( typ == TY_LONGDOUBLE ) {
        rtn_id += RT_C_EXTENDED;
        r_typ = TY_XCOMPLEX;
    } else {
        r_typ = TY_COMPLEX;
    }
    handle = InitCall( rtn_id );
    CGAddParm( handle, a, typ );
    CGAddParm( handle, b, typ );
    CGAddParm( handle, s, PromoteIntType( CGType( s ) ) );
    SplitCmplx( CGCall( handle ), r_typ );
}


static void    XCmplxMixOp( RTCODE rtn_id, bool cmplx_scalar ) {
//=======================================================

// F-Code processor for binary complex number operations involving
// runtime routines.
// x / (c,d) or (c,d) / x

    unsigned_16 typ_info;
    cg_type     s_typ;
    cg_type     x_typ;
    cg_name     s;
    cg_cmplx    x;

    typ_info = GetU16();
    if( cmplx_scalar ) {
        x_typ = GetType1( typ_info );
        s_typ = GetType2( typ_info );
        XPopCmplx( &x, x_typ );
        s = XPopValue( s_typ );
    } else {
        s_typ = GetType1( typ_info );
        x_typ = GetType2( typ_info );
        s = XPopValue( s_typ );
        XPopCmplx( &x, x_typ );
    }
    x_typ = ResCGType( s_typ, CmplxBaseType( x_typ ) );
    if( cmplx_scalar ) {
        // currently, the only time XCmplxMixOp() is called when the left
        // operand is complex and the right operand is a scalar, is for
        // exponentiation
        s_typ = PromoteIntType( s_typ );
        if( s_typ == TY_INT_4 ) {
            DoCmplxScalarOp( RT_C8POWI, x.realpart, x.imagpart, s );
        } else {
            DoCmplxOp( rtn_id, x.realpart, x.imagpart, s, CGInteger( 0, x_typ ) );
        }
    } else {
        DoCmplxOp( rtn_id, s, CGInteger( 0, x_typ ), x.realpart, x.imagpart );
    }
}


static  void    CCCmp( cg_op op, cg_name a, cg_name b, cg_name c, cg_name d ) {
//=============================================================================

// Complex/Complex compare.

    cg_type     res_type;
    cg_op       flow_op;

    res_type = ResCGType( CGType( a ), CGType( c ) );
    if( op == O_EQ ) {
       flow_op = O_FLOW_AND;
    } else {
       flow_op = O_FLOW_OR;
    }
    XPush( CGFlow( flow_op, CGCompare( op, a, c, res_type ),
                            CGCompare( op, b, d, res_type ) ) );
}


static void    CCCompare( int op ) {
//===========================

// Complex/Complex compare.

    cg_cmplx    x;
    cg_cmplx    y;
    unsigned_16 typ_info;

    typ_info = GetU16();
    XPopCmplx( &x, GetType1( typ_info ) );
    XPopCmplx( &y, GetType2( typ_info ) );
    CCCmp( op, x.realpart, x.imagpart, y.realpart, y.imagpart );
}


static void    XCCompare( int op ) {
//===========================

// Scalar/Complex compare.

    cg_name     x;
    cg_cmplx    z;
    unsigned_16 typ_info;
    cg_type     typ1;

    typ_info = GetU16();
    typ1 = GetType1( typ_info );
    x = XPopValue( typ1 );
    XPopCmplx( &z, GetType2( typ_info ) );
    CCCmp( op, x, CGInteger( 0, typ1 ), z.realpart, z.imagpart );
}


static  void    CXCompare( int op ) {
//===================================

// Complex/Scalar compare.

    cg_name     x;
    cg_cmplx    z;
    unsigned_16 typ_info;
    cg_type     typ2;

    typ_info = GetU16();
    typ2 = GetType2( typ_info );
    XPopCmplx( &z, GetType1( typ_info ) );
    x = XPopValue( typ2 );
    CCCmp( op, z.realpart, z.imagpart, x, CGInteger( 0, typ2 ) );
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


void            CmplxAssign( sym_id sym, cg_type dst_typ, cg_type src_typ ) {
//===========================================================================

// Do complex assignment.

    cg_type     typ;
    cg_name     dest;
    cg_name     dest_1;
    cg_name     dest_2;
    cg_cmplx    z;
    unsigned_16 flags;
    temp_handle tr;
    temp_handle ti;

    flags = sym->u.ns.flags;
    dest = NULL;
    if( (flags & SY_CLASS) == SY_SUBPROGRAM ) {
        // assigning to statement function
        if( !(OZOpts & OZOPT_O_INLINE) ) {
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
    typ = CmplxBaseType( dst_typ );
    if( ( src_typ != TY_COMPLEX ) && ( src_typ != TY_DCOMPLEX ) &&
                                                ( src_typ != TY_XCOMPLEX ) ) {
        z.realpart = XPopValue( src_typ );
        z.imagpart = CGInteger( 0, typ );
    } else {
        XPopCmplx( &z, src_typ );
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
    tr = CGTemp( typ );
    ti = CGTemp( typ );
    CGDone( CGAssign( CGTempName( tr, typ ), z.realpart, typ ) );
    CGDone( CGAssign( CGTempName( ti, typ ), z.imagpart, typ ) );

    CloneCGName( dest, &dest_1, &dest_2 );
    XPush( CGAssign( ImagPtr( dest_2, typ ),
                CGUnary( O_POINTS, CGTempName( ti, typ ), typ ), typ ) );
    XPush( CGAssign( dest_1, CGUnary( O_POINTS, CGTempName( tr, typ ), typ ),
                        typ ) );
}


cg_name         CmplxAddr( cg_name real, cg_name imag ) {
//=======================================================

// Pass a complex value to a subprogram.

    tmp_handle  tmp;
    cg_type     typ;
    cg_type     c_type;

    typ = CGType( real );
    if( typ == TY_SINGLE ) {
        c_type = TY_COMPLEX;
    } else if( typ == TY_DOUBLE ) {
        c_type = TY_DCOMPLEX;
    } else {
        c_type = TY_XCOMPLEX;
    }
    tmp = AllocTmp( c_type );
    CGTrash( CGAssign( TmpPtr( tmp, c_type ), real, typ ) );
    CGTrash( CGAssign( ImagPtr( TmpPtr( tmp, c_type ), typ ), imag, typ ) );
    return( TmpPtr( tmp, c_type ) );
}


void            Cmplx2Scalar( void ) {
//==============================

// Convert complex to scalar.

    cg_name     opn;

    opn = XPop();
    if( !TypePointer( CGType( opn ) ) ) {
        CGTrash( XPop() );
    }
    XPush( opn );
}


void    FCImag( void ) {
//================

    cg_name     opn;
    cg_type     typ;

    typ = CmplxBaseType( GetType( GetU16() ) );
    opn = XPop();
    if( TypePointer( CGType( opn ) ) ) {
        XPush( CGUnary( O_POINTS, ImagPtr( opn, typ ), typ ) );
    } else {
        CGTrash( opn );
    }
}


void    FCConjg( void ) {
//=================

    cg_cmplx    z;
    cg_type     typ;

    typ = GetType( GetU16() );
    XPopCmplx( &z, typ );
    XPush( CGUnary( O_UMINUS, z.imagpart, CmplxBaseType( typ ) ) );
    XPush( z.realpart );
}
