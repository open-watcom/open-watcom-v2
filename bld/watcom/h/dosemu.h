/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOSEMU check for the DOS.
*
****************************************************************************/


#include "realmod.h"


/*
 * check text "$DOSEMU$" at 0xF000:0xFFE0
 */
extern int DOSEMUCheck( void );
#ifdef _M_I86
#pragma aux DOSEMUCheck = \
        "push ds"           \
        "mov  ax,0f000h"    \
        "mov  ds,ax"        \
        "mov  si,0ffe0h"    \
        "lodsw"             \
        "cmp  ax,'D$'"      \
        "jne short L1"      \
        "lodsw"             \
        "cmp  ax,'SO'"      \
        "jne short L1"      \
        "lodsw"             \
        "cmp  ax,'ME'"      \
        "jne short L1"      \
        "lodsw"             \
        "cmp  ax,'$U'"      \
    "L1: mov  ax,0"         \
        "jne short L2"      \
        "inc  ax"           \
    "L2: pop  ds"           \
    __parm      [] \
    __value     [__ax] \
    __modify    [__si]
#else
#pragma aux DOSEMUCheck = \
        "push ds"           \
        "mov  ax,_ExtenderRealModeSelector" \
        "mov  ds,eax"       \
        "mov  esi,0fffe0h"  \
        "lodsd"             \
        "cmp  eax,'SOD$'"   \
        "jne short L1"      \
        "lodsd"             \
        "cmp  eax,'$UME'"   \
    "L1: mov  eax,0"        \
        "jne short L2"      \
        "inc  eax"          \
    "L2: pop  ds"           \
    __parm      [] \
    __value     [__eax] \
    __modify    [__esi]
#endif
