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
#include "realmod.h"


#if 0

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

#else

#define is_date_char( c )       (( c >= '0' && c <= '9' ) || c == '/' )
#define BIOS_DATE_LEN           (2+1+2+1+2) /* yy-mm-dd */

#define CHECK_IT                                                \
    {                                                           \
        int i;                                                  \
        unsigned num_ok;                                        \
                                                                \
        /* check for IBM BIOS revsion date in ROM */            \
        num_ok = 0;                                             \
        for( i = 0; i < BIOS_DATE_LEN; ++i ) {                  \
            if( is_date_char( p[i] ) ) {                        \
                ++num_ok;                                       \
            }                                                   \
        }                                                       \
        /* wishy-washy test for BIOS dates that */              \
        /* contain some garbage chars. */                       \
        /* Commodore PC60-40 has BIOS date "02/0(/88"). */      \
        return( num_ok < (BIOS_DATE_LEN / 2) );                 \
    }


static int __is_PC98( void )
{
#if defined(__WINDOWS_386__)
    extern      unsigned short  __F000;
    char        _WCFAR *p;

    p = MK_FP( __F000, 0xfff5 );
    CHECK_IT
#elif defined(__WINDOWS__)
    extern char _WCFAR  _F000h[];
    char _WCFAR *       p;

    p = MK_FP( _F000h, 0xfff5 );
    CHECK_IT
#elif defined(__DOS__) && defined(__386__)
    char _WCFAR *p;

    if( _ExtenderRealModeSelector == 0 )
        return( 0 );
    p = MK_FP( _ExtenderRealModeSelector, 0xffff5 );
    CHECK_IT
#elif defined(__DOS__)
    char _WCFAR *p;

    p = MK_FP( 0xf000, 0xfff5 );
    CHECK_IT
#else
    return( 0 );
#endif
}

#endif

/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize the __isPC98 global variable.
****/

int     _WCNEAR __isPC98 = 0;  // -1 not yet initialized
                                // 0  IBM PC
                                // 1  NEC PC-98

static void init_on_startup( void )
{
    __isPC98 = __is_PC98();
}


AXI( init_on_startup, INIT_PRIORITY_LIBRARY )
