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

#if defined(_M_IX86)
    #if !defined(__WINDOWS__) && !(defined(__RDOSDEV__) && defined(__SW_ZDP))
        #if defined(__BIG_DATA__)
            #define REG_DS      __ds
        #endif
    #endif

    #if !defined(__FLAT__)
        #define REG_ES          __es
    #endif

    #if defined(__386__)
        #if defined(__WINDOWS__) || !defined(__FLAT__)
            #define REG_FS      __fs
        #endif

        #define REG_GS          __gs
        #if defined(__SW_3S)
            #define REG_xAX      __eax
            #define REG_xBX      __ebx
            #define REG_xCX      __ecx
            #define REG_xDX      __edx
        #endif
    #endif

    #ifndef REG_xAX
        #define REG_xAX
    #endif
    #ifndef REG_xBX
        #define REG_xBX
    #endif
    #ifndef REG_xCX
        #define REG_xCX
    #endif
    #ifndef REG_xDX
        #define REG_xDX
    #endif
    #ifndef REG_DS
        #define REG_DS
    #endif
    #ifndef REG_ES
        #define REG_ES
    #endif
    #ifndef REG_FS
        #define REG_FS
    #endif
    #ifndef REG_GS
        #define REG_GS
    #endif

    #pragma aux __outside_CLIB __modify [REG_xAX REG_xBX REG_xCX REG_xDX REG_DS REG_ES REG_FS REG_GS]
    /*
        use as follows:

        typedef void vfv( void );
        #pragma aux (__outside_CLIB) vfv;

    */

    #undef REG_xAX
    #undef REG_xBX
    #undef REG_xCX
    #undef REG_xDX
    #undef REG_DS
    #undef REG_ES
    #undef REG_FS
    #undef REG_GS
#endif

#endif
