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
;* Description:  Stack crawler for MIPS.
;*
;*****************************************************************************


.text

.globl  _WtcStkCrawl

.set noat
.set noreorder

/* _WtcStkCrawl touches newly allocated stack area page by page.
 * This is required on operating systems that use the guard page
 * mechanism. Note that _WtcStkCrawl is used for alloca() support
 * and can use return address in $ra, unlike _WtcStkCrawlSize.
 *
 * Input:
 * $fp  - frame pointer (known to point to valid page)
 * $sp  - stack pointer (bottom of dynamic local storage)
 */
_WtcStkCrawl:
        slt     $t1,$sp,$fp     /* check if we need to do anything */
        beq     $t1,$zero,done
        move    $t0,$fp         /* start at frame pointer (in delay slot) */
L1:     addiu   $t0,$t0,-4096   /* go down a page */
        slt     $t1,$t0,$sp     /* see if t0 < sp */
        beq     $t1,$zero,L1    /* if not, loop */
        lw      $zero,4092($t0) /* touch memory (in delay slot) */
done:   jr      $ra             /* return whence we came */
        nop
