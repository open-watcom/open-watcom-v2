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


#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "plusplus.h"
#include "preproc.h"
#include "codegen.h"
#include "errdefns.h"
#include "floatsup.h"
#include "floatlim.h"


static cfloat *makeOK( cfloat *f )
{
    BFFree( f );
    f = BFCnvUF( 1 );
    return( f );
}


cfloat *BFCheckFloatLimit( cfloat *f )
/************************************/
{
    int sign;
    boolean err;

    err = FALSE;
    sign = BFSign( f );
    if( sign > 0 ) {
        if( BFCmp( f, &MaxPosFloat ) > 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, &MinPosFloat ) < 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    } else if( sign < 0 ) {
        if( BFCmp( f, &MaxNegFloat ) < 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, &MinNegFloat ) > 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}

extern cfloat *BFCheckDblLimit( cfloat *f )
{
    int sign;
    boolean err;

    err = FALSE;
    sign = BFSign( f );
    if( sign > 0 ) {
        if( BFCmp( f, &MaxPosDbl ) > 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, &MinPosDbl ) == -1 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );

        }
    } else if( sign < 0 ) {
        if( BFCmp( f, &MaxNegDbl ) < 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, &MinNegDbl ) > 0 ) {
            err = TRUE;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}


target_long BFGetLong( CPP_FLOAT **f )
/************************************/
{
    CPP_FLOAT *new_f;
    target_long val;

    new_f = BFTrunc( *f );
    BFFree( *f );
    *f = NULL;
    val = BFCnvF32( new_f );
    BFFree( new_f );
    return( val );
}
