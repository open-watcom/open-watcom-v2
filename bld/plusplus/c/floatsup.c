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


#include "plusplus.h"

#include <stdarg.h>

#include "preproc.h"
#include "codegen.h"
#include "floatsup.h"
#include "floatlim.h"


static float_handle makeOK( float_handle f )
{
    BFFree( f );
    f = BFCnvUF( 1 );
    return( f );
}


float_handle BFCheckFloatLimit( float_handle f )
/************************************/
{
    int sign;
    bool err;

    err = false;
    sign = BFSign( f );
    if( sign > 0 ) {
        if( BFCmp( f, (float_handle)&MaxPosFloat ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, (float_handle)&MinPosFloat ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    } else if( sign < 0 ) {
        if( BFCmp( f, (float_handle)&MaxNegFloat ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, (float_handle)&MinNegFloat ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}

extern float_handle BFCheckDblLimit( float_handle f )
{
    int sign;
    bool err;

    err = false;
    sign = BFSign( f );
    if( sign > 0 ) {
        if( BFCmp( f, (float_handle)&MaxPosDbl ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, (float_handle)&MinPosDbl ) == -1 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );

        }
    } else if( sign < 0 ) {
        if( BFCmp( f, (float_handle)&MaxNegDbl ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( BFCmp( f, (float_handle)&MinNegDbl ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}


target_long BFGetLong( float_handle *f )
/**************************************/
{
    float_handle new_f;
    target_long val;

    new_f = BFTrunc( *f );
    BFFree( *f );
    *f = NULL;
    val = BFCnvF32( new_f );
    BFFree( new_f );
    return( val );
}
