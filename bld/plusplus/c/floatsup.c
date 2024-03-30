/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


cfstruct    cxxh;

static float_handle     MaxPosFloat;
static float_handle     MinPosFloat;
static float_handle     MaxPosDbl;
static float_handle     MinPosDbl;
static float_handle     MaxNegFloat;
static float_handle     MinNegFloat;
static float_handle     MaxNegDbl;
static float_handle     MinNegDbl;

void    FloatCheckInit( void )
/****************************/
{
    cxxh.alloc = CMemAlloc;
    cxxh.free = CMemFree;
    CFInit( &cxxh );
    /*
     * constants are extracted from ISO C standard 2.2.4.2.2
     */
    MaxPosFloat = CFCnvSF( &cxxh, "3.40282347e+38" );
    MinPosFloat = CFCnvSF( &cxxh, "1.17549435e-38" );
    MaxPosDbl   = CFCnvSF( &cxxh, "1.7976931348623157e+308" );
    MinPosDbl   = CFCnvSF( &cxxh, "2.2250738585072014e-308" );
    MaxNegFloat = CFCnvSF( &cxxh, "-3.40282347e+38" );
    MinNegFloat = CFCnvSF( &cxxh, "-1.17549435e-38" );
    MaxNegDbl   = CFCnvSF( &cxxh, "-1.7976931348623157e+308" );
    MinNegDbl   = CFCnvSF( &cxxh, "-2.2250738585072014e-308" );
}

void    FloatCheckFini( void )
/****************************/
{
    CFFree( &cxxh, MaxPosFloat );
    CFFree( &cxxh, MinPosFloat );
    CFFree( &cxxh, MaxPosDbl );
    CFFree( &cxxh, MinPosDbl );
    CFFree( &cxxh, MaxNegFloat );
    CFFree( &cxxh, MinNegFloat );
    CFFree( &cxxh, MaxNegDbl );
    CFFree( &cxxh, MinNegDbl );

    CFFini( &cxxh );
}

static float_handle makeOK( float_handle f )
/******************************************/
{
    CFFree( &cxxh, f );
    f = CFCnvUF( &cxxh, 1 );
    return( f );
}


float_handle CFCheckFloatLimit( float_handle f )
/**********************************************/
{
    int sign;
    bool err;

    err = false;
    sign = CFTest( f );
    if( sign > 0 ) {
        if( CFCompare( f, MaxPosFloat ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( CFCompare( f, MinPosFloat ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    } else if( sign < 0 ) {
        if( CFCompare( f, MaxNegFloat ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( CFCompare( f, MinNegFloat ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}

float_handle CFCheckDblLimit( float_handle f )
/********************************************/
{
    int sign;
    bool err;

    err = false;
    sign = CFTest( f );
    if( sign > 0 ) {
        if( CFCompare( f, MaxPosDbl ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( CFCompare( f, MinPosDbl ) == -1 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );

        }
    } else if( sign < 0 ) {
        if( CFCompare( f, MaxNegDbl ) < 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        } else if( CFCompare( f, MinNegDbl ) > 0 ) {
            err = true;
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        }
    }
    if( err ) {
        f = makeOK( f );
    }
    return( f );
}


target_long CFGetLong( float_handle *f )
/**************************************/
{
    float_handle new_f;
    target_long val;

    new_f = CFTrunc( &cxxh, *f );
    CFFree( &cxxh, *f );
    *f = NULL;
    val = CFCnvF32( new_f );
    CFFree( &cxxh, new_f );
    return( val );
}
