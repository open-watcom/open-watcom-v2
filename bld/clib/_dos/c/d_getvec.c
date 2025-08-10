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
* Description:  Get DOS interrupt vector.
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
    extern  void (__interrupt _WCFAR *_getvect( unsigned ax ))();
    #pragma aux _getvect = \
            "mov ah,35h"    \
            __INT_21        \
        __parm      [__ax] \
        __value     [__es __bx]
#else
    extern  void (__interrupt _WCFAR *_getvect( unsigned ax, unsigned char cl ))();
    #pragma aux _getvect = \
            "push es"       \
            __INT_21        \
            "mov dx,es"     \
            "pop es"        \
        __parm      [__ax] [__cl] \
        __value     [__dx __ebx] \
        __modify    [__edx]
#endif

_WCRTLINK void (__interrupt _WCFAR *_dos_getvect( unsigned intno ))()
{
#if defined( _M_I86 )
    return( _getvect( intno ) );
#elif defined(__WINDOWS_386__)
    return( TinyGetVect( intno ) );
#else
    if( _IsPharLap() ) {
        return( _getvect( 0x2502, intno ) );
    } else {        /* DOS/4G style */
        return( _getvect( 0x3500 | (intno & 0xff), 0 ) );
    }
#endif
}
