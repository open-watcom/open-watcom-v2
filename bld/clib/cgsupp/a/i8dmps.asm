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
;* Description:  Long division for MIPS.
;*
;*****************************************************************************


.text

.globl  _WtcI8D
.globl  _WtcU8D

.set noat
.set noreorder

/* _WtcI8D and _WtcU8D perform 64-bit long division with signed or
 * unsigned operands, respectively.
 *
 * Input:
 * $a0:$a1  - 64-bit dividend
 * $a2:$a3  - 64-bit divisor
 *
 * Returns:
 * $v0:$v1  - 64-bit result of division
 */
_WtcI8D:
        break                   /* hit the breaks */
        jr      $ra
        nop

_WtcU8D:
        break                   /* hit the breaks */
        jr      $ra
        nop
