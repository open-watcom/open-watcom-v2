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
;* Description:  Stack scribble routine for MIPS.
;*
;*****************************************************************************


.text

.globl  _WtcStkScribble

.set noat
.set noreorder

/*
 * Note that this routine is called from function prolog,
 * and so is able to destroy registers which are volatile
 * across standard routines.
 */

_WtcInitPattern:
    .long   0xdeadbeef

/* _WtcStkScribble pre-initializes local stack storage to
 * _WtcInitPattern, making it very likely that unitialized
 * accesses will be caught. The value should be greater than
 * 0x80000000 as that guarantees an exception in user mode
 * should it be interpreted as address.
 *
 * Input:
 * $v0  - return address(!)
 * $t0  - number of bytes to trash
 * $sp  - address of bottom of stack
 */
_WtcStkScribble:
        la      $t1,_WtcInitPattern
        lw      $t2,($t1)       /* load stomp pattern */
        addu    $t0,$sp,$t0     /* make t0 point to top of local storage */
L1:     addiu   $t0,$t0,-4
        bne     $sp,$t0,L1      /* trash stack word by word */
        sw      $t2,($t0)       /* instruction in delay slot! */
        jr      $v0             /* return whence we came */
        nop
