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


#ifdef _M_I86
extern  unsigned short  _CS( void );
#pragma aux _CS = "mov ax,cs" __value [__ax]
extern  unsigned short  _DS( void );
#pragma aux _DS = "mov ax,ds" __value [__ax]
extern  unsigned short  _ES( void );
#pragma aux _ES = "mov ax,es" __value [__ax]
extern  unsigned short  _SS( void );
#pragma aux _SS = "mov ax,ss" __value [__ax]
#elif defined( _M_IX86 )
extern  unsigned short  _CS( void );
#pragma aux _CS = "mov eax,cs" __value [__ax]
extern  unsigned short  _DS( void );
#pragma aux _DS = "mov eax,ds" __value [__ax]
extern  unsigned short  _ES( void );
#pragma aux _ES = "mov eax,es" __value [__ax]
extern  unsigned short  _SS( void );
#pragma aux _SS = "mov eax,ss" __value [__ax]
extern  unsigned short  _FS( void );
#pragma aux _FS = "mov eax,fs" __value [__ax]
extern  unsigned short  _GS( void );
#pragma aux _GS = "mov eax,gs" __value [__ax]
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
