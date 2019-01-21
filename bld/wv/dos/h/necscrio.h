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


/*
 * OKAY - once and for all  I am defining these here!!!!!
 */

#define NEC_DEBUG_SCREEN        1
#define NEC_USER_SCREEN         0

extern void BIOSSetPage( char );
#pragma aux BIOSSetPage = \
        "cmp  al,1"         \
        "je short Page1"    \
        "xor  dx,dx"        \
        "jmp short foo"     \
    "Page1:"                \
        "mov  dx,1000h"     \
    "foo:"                  \
        "mov  ah,0Eh"       \
        "int 18h"           \
    __parm __caller [__al] \
    __value         \
    __modify        [__ax __dx]

extern char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "mov  ah,0Bh"       \
        "int 18h"           \
    __parm      [] \
    __value     [__al] \
    __modify    [__ax]

extern void BIOSSetMode( char );
#pragma aux BIOSSetMode = \
        "mov  ah,0Ah"       \
        "int 18h"           \
    __parm      [__al] \
    __value     \
    __modify    [__ax]

extern void BIOSTextOn( void );
#pragma aux BIOSTextOn = \
        "mov  ah,0Ch"       \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void BIOSTextOff( void );
#pragma aux BIOSTextOff = \
        "mov  ah,0Dh"       \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void BIOSGraphOn( void );
#pragma aux BIOSGraphOn = \
        "mov  ah,40h"       \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void BIOSGraphOff( void );
#pragma aux BIOSGraphOff = \
        "mov    ah,41h"     \
        "int    18h"        \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void DoRingBell( char __far * s );
#pragma aux DoRingBell = \
        "mov  bx,2"         \
        "mov  cx,1"         \
        "mov  ah,40h"       \
        "int 21h"           \
    __parm      [__dx] \
    __value     \
    __modify    [__ah __cx]

#define RING_BELL_ DoRingBell( "\007" );

void GdcMask( void );
#pragma aux GdcMask = \
        "pushf"             \
        "cli"               \
    __parm      [] \
    __value     \
    __modify    []

void GdcUnMask( void );
#pragma aux GdcUnMask = \
        "popf"              \
    __parm      [] \
    __value     \
    __modify    []

void GdcDelay( void );
#pragma aux GdcDelay = \
        "jmp short L1"      \
    "L1: jmp short L2"      \
    "L2: jmp short L3"      \
    "L3:"                   \
    __parm      [] \
    __value     \
    __modify    []

extern void BIOSSetCurPos( int );
#pragma aux BIOSSetCurPos = \
        "mov  ah,13h"       \
        "int 18h"           \
    __parm __caller [__dx] \
    __value         \
    __modify        [ah]

extern void BIOS_OPEN_APP( void );
#pragma aux BIOS_OPEN_APP = \
        "xor    ax,ax"      \
        "mov    cl,0e0h"    \
        "int 0DCh"          \
    __parm      [] \
    __value     \
    __modify    [__ax __cl]

extern void BIOS_CLOSE_APP( void );
#pragma aux BIOS_CLOSE_APP = \
        "mov  cl,0e1h"      \
        "int 0DCh"          \
    __parm      [] \
    __value     \
    __modify    [__ax __cl]

extern unsigned short BIOS_JIS_SHF( unsigned short );
#pragma aux BIOS_JIS_SHF = \
        "mov  cl,0f3h"      \
        "int 0DCh"          \
    __parm __caller [__ax] \
    __value         [__ax] \
    __modify        [__ax __cl]

extern void BIOSSetCursorOn( void );
#pragma aux BIOSSetCursorOn = \
        "mov  ah,11h"       \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void BIOSSetCursorOff( void );
#pragma aux BIOSSetCursorOff =  \
        "mov  ah,12h"       \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ah]

extern void BIOSSetCurBlinkOn( void );
#pragma aux BIOSSetCurBlinkOn = \
        "mov  ax,1000h"     \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ax]

extern void BIOSSetCurBlinkOff( void );
#pragma aux BIOSSetCurBlinkOff = \
        "mov  ax,1001h"     \
        "int 18h"           \
    __parm      [] \
    __value     \
    __modify    [__ax]

