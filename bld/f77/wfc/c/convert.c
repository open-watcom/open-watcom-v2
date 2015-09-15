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
// CONVERT   : numeric conversion routines
//

#include "ftnstd.h"
#include "global.h"
#include "convert.h"


static  void    CnI2I( itnode *it, uint size ) {
//==============================================

    if( it->size == size ) return;
    if( it->size == sizeof( intstar1 ) ) {
        if( size == sizeof( intstar2 ) ) {
            if( it->is_unsigned ) {
                it->value.intstar2 = (unsigned char)it->value.intstar1;
            } else {
                it->value.intstar2 = it->value.intstar1;
            }
        } else {
            if( it->is_unsigned ) {
                it->value.intstar4 = (unsigned char)it->value.intstar1;
            } else {
                it->value.intstar4 = it->value.intstar1;
            }
        }
    } else if( it->size == sizeof( intstar2 ) ) {
        if( size == sizeof( intstar1 ) ) {
            it->value.intstar1 = it->value.intstar2;
        } else {
            if( it->is_unsigned ) {
                it->value.intstar4 = (unsigned short)it->value.intstar2;
            } else {
                it->value.intstar4 = it->value.intstar2;
            }
        }
    } else {
        if( size == sizeof( intstar1 ) ) {
            it->value.intstar1 = it->value.intstar4;
        } else {
            it->value.intstar2 = it->value.intstar4;
        }
    }
}


static  void    CnR2I( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.intstar4 = it->value.single;
}


static  void    CnD2I( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.intstar4 = it->value.dble;
}


static  void    CnE2I( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.intstar4 = it->value.extended;
}


static  void    CnI2R( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.single = it->value.intstar4;
}


static  void    CnD2R( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.single = it->value.dble;
}


static  void    CnE2R( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.single = it->value.extended;
}


static  void    CnI2D( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dble = it->value.intstar4;
}


static  void    CnR2D( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dble = it->value.single;
}


static  void    CnE2D( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dble = it->value.extended;
}


static  void    CnI2E( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.extended = it->value.intstar4;
}


static  void    CnR2E( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.extended = it->value.single;
}


static  void    CnD2E( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.extended = it->value.dble;
}


static  void    CnI2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.scomplex.realpart = it->value.intstar4;
    it->value.scomplex.imagpart = 0;
}


static  void    CnR2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.scomplex.imagpart = 0;
}


static  void    CnD2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.scomplex.realpart = it->value.dble;
    it->value.scomplex.imagpart = 0;
}


static  void    CnE2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.scomplex.realpart = it->value.extended;
    it->value.scomplex.imagpart = 0;
}


static  void    CnQ2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.realpart = it->value.scomplex.realpart;
    it->value.dcomplex.imagpart = it->value.scomplex.imagpart;
}


static  void    CnX2C( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.realpart = it->value.xcomplex.realpart;
    it->value.dcomplex.imagpart = it->value.xcomplex.imagpart;
}


static  void    CnI2Q( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.realpart = it->value.intstar4;
    it->value.dcomplex.imagpart = 0;
}


static  void    CnR2Q( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.realpart = it->value.single;
    it->value.dcomplex.imagpart = 0;
}


static  void    CnD2Q( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.imagpart = 0;
}


static  void    CnE2Q( itnode *it, uint dummy ) {
//===================================

    dummy = dummy;
    it->value.dcomplex.realpart = it->value.extended;
    it->value.dcomplex.imagpart = 0;
}


static  void    CnC2Q( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.dcomplex.imagpart = it->value.scomplex.imagpart;
    it->value.dcomplex.realpart = it->value.scomplex.realpart;
}


static  void    CnX2Q( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.dcomplex.imagpart = it->value.xcomplex.imagpart;
    it->value.dcomplex.realpart = it->value.xcomplex.realpart;
}


static  void    CnI2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.intstar4;
    it->value.xcomplex.imagpart = 0;
}


static  void    CnR2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.single;
    it->value.xcomplex.imagpart = 0;
}


static  void    CnD2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.dble;
    it->value.xcomplex.imagpart = 0;
}


