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


cfstruct            cxxh;

void    FloatSupportInit( void )
/******************************/
{
    cxxh.alloc = CMemAlloc;
    cxxh.free = CMemFree;
    CFInit( &cxxh );
}

void    FloatSupportFini( void )
/******************************/
{
    CFFini( &cxxh );
}

static float_handle makeOK( float_handle f )
/******************************************/
{
    if( CFExp( f ) > 0 ) {
        CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
    } else {
        CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
    }
    CFFree( &cxxh, f );
    f = CFCnvUF( &cxxh, 1 );
    return( f );
}

float_handle CFCheckFloatLimit( float_handle f )
/**********************************************/
{
    if( CFIsFloat( f ) )
        return( f );
    f = makeOK( f );
    return( f );
}

float_handle CFCheckDoubleLimit( float_handle f )
/***********************************************/
{
    if( CFIsDouble( f ) )
        return( f );
    f = makeOK( f );
    return( f );
}

target_long CFFloat2Long( float_handle *f )
/*****************************************/
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
