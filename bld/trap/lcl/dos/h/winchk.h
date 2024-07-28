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
