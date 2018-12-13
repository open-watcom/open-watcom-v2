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


#include "variety.h"
#include <i86.h>


extern  unsigned short  _CS( void );
#pragma aux _CS = 0x8C 0xC8 /* mov ax,cs */ __value [__ax]
extern  unsigned short  _DS( void );
#pragma aux _DS = 0x8C 0xD8 /* mov ax,ds */ __value [__ax]
extern  unsigned short  _ES( void );
#pragma aux _ES = 0x8C 0xC0 /* mov ax,es */ __value [__ax]
extern  unsigned short  _SS( void );
#pragma aux _SS = 0x8C 0xD0 /* mov ax,ss */ __value [__ax]
#if defined(__386__)
extern  unsigned short  _FS( void );
#pragma aux _FS = 0x8C 0xE0 /* mov ax,fs */ __value [__ax]
extern  unsigned short  _GS( void );
#pragma aux _GS = 0x8C 0xE8 /* mov ax,gs */ __value [__ax]
#endif


_WCRTLINK void segread( struct SREGS *segregs )
{
    __ptr_check( segregs, 0 );
    segregs->cs = _CS();
    segregs->ds = _DS();
    segregs->es = _ES();
    segregs->ss = _SS();
#if defined(__386__)
    segregs->fs = _FS();
    segregs->gs = _GS();
#endif
}
