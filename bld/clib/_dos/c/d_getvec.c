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
* Description:  Get DOS interrupt vector.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <dos.h>


#if defined(__386__)
  #if defined(__WINDOWS_386__)
    #include "tinyio.h"
  #else
    #include "extender.h"
    extern  void (__interrupt _WCFAR *_getvect( unsigned ax, unsigned char cl ))();
    #pragma aux  _getvect =     \
            "push es"           \
            "int 21h"           \
            "mov dx,es"         \
            "pop es"            \
            parm [ax] [cl] value [dx ebx] modify [edx];
  #endif
#else
    extern  void (__interrupt _WCFAR *_getvect( unsigned ax ))();
    #pragma aux  _getvect = \
        "mov ah,35h"        \
        "int 21h"           \
        parm [ax] value [es bx];
#endif

_WCRTLINK void (__interrupt _WCFAR *_dos_getvect( unsigned intnum ))()
{
#if defined(__386__)
  #if defined(__WINDOWS_386__)
    return( TinyGetVect( intnum ) );
  #else
    if( _IsPharLap() ) {
        return( _getvect( 0x2502, intnum ) );
    } else {        /* DOS/4G style */
        return( _getvect( 0x3500 | (intnum & 0xff), 0 ) );
    }
  #endif
#else
    return( _getvect( intnum ) );
#endif
}
