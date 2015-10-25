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
// FCCNVT    : conversion F-Code processor
//

#include "ftnstd.h"
#include "wf77defs.h"
#include "wf77cg.h"
#include "emitobj.h"
#include "fctypes.h"
#include "fcjmptab.h"
#include "fcstring.h"
#include "fccmplx.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"



void    FCIChar( void ) {
//=================

    XPush( CGUnary( O_CONVERT,
                    CGUnary( O_POINTS, SCBPointer( XPop() ), TY_UINT_1 ),
                    TY_UINT_4 ) );
}


void    FCConvert( void ) {
//===================

// Perform type conversion.

    unsigned_16 typ_info;
    cg_type     from;
    cg_type     to;
    cg_cmplx    z;

    typ_info = GetU16();
    from = GetType1( typ_info );
    to = GetType2( typ_info );
    if( from == TY_COMPLEX ) {
        XPopCmplx( &z, TY_COMPLEX );
        if( to == TY_DCOMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_DOUBLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_DOUBLE ) );
        } else if( to == TY_COMPLEX ) {
            XPush( z.imagpart );
        } else if( to == TY_XCOMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_LONGDOUBLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_LONGDOUBLE ) );
        } else {
            CGTrash( z.imagpart );
            if( to != TY_SINGLE ) {
                z.realpart = CGUnary( O_CONVERT, z.realpart, to );
            }
        }
        XPush( z.realpart );
    } else if( from == TY_DCOMPLEX ) {
        XPopCmplx( &z, TY_DCOMPLEX );
        if( to == TY_COMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_SINGLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_SINGLE ) );
        } else if( to == TY_DCOMPLEX ) {
            XPush( z.imagpart );
        } else if( to == TY_XCOMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_LONGDOUBLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_LONGDOUBLE ) );
        } else {
            CGTrash( z.imagpart );
            if( to != TY_DOUBLE ) {
                z.realpart = CGUnary( O_CONVERT, z.realpart, to );
            }
        }
        XPush( z.realpart );
    } else if( from == TY_XCOMPLEX ) {
        XPopCmplx( &z, TY_XCOMPLEX );
        if( to == TY_COMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_SINGLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_SINGLE ) );
        } else if( to == TY_DCOMPLEX ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, TY_DOUBLE );
            XPush( CGUnary( O_CONVERT, z.imagpart, TY_DOUBLE ) );
        } else if( to == TY_XCOMPLEX ) {
            XPush( z.imagpart );
        } else {
            CGTrash( z.imagpart );
            if( to != TY_LONGDOUBLE ) {
                z.realpart = CGUnary( O_CONVERT, z.realpart, to );
            }
        }
        XPush( z.realpart );
    } else { // converting a scalar
        z.realpart = XPopValue( from );
        if( to == TY_COMPLEX ) {
            to = TY_SINGLE;
            XPush( CGInteger( 0, to ) );
        } else if( to == TY_DCOMPLEX ) {
            to = TY_DOUBLE;
            XPush( CGInteger( 0, to ) );
        } else if( to == TY_XCOMPLEX ) {
            to = TY_LONGDOUBLE;
            XPush( CGInteger( 0, to ) );
        }
        if( to != from ) {
            z.realpart = CGUnary( O_CONVERT, z.realpart, to );
        }
        XPush( z.realpart );
    }
}


static  void    MakeComplex( cg_type to ) {
//=========================================

// Make a COMPLEX/COMPLEX*16/COMPLEX*20 number from 2 scalars.

    cg_type     arg_type;
    cg_name     real_part;
    cg_name     imag_part;

    arg_type = GetType( GetU16() );
    real_part = XPopValue( arg_type );
    imag_part = XPopValue( arg_type );
    if( arg_type != to ) {
        real_part = CGUnary( O_CONVERT, real_part, to );
        imag_part = CGUnary( O_CONVERT, imag_part, to );
    }
    XPush( imag_part );
    XPush( real_part );
}


void            FCMakeComplex( void ) {
//===============================

// Make a COMPLEX number from 2 scalars.

    MakeComplex( TY_SINGLE );
}


void            FCMakeDComplex( void ) {
//================================

// Make a DCOMPLEX number from 2 scalars.

    MakeComplex( TY_DOUBLE );
}


void            FCMakeXComplex( void ) {
//================================

// Make a XCOMPLEX number from 2 scalars.

    MakeComplex( TY_LONGDOUBLE );
}


