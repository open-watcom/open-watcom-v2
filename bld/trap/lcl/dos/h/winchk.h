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
* Description:  Varoius environment checks for the DOS debugger.
*
****************************************************************************/


extern byte EnhancedWinCheck( void );
#pragma aux EnhancedWinCheck = \
        "mov  ax,1600h" \
        "int 2fh"       \
    __parm      [] \
    __value     [__al] \
    __modify    [__ah]

extern unsigned DPMIVersion( void );
#pragma aux DPMIVersion = \
        "mov  ax,1687h" \
        "int 2fh"       \
        "test ax,ax"    \
        "je short L1"   \
        "xor  dx,dx"    \
    "L1:"               \
    __parm      [] \
    __value     [__dx] \
    __modify    [__ax __bx __cx __dx __si __es __di]

const char DOSEMUString[] = "$DOSEMU$";

extern int DOSEMUCheck( void );
#pragma aux DOSEMUCheck = \
        "push ds"           \
        "mov  ax,0f000h"    \
        "mov  es,ax"        \
        "mov  di,0ffe0h"    \
        "mov  ax,seg DOSEMUString" \
        "mov  ds,ax"        \
        "mov  si,offset DOSEMUString" \
        "mov  cx,4"         \
        "cld"               \
        "repe cmpsw"        \
        "mov  ax,0"         \
        "jne short L1"      \
        "inc  ax"           \
    "L1: pop  ds"           \
    __parm      [] \
    __value     [__ax] \
    __modify    [__bx __cx __dx __si __es __di]
