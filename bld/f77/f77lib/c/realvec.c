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


//
// REALVEC      : real-mode vector handling for Phar Lap
//

#include <dos.h>
#include "realvec.h"


extern unsigned long _getrealvect(char);
#pragma aux _getrealvect = \
                "mov    ax,2503h" \
                "int    21h" \
                parm [cl] \
                value [ebx] \
                modify exact [ax ebx]

extern void _setrealvect(char,unsigned long);
#pragma aux _setrealvect = \
                "mov    ax,2505h" \
                "int    21h" \
                parm [cl] [ebx] \
                modify exact [ax]

extern void _setvectp(unsigned short,void (__interrupt __far *)(void));
#pragma aux _setvectp = \
                "push   ds" \
                "mov    ds,cx" \
                "mov    cl,al" \
                "mov    ax,2506h" \
                "int    21h" \
                "pop    ds" \
                parm [al] [cx edx] \
                modify exact [ax cl]


unsigned long   _dos_getrealvect( int intnum ) {
//==============================================

    return( _getrealvect( intnum ) );
}


void    _dos_setrealvect( int intnum, unsigned long func ) {
//==========================================================

    _setrealvect( intnum, func );
}


void    _dos_setvectp( int intnum, void (__interrupt __far *func)(void) ) {
//=================================================================

    _setvectp( intnum, func );
}
