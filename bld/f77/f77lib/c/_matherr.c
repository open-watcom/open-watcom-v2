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
* Description:  math error handler
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "fmath.h"
#include "mathcode.h"
#include "errcod.h"

intstar4 __imath2err( unsigned int err_info, intstar4 *a1, intstar4 *a2 ) {
//=========================================================================

    a1 = a1; a2 = a2;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_POW:
            RTErr( EX_Z_2_NOT_POS );
            break;
        case FUNC_MOD:
            RTErr( LI_ARG_ZERO );
            break;
        }
    }
    RTErr( CP_TERMINATE );
    return( 0 );
}


single  __amath1err( unsigned int err_info, single *a1 ) {
//========================================================

    a1 = a1;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_COTAN:
            RTErr( KO_FOVERFLOW );
            break;
        }
    }
    RTErr( CP_TERMINATE );
    return( 0.0 );
}


single  __amath2err( unsigned int err_info, single *a1, single *a2 ) {
//====================================================================

    a1 = a1; a2 = a2;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_MOD:
            RTErr( LI_ARG_ZERO );
            break;
        }
    }
    RTErr( CP_TERMINATE );
    return( 0.0 );
}


double  __math2err( unsigned int err_info, double *a1, double *a2 ) {
//===================================================================

    a2 = a2;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_SQRT:
            RTErr( LI_ARG_NEG );
            break;
        case FUNC_ASIN:
        case FUNC_ACOS:
            RTErr( LI_ARG_LE_ONE );
            break;
        case FUNC_ATAN2:
            RTErr( LI_ARG_ZERO );
            break;
        case FUNC_POW:
            if( *a1 == 0.0 ) { // 0.0**y, y < 0
                RTErr( EX_Z_2_NOT_POS );
            } else { // base < 0 and non-integer power
                RTErr( EX_NOT_INT_ARG );
            }
            break;
        case FUNC_DPOWI:
        case FUNC_IPOW:
            RTErr( EX_Z_2_NOT_POS );
            break;
        case FUNC_LOG:
        case FUNC_LOG10:
            RTErr( LI_ARG_GT_ZERO );
            break;
        case FUNC_MOD:
            RTErr( LI_ARG_ZERO );
            break;
        case FUNC_COTAN:
            RTErr( KO_FOVERFLOW );
            break;
        }
    } else if( err_info & M_SING ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_LOG:
        case FUNC_LOG10:
            RTErr( LI_ARG_ZERO );
            break;
        }
    } else if( err_info & M_OVERFLOW ) {
        RTErr( KO_FOVERFLOW );
    } else if( err_info & M_UNDERFLOW ) {
        return( 0.0 );
    }
    RTErr( CP_TERMINATE );
    return( 0.0 );
}


double  __math1err( unsigned int err_info, double *a1 ) {
//=======================================================

    return( __math2err( err_info, a1, a1 ) );
}


complex __zmath2err( unsigned int err_info, complex *a1, complex *a2 ) {
//======================================================================

    complex     res;

    a1 = a1;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_POW:
            if( a2->imagpart != 0 ) {
                RTErr( EX_CZ_2_NOT_REAL );
            } else {
                RTErr( EX_Z_2_NOT_POS );
            }
            break;
        }
    }
    RTErr( CP_TERMINATE );
    res.realpart = 0.0;
    res.imagpart = 0.0;
    return( res );
}


dcomplex __qmath2err( unsigned int err_info, dcomplex *a1, dcomplex *a2 ) {
//=========================================================================

    dcomplex    res;

    a1 = a1;
    if( err_info & M_DOMAIN ) {
        switch( err_info & FUNC_MASK ) {
        case FUNC_POW:
            if( a2->imagpart != 0 ) {
                RTErr( EX_CZ_2_NOT_REAL );
            } else {
                RTErr( EX_Z_2_NOT_POS );
            }
            break;
        }
    }
    RTErr( CP_TERMINATE );
    res.realpart = 0.0;
    res.imagpart = 0.0;
    return( res );
}
