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
* Description:  Implementation of _fstrset() - far strset().
*
****************************************************************************/


#include "variety.h"
#include <string.h>


#ifdef _M_I86

extern char _WCFAR *fast_strset( char _WCFAR *, char );
#pragma aux fast_strset = \
        "push ds"           \
        "push si"           \
        "push ax"           \
        "mov  ds,dx"        \
        "mov  si,ax"        \
        "mov  bh,bl"        \
    "L1: lodsw"             \
        "test al,al"        \
        "je short L3"       \
        "test ah,ah"        \
        "je short L2"       \
        "mov  -2[si],bx"    \
        "jmp short L1"      \
    "L2: mov  -2[si],bl"    \
    "L3: pop  ax"           \
        "pop  si"           \
        "pop  ds"           \
    __parm __caller     [__dx __ax] [__bl] \
    __value             [__dx __ax] \
    __modify __exact    [__bh]

#endif

_WCRTLINK char _WCFAR *_fstrset( char _WCFAR *s, int c )
{
//#ifdef _M_I86
    //return( fast_strset( s, c ) );
//#else
    char _WCFAR     *p;

    for( p = s; *p; ++p ) {
        *p = c;
    }
    return( s );
//#endif
}
