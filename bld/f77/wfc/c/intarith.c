/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  integer arithmetic
*
****************************************************************************/


#include "ftnstd.h"
#include "intarith.h"


#if defined( __WATCOMC__ ) && defined( _M_IX86 )
 extern bool __Add( intstar4 *arg1, intstar4 *arg2 );
 #pragma aux __Add = \
        "mov  edx,[edx]"    \
        "add  [eax],edx"    \
        "seto al"           \
    __parm      [__eax] [__edx] \
    __value     [__al] \
    __modify    []
 extern bool __Sub( intstar4 *arg1, intstar4 *arg2 );
 #pragma aux __Sub = \
        "mov  edx,[edx]"    \
        "sub  [eax],edx"    \
        "seto al"           \
    __parm      [__eax] [__edx] \
    __value     [__al] \
    __modify    []
 extern bool __Mul( intstar4 *arg1, intstar4 *arg2 );
 #pragma aux __Mul = \
        "push ebx"          \
        "mov  edx,[edx]"    \
        "mov  ebx,[eax]"    \
        "imul ebx,edx"      \
        "mov  [eax],ebx"    \
        "pop  ebx"          \
        "seto al"           \
    __parm      [__eax] [__edx] \
    __value     [__al] \
    __modify    []
#else
static bool __Add( intstar4 *arg1, intstar4 *arg2 )
{
    intstar4  arg1v = *arg1;
    intstar4  arg2v = *arg2;
    intstar4  result = arg1v + arg2v;

    *arg1 = result;
    return( ( result < arg2v ) && ( arg1v >= 0 ) || ( result > arg2v ) && ( arg1v < 0 ) );
}

static bool __Sub( intstar4 *arg1, intstar4 *arg2 )
{
    intstar4  arg1v = *arg1;
    intstar4  arg2v = *arg2;
    intstar4  result = arg1v - arg2v;

    *arg1 = result;
    return( ( result < arg1v ) && ( arg2v < 0 ) || ( result > arg1v ) && ( arg2v >= 0 ) );
}

static bool __Mul( intstar4 *arg1, intstar4 *arg2 )
{
#if _INTEGRAL_MAX_BITS >= 64
    long long arg1v = *arg1;
    long long arg2v = *arg2;
    long long result = arg1v * arg2v;

    *arg1 = result;
    result >>= 31;
    return( result != 0 && result != -1 );
#else
    #error "compiler doesn't support 64-bit integral type"
#endif
}
#endif

bool    AddIOFlo( intstar4 *arg1, intstar4 *arg2 )
//================================================
// Add two integers and check for overflow.
{
    return( __Add( arg1, arg2 ) );
}


bool    SubIOFlo( intstar4 *arg1, intstar4 *arg2 )
//================================================
// Subtract two integers and check for overflow.
{
    return( __Sub( arg1, arg2 ) );
}


bool    MulIOFlo( intstar4 *arg1, intstar4 *arg2 )
//================================================
// Multiply two integers and check for overflow.
{
    return( __Mul( arg1, arg2 ) );
}
