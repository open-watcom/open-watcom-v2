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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _BIOSUI_H_
#define _BIOSUI_H_

#include "extender.h"

#define _INT_21        "int 21h"
#define _INT_33        "int 33h"

#define VECTOR_MOUSE    0x33

#define MOUSE_DRIVER_OK     ((unsigned short)-1)


struct mouse_data {
    unsigned    bx;
    unsigned    cx;
    unsigned    dx;
};
typedef struct mouse_data __based( __segname( "_STACK" ) ) *md_stk_ptr;


extern unsigned short MouseDrvReset( void );
#pragma aux MouseDrvReset = \
        "xor  ax,ax"        \
        _INT_33             \
    __parm      [] \
    __value     [__ax] \
    __modify    [__ax __bx]

extern void MouseDrvCallRetState( unsigned short, md_stk_ptr );
#ifdef _M_I86
#pragma aux MouseDrvCallRetState = \
        _INT_33                 \
        "mov ss:[si+0],bx"      \
        "mov ss:[si+2],cx"      \
        "mov ss:[si+4],dx"      \
    __parm      [__ax] [__si] \
    __value     \
    __modify    [__bx __cx __dx]
#else
#pragma aux MouseDrvCallRetState = \
        _INT_33                 \
        "mov ss:[esi+0],bx"     \
        "mov ss:[esi+2],cx"     \
        "mov ss:[esi+4],dx"     \
    __parm      [__ax] [__esi] \
    __value     \
    __modify    [__bx __cx __dx]
#endif
extern unsigned short MouseDrvCall1( unsigned short );
#pragma aux MouseDrvCall1 = \
        _INT_33             \
    __parm      [__ax] \
    __value     [__ax] \
    __modify    [__cx __dx]

extern unsigned short MouseDrvCall2( unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall2 = \
        _INT_33             \
    __parm      [__ax] [__bx] [__cx] [__dx] \
    __value     [__ax] \
    __modify    []

extern void MouseDrvCall3( unsigned short, unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall3 = \
        _INT_33             \
    __parm      [__ax] [__cx] [__dx] [__si] [__di] \
    __value     \
    __modify    []

extern unsigned short MouseDrvCall4( unsigned short );
#pragma aux MouseDrvCall4 = \
        _INT_33             \
    __parm      [__ax] \
    __value     [__ax] \
    __modify    [__bx __cx __dx]


#endif // _BIOSUI_H_
