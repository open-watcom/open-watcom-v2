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


#define EAXtoDXAX \
    "mov  dx,ax"    \
    "shr  eax,16"   \
    "xchg dx,ax"

#define DXAXtoEAX \
    "xchg dx,ax"    \
    "shl  eax,16"   \
    "mov  ax,dx"

extern void SetDR0( unsigned long );
#pragma aux SetDR0 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr0,eax"  \
    __parm [__dx __ax]
extern void SetDR1( unsigned long );
#pragma aux SetDR1 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr1,eax"  \
    __parm [__dx __ax]
extern void SetDR2( unsigned long );
#pragma aux SetDR2 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr2,eax"  \
    __parm [__dx __ax]
extern void SetDR3( unsigned long );
#pragma aux SetDR3 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr3,eax"  \
    __parm [__dx __ax]
extern void SetDR6( unsigned long );
#pragma aux SetDR6 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr6,eax"  \
    __parm [__dx __ax]
extern void SetDR7( unsigned long );
#pragma aux SetDR7 = \
        ".386p"         \
        DXAXtoEAX        \
        "mov  dr7,eax"  \
    __parm [__dx __ax]

extern unsigned long GetDR0( void );
#pragma aux GetDR0 = \
        ".386p"         \
        "mov  eax,dr0"  \
        EAXtoDXAX        \
    __value [__dx __ax]
extern unsigned long GetDR1( void );
#pragma aux GetDR1 = \
        ".386p"         \
        "mov  eax,dr1"  \
        EAXtoDXAX        \
    __value [__dx __ax]
extern unsigned long GetDR2( void );
#pragma aux GetDR2 = \
        ".386p"         \
        "mov  eax,dr2"  \
        EAXtoDXAX        \
    __value [__dx __ax]
extern unsigned long GetDR3( void );
#pragma aux GetDR3 = \
        ".386p"         \
        "mov  eax,dr3"  \
        EAXtoDXAX        \
    __value [__dx __ax]
extern unsigned long GetDR6( void );
#pragma aux GetDR6 = \
        ".386p"         \
        "mov  eax,dr6"  \
        EAXtoDXAX        \
    __value [__dx __ax]
extern unsigned long GetDR7( void );
#pragma aux GetDR7 = \
        ".386p"         \
        "mov  eax,dr7"  \
        EAXtoDXAX        \
    __value [__dx __ax]
