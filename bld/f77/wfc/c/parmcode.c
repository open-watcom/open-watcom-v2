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
// PARMCODE     : determine argument code
//

#include "ftnstd.h"
#include "opn.h"
#include "global.h"
#include "prmcodes.h"
#include "utility.h"


PCODE   ParmClass( itnode *arg ) {
//===============================

// Return the argument class.

    USOPN       opn;
    unsigned_16 sp_typ;
    unsigned_16 flags;

    flags = arg->flags;
    opn = arg->opn.us & USOPN_WHAT;
    switch( opn ) {
        case USOPN_NNL:
            if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                sp_typ = flags & SY_SUBPROG_TYPE;
                if( sp_typ == SY_FN_OR_SUB ) {
                    return( PC_FN_OR_SUB );
                } else if( sp_typ == SY_SUBROUTINE ) {
                    return( PC_PROCEDURE );
                } else if( sp_typ != SY_STMT_FUNC ) {
                    return( PC_PROCEDURE );
                } else { // result of statement function
                    return( PC_CONST );
                }
            } else {
                return( PC_VARIABLE );
            }
            break;
        case USOPN_NWL:
            if( Subscripted() ) {
                return( PC_ARRAY_ELT );
            } else {
                return( PC_CONST );
            }
            break;
        case USOPN_ARR:
            return( PC_ARRAY_NAME );
            break;
        case USOPN_STN:
            return( PC_STATEMENT );
            break;
        case USOPN_CON:
            return( PC_CONST );
            break;
        case USOPN_ASS:
            return( PC_SS_ARRAY );
            break;
        default:
            return( PC_CONST );
    }
}


PCODE   ParmCode( itnode *arg ) {
//===============================

// Return the argument code.
// We cannot assume that USOPN_SAFE is PC_CONST otherwise we will not be able
// to diagnose an error in the following case (optimizing compiler only).
//      external f
//      print *, sin( f )
// "f" will be USOPN_SAFE by the time ParmCode() is called but we want to return
// PC_FN_OR_SUB.

    USOPN   opn;

    opn = arg->opn.us & USOPN_WHERE;
    if( ( arg->opn.us & USOPN_WHAT ) == USOPN_ARR ) {
        // an array name can't be part of an expression so check it first
        // so that we can detect whether an array has been passed to an
        // intrinsic function
        return( PC_ARRAY_NAME );
    } else if( opn == USOPN_VAL ) {
        return( PC_CONST );
    } else {
        return( ParmClass( arg ) );
    }
}


