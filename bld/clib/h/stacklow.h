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


#ifndef _STACKLOW_H_INCLUDED
#define _STACKLOW_H_INCLUDED

#define __Align4K( x ) (((x)+0xfffL) & ~0xfffL )

#if defined(_M_IX86)
    extern  unsigned    _SP( void );
    #if defined(__386__)
        #pragma aux _SP = "mov eax,esp" value [eax];
    #else
        #pragma aux _SP = "mov ax,sp" value [ax];
    #endif
#endif

#if defined(__NT__)
    extern void         __init_stack_limits( unsigned *stacklow, unsigned *stacktop );
#endif

#if defined(__SW_BM)
    #include "thread.h"
    #define _STACKLOW   (__THREADDATAPTR->__stklowP)
#else
    extern  unsigned        _STACKLOW;
#endif

#endif

