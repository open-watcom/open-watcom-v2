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


#include "gdefn.h"
#include "dummy.h"

// This file contains definitions used to create the
// standalone graphics library GRAPH.LIB and GRAPH386.LIB

short           _small_code_;
short           _fltused_;

#pragma aux     _small_code_    "_small_code_";
#pragma aux     _fltused_       "_fltused_";
#pragma aux     F4DivZero       "F4DivZero";
#pragma aux     F4OverFlow      "F4OverFlow";
#pragma aux     F4UnderFlow     "F4UnderFlow";
#if 0
#pragma aux     F8DivZero       "F8DivZero";
#pragma aux     F8OverFlow      "F8OverFlow";
#pragma aux     F8UnderFlow     "F8UnderFlow";
#endif

extern float F4RetInf( float );
#if defined( _M_I86 )
#pragma aux F4RetInf = \
        "and  ax,8000H" \
        "or   ax,7f7fH" \
        "mov  dx,-1" \
        "xchg ax,dx" \
    __parm __caller [__ax __dx] \
    __value         [__ax __dx]
#else
#pragma aux F4RetInf = \
        "and  eax,80000000H" \
        "or   eax,7f7fffffH" \
    __parm __caller [__eax] \
    __value         [__eax]
#endif

#if 0
extern double   F8RetInf( double );
#if defined( _M_I86 )
#pragma aux F8RetInf = \
        "and  ax,8000H" \
        "or   ax,7fefH" \
        "mov  bx,-1"    \
        "mov  cx,bx"    \
        "mov  dx,bx"    \
    __parm __caller [__ax __bx __cx __dx] \
    __value         [__ax __bx __cx __dx]
#else
#pragma aux F8RetInf = \
        "and  eax,80000000H"    \
        "or   eax,7fefffffH"    \
        "mov  edx,-1"           \
        "xchg eax,edx"          \
    __parm __caller [__eax __edx] \
    __value         [__eax __edx]
#endif
#endif


float _WCI86FAR F4UnderFlow( void )
//============================

{
    return( 0 );
}

float _WCI86FAR F4DivZero( float num )
//===============================

{
    return( F4RetInf( num ) );
}

float _WCI86FAR F4OverFlow( float num )
//================================

{
    return( F4RetInf( num ) );
}

#if 0
double _WCI86FAR F8UnderFlow( void )
//=============================

{
    return( 0 );
}

double _WCI86FAR F8DivZero( double num )
//=================================

{
    return( F8RetInf( num ) );
}

double _WCI86FAR F8OverFlow( double num )
//==================================

{
    return( F8RetInf( num ) );
}
#endif
