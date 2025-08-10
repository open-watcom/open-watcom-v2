/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Set DOS interrupt vector.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <dos.h>
#include "tinyio.h"
#if !defined( _M_I86 )
    #include "extender.h"
#endif


#if defined( _M_I86 )
    extern  void _setvect( unsigned, void (__interrupt _WCFAR *)());
    #pragma aux _setvect = \
            "push ds"           \
            "mov ds,cx"         \
            "mov ah,25h"        \
            __INT_21            \
            "pop ds"            \
        __parm __caller [__ax] [__cx __dx]
#else
    extern  void pharlap_setvect( unsigned, void (__interrupt _WCFAR *)());
    #pragma aux pharlap_setvect = \
            "push ds"           \
            "mov ds,ecx"        \
            "mov cl,al"         \
            "mov ax,2504h"      \
            __INT_21            \
            "pop ds"            \
        __parm __caller [__al] [__cx __edx]

    extern  void dos4g_setvect( unsigned, void (__interrupt _WCFAR *)());
    #pragma aux dos4g_setvect = \
            "push ds"           \
            "mov ds,ecx"        \
            "mov ah,25h"        \
            __INT_21            \
            "pop ds"            \
        __parm __caller [__al] [__cx __edx]
#endif

_WCRTLINK void _dos_setvect( unsigned intno, void (__interrupt _WCFAR *func)() )
{
#if defined( _M_I86 )
    _setvect( intno, func );
#elif defined(__WINDOWS_386__)
    TinySetVect( intno, (void _WCNEAR *) func );
#elif defined(__386__)
    if( _IsPharLap() ) {
        pharlap_setvect( intno, func );
    } else {        /* DOS/4G style */
        dos4g_setvect( intno, func );
    }
#endif
}
