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
* Description:  integer arithmetic
*
****************************************************************************/

#include "ftnstd.h"
#include "xfflags.h"

#if defined( _M_I86 )  // 16-bit
 extern  bool    __Add( intstar4 *arg1, intstar4 *arg2 );
 extern  bool    __Sub( intstar4 *arg1, intstar4 *arg2 );
 #if defined(__SMALL__) || defined(__MEDIUM__)
  #pragma aux   __Add = \
                "mov    ax,[bx]"   \
                "add    [di],ax"   \
                "mov    ax,2[bx]"  \
                "adc    2[di],ax"  \
                "mov    al,0"      \
                "jno    short L1"  \
                "inc    al"        \
                "L1:"              \
                parm [di] [bx] value [al];
  #pragma aux   __Sub = \
                "mov    ax,[bx]"   \
                "sub    [di],ax"   \
                "mov    ax,2[bx]"  \
                "sbb    2[di],ax"  \
                "mov    al,0"      \
                "jno    short L1"  \
                "inc    al"        \
                "L1:"              \
                parm [di] [bx] value [al];
 #else  // large data models
  #pragma aux   __Add = \
                "push   ds"        \
                "mov    ds,cx"     \
                "push   ax"        \
                "mov    ax,[bx]"   \
                "mov    cx,2[bx]"  \
                "mov    ds,dx"     \
                "pop    bx"        \
                "add    [bx],ax"   \
                "adc    2[bx],cx"  \
                "mov    al,0"      \
                "jno    short L1"  \
                "inc    al"        \
                "L1:pop ds"        \
                parm [dx ax] [cx bx] value [al];
  #pragma aux   __Sub = \
                "push   ds"        \
                "mov    ds,cx"     \
                "push   ax"        \
                "mov    ax,[bx]"   \
                "mov    cx,2[bx]"  \
                "mov    ds,dx"     \
                "pop    bx"        \
                "sub    [bx],ax"   \
                "sbb    2[bx],cx"  \
                "mov    al,0"      \
                "jno    short L1"  \
                "inc    al"        \
                "L1:pop ds"        \
                parm [dx ax] [cx bx] value [al];
 #endif
 extern  bool   ChkI4Mul(intstar4 __far *arg1,intstar4 arg2);
#elif defined( _M_IX86 )
 extern  bool    __Add( intstar4 *arg1, intstar4 *arg2 );
 extern  bool    __Sub( intstar4 *arg1, intstar4 *arg2 );
 #pragma aux    __Add = \
                "mov    edx,[edx]" \
                "add    [eax],edx" \
                "seto   al"        \
                parm [eax] [edx] value [al];
 #pragma aux    __Sub = \
                "mov    edx,[edx]" \
                "sub    [eax],edx" \
                "seto   al"        \
                parm [eax] [edx] value [al];
 extern bool    __Mul( intstar4 *arg1, intstar4 *arg2 );
 #pragma aux    __Mul = \
                "push   ebx"       \
                "mov    edx,[edx]" \
                "mov    ebx,[eax]" \
                "imul   ebx,edx"   \
                "mov    [eax],ebx" \
                "pop    ebx"       \
                "seto   al"        \
                parm [eax] [edx] value [al];
#elif defined( _M_X64 )
bool    __Add( intstar4 *arg1, intstar4 *arg2 )
{
    intstar4  arg1v = *arg1;
    intstar4  arg2v = *arg2;
    intstar4  result = arg1v + arg2v;

    *arg1 = result;
    return( ( result < arg2v ) && ( arg1v >= 0 ) || ( result > arg2v ) && ( arg1v < 0 ) );
}

bool    __Sub( intstar4 *arg1, intstar4 *arg2 )
{
    intstar4  arg1v = *arg1;
    intstar4  arg2v = *arg2;
    intstar4  result = arg1v - arg2v;

    *arg1 = result;
    return( ( result < arg1v ) && ( arg2v < 0 ) || ( result > arg1v ) && ( arg2v >= 0 ) );
}

bool    __Mul( intstar4 *arg1, intstar4 *arg2 )
{
#if _INTEGRAL_MAX_BITS >= 64
    long long arg1v = *arg1;
    long long arg2v = *arg2;
    long long result = arg1v * arg2v;

    *arg1 = result;
    result >>= 31;
    return( result != 0 && result != -1 );
#else
    #error compiler doesn't support 64-bit integral type
#endif
}
#else
 extern  bool    __Add( intstar4 *arg1, intstar4 *arg2 );
 extern  bool    __Sub( intstar4 *arg1, intstar4 *arg2 );
 extern  bool    __Mul( intstar4 *arg1, intstar4 *arg2 );
#endif

bool    AddIOFlo( intstar4 *arg1, intstar4 *arg2 ) {
//==================================================

// Add two integers and check for overflow.
#if 0
    __XcptFlags &= ~XF_IOVERFLOW;
    *arg1 += *arg2;
    ChkOverFlow();
    return( ( __XcptFlags & XF_IOVERFLOW ) != 0 );
#else
    return( __Add( arg1, arg2 ) );
#endif
}


bool    SubIOFlo( intstar4 *arg1, intstar4 *arg2 ) {
//==================================================

// Subtract two integers and check for overflow.
#if 0
    __XcptFlags &= ~XF_IOVERFLOW;
    *arg1 -= *arg2;
    ChkOverFlow();
    return( ( __XcptFlags & XF_IOVERFLOW ) != 0 );
#else
    return( __Sub( arg1, arg2 ) );
#endif
}


bool    MulIOFlo( intstar4 *arg1, intstar4 *arg2 ) {
//==================================================

// Multiply two integers and check for overflow.
#if 0
    __XcptFlags &= ~XF_IOVERFLOW;
  #ifdef _M_I86
    *arg1 = ChkI4Mul( *arg1, *arg2 );
  #else
    *arg1 *= *arg2;
    ChkOverFlow();
  #endif
    return( ( __XcptFlags & XF_IOVERFLOW ) != 0 );
#else
  #ifdef _M_I86
    return( ChkI4Mul( arg1, *arg2 ) );
  #else
    return( __Mul( arg1, arg2 ) );
  #endif
#endif
}
