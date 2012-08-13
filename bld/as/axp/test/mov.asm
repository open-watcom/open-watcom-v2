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

.globl MovTest1
.globl MovTest2
.globl MovTest3
.globl MovTest4

.globl LdaTest1
.globl LdaTest2
.globl LdaTest3
.globl LdaTest4
.globl LdaTest5

.text
MovTest1:
        mov     0x48,$v0
        ret

MovTest2:
        mov     0x1234,$v0
        ret

MovTest3:
        mov     0x12345678,$v0
        ret

MovTest4:
        mov     0x7fff89ab, $v0
        ret

LdaTest1:
        lda     $v0, 0x38
        ret

LdaTest2:
        lda     $v0, L1
        ldl     $v0, ($v0)
        ret

LdaTest3:
        lda     $v0, L1
        ldl     $v0, 4($v0)
        ret

LdaTest4:
        lda     $v0, 0x12345688
        ret

LdaTest5:
        lda     $v0, 0x7fff89ac
        ret

.data
L1:     .long   0x1224
        .long   0x1244
