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
* Description:  Special interface for calls to non-Watcom routines from
*               inside clib (x86 specific).
*
****************************************************************************/


#ifndef EXTFUNC_H_INCLUDED
#define EXTFUNC_H_INCLUDED

#include "variety.h"

#if defined(_M_IX86)
    #if !defined(__WINDOWS__) && !(defined(__RDOSDEV__) && defined(__SW_ZDP))
        #if defined(__BIG_DATA__)
            #define __DS        ds
        #endif
    #endif

    #if !defined(__FLAT__)
        #define __ES    es
    #endif

    #if defined(__386__)
        #if defined(__WINDOWS__) || !defined(__FLAT__)
            #define __FS        fs
        #endif

        #define __GS    gs
        #if defined(__SW_3S)
            #define __AX        eax
            #define __BX        ebx
            #define __CX        ecx
            #define __DX        edx
        #endif
    #endif

    #ifndef __AX
        #define __AX
    #endif
    #ifndef __BX
        #define __BX
    #endif
    #ifndef __CX
        #define __CX
    #endif
    #ifndef __DX
        #define __DX
    #endif
    #ifndef __DS
        #define __DS
    #endif
    #ifndef __ES
        #define __ES
    #endif
    #ifndef __FS
        #define __FS
    #endif
    #ifndef __GS
        #define __GS
    #endif

    #pragma aux __outside_CLIB modify [__AX __BX __CX __DX __DS __ES __FS __GS];
    /*
            use as follows:

    typedef void vfv( void );
    #pragma aux (__outside_CLIB) __vfv;
    */

    #undef __AX
    #undef __BX
    #undef __CX
    #undef __DX
    #undef __DS
    #undef __ES
    #undef __FS
    #undef __GS
#endif

#endif
