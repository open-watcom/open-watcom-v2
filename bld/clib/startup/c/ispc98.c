/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include <i86.h>
#include <stdlib.h>
#include "tinyio.h"
#include "ispc98.h"
#include "rtinit.h"


#ifdef _M_I86
static unsigned char ispc98( void );
#pragma aux ispc98 = \
        "cld"           \
        "mov ax,1000h"  \
        "int 1Ah"       \
        "jc short L1"   \
        "cmp ax,1000h"  \
        "jz short L1"   \
        "mov al,1"      \
        "jmp short L2"  \
    "L1: mov al,0"      \
    "L2:"               \
    __value [__al]
#else
extern int __dpmi_int1a_call( call_struct *cs );
#pragma aux __dpmi_int1a_call = \
        "push   es"         \
        "mov    eax,ds"     \
        "mov    es,eax"     \
        "xor    ecx,ecx"    \
        "mov    bx,1Ah"     \
        "mov    ax,300h"    \
        "int 31h"           \
        "pop    es"         \
        "sbb    eax,eax"    \
    __parm __caller     [__edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __ecx]
#endif

static int __is_PC98( void )
{
#if defined( _M_I86 )
    return( ispc98() );
#else
    call_struct     dpmi_rm;

    dpmi_rm.flags = 0;
    dpmi_rm.eax = 0x1000;
    if( __dpmi_int1a_call( &dpmi_rm ) || (dpmi_rm.flags & INTR_CF) || dpmi_rm.eax == 0x1000 )
        return( 0 );
    return( 1 );
#endif
}

/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize the __isPC98 global variable.
****/

int     _WCNEAR __isPC98 = -1;  // -1 not yet initialized
                                // 0  IBM PC
                                // 1  NEC PC-98

static void init_on_startup( void )
{
    __isPC98 = __is_PC98();
}


AXI( init_on_startup, INIT_PRIORITY_LIBRARY )