static  void    CnE2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.extended;
    it->value.xcomplex.imagpart = 0;
}


static  void    CnC2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.scomplex.imagpart;
    it->value.xcomplex.realpart = it->value.scomplex.realpart;
}


static  void    CnQ2X( itnode *it, uint dummy ) {
//====================================

    dummy = dummy;
    it->value.xcomplex.imagpart = it->value.dcomplex.imagpart;
    it->value.xcomplex.realpart = it->value.dcomplex.realpart;
}


static  void    NullRtn( itnode *dummy, uint dumy ) {
//===============================

    dummy = dummy;
    dumy = dumy;
}

//
// CnvToTab -- Indexed by the operand types & desired type
//

#define CONV_TAB_ROWS   9
#define CONV_TAB_COLS   9
#define CONV_TAB_SIZE   81

// Notes:
//
// 1. Converting COMPLEX to INTEGER  <===> REAL     to INTEGER
// 2. Converting DCMPLX  to INTEGER  <===> DOUBLE   to INTEGER
// 3. Converting XCMPLX  to INTEGER  <===> EXTENDED to INTEGER
// 4. Converting COMPLEX to REAL     <===> REAL     to REAL     <===> NOP
// 5. Converting DCMPLX  to REAL     <===> DOUBLE   to REAL
// 6. Converting XCMPLX  to REAL     <===> EXTENDED to REAL
// 7. Converting COMPLEX to DOUBLE   <===> REAL     to DOUBLE
// 8. Converting DCMPLX  to DOUBLE   <===> DOUBLE   to DOUBLE   <===> NOP
// 9. Converting DCMPLX  to DOUBLE   <===> EXTENDED to DOUBLE
// 10 Converting XCMPLX  to EXTENDED <===> EXTENDED to EXTENDED <===> NOP

static  void    (* const __FAR CnvToTab[])( itnode *, uint ) = {                  //from/
                                                                                  //   /
// int  int_2   int_1   real     double    extended  complex   dcomplex  xcomplex // / to
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                                                                                   //
&CnI2I, &CnI2I, &CnI2I, &CnR2I,   &CnD2I,   &CnE2I,   &CnR2I,   &CnD2I,   &CnE2I,  // int
&CnI2I, &CnI2I, &CnI2I, &CnR2I,   &CnD2I,   &CnE2I,   &CnR2I,   &CnD2I,   &CnE2I,  // int2
&CnI2I, &CnI2I, &CnI2I, &CnR2I,   &CnD2I,   &CnE2I,   &CnR2I,   &CnD2I,   &CnE2I,  // int1
&CnI2R, &CnI2R, &CnI2R, &NullRtn, &CnD2R,   &CnE2R,   &NullRtn, &CnD2R,   &CnE2R,  // real
&CnI2D, &CnI2D, &CnI2D, &CnR2D,   &NullRtn, &CnE2D,   &CnR2D,   &NullRtn, &CnE2D,  // dble
&CnI2E, &CnI2E, &CnI2E, &CnR2E,   &CnD2E,   &NullRtn, &CnR2D,   &CnD2E,   &NullRtn,// extended
&CnI2C, &CnI2C, &CnI2C, &CnR2C,   &CnD2C,   &CnE2C,   &NullRtn, &CnQ2C,   &CnX2C,  // cmplx
&CnI2Q, &CnI2Q, &CnI2Q, &CnR2Q,   &CnD2Q,   &CnE2Q,   &CnC2Q,   &NullRtn, &CnX2Q,  // dcmplx
&CnI2X, &CnI2X, &CnI2X, &CnR2X,   &CnD2X,   &CnE2X,   &CnC2X,   &CnQ2X,   &NullRtn // xcmplx
};


void    CnvTo( itnode *itptr, TYPE typ, uint size ) {
//=================================================

// Convert itnode to desired numeric type.

    CnvToTab[ ( ( typ - FT_INTEGER_1 ) * CONV_TAB_COLS )
           + ( itptr->typ - FT_INTEGER_1 ) ]( itptr, size );
    itptr->typ = typ;
    itptr->size = size;
}
