;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


#include "tests.h"
.set noreorder
//.set noat

.globl AbsTest1
.globl AbsTest2
.globl AbsTest3
.globl AbsTest4
.globl AbsTest5
.globl AbsTest6
.globl AbsTest7

.text
AbsTest1:
        mov     -0x1995, $a0
        absq    $a0, $v0
        ret
AbsTest2:
        mov     -0x1997, $v0
        absq    $v0, $v0
        ret
AbsTest3:
        absq    0x12345678, $v0
        ret
AbsTest4:
        absl    -0x12, $v0
        ret
AbsTest5:
        absl    0x12, $v0
        ret
AbsTest6:
        absl    -0x3456, $v0
        ret
AbsTest7:
        mov     -0x19951997, $a0
        absl    $a0, $v0
        ret
