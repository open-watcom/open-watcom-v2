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


/*
PDCTXAXP -- context support for DEC ALPHA AXP

*/


//--------------------------------------------------------------------------
// The following routine receives control at the end of either an
// RtlUnwind or RtlUnwindRfp invocation. Its purpose is to restore
// the SP, FP registers and to return to the same location as did
// the corresponding call to CPPLIB( PdCtx ).
//
// 0 is returned so that the caller can distinguish between an initial
// invocation and a return from unwinding.
//
// Upon entry, the SP and FP registers correspond to those when
// the exception was raised, presumeably up the stack.  Consequently,
// there is an assumption that that intervening stack contents would
// not be modified while the routine is active.
//
// Input:
//
//      v0 - address of save area
//      sp - for code
//      fp - for code
//      ra - an old value
//
// Output:
//
//      v0 - zero (return value)
//      sp - same as for corresponding call to CPPLIB( PdCtx )
//      fp - same as for corresponding call to CPPLIB( PdCtx )
//      ra - same as for corresponding call to CPPLIB( PdCtx )
//
//--------------------------------------------------------------------------

_internal_restore:
    ldq     $ra,0x10($v0)       // restore RA
    ldq     $sp,0x00($v0)       // restore SP
    ldq     $fp,0x08($v0)       // restore FP
    clr     $v0                 // return 0
    ret                         // return

//--------------------------------------------------------------------------
//  CPPLIB( PdCtx ) is called to save the context to be restored
//  when RtlUnwind or RtlUnwindRfp are called.  It returns the address
//  of the restoration routine, to be used as the continuation address
//  on completion of these functions.
//
//  Input:
//
//      a0 - address of _CONTEXT record
//      a1 - address of 3 quads to be used as a save area
//
//  Output:
//
//      v0 - address of _internal_restore
//
//--------------------------------------------------------------------------

    .globl  __wcpp_4_PdCtx_

__wcpp_4_PdCtx_:
    stq     $ra,0x10($a1)       // save RA address
    stq     $sp,0x00($a1)       // save SP address
    stq     $fp,0x08($a1)       // save FP address
    lda     $v0,_internal_restore // return value
    ret                         // return



//--------------------------------------------------------------------------
// Thunk for exception handler
//
// This thunk is used so we can identify our handlers from other ones.  We
// can't just use an address comparison, since our handler might be
// duplicated in a DLL.
//
// This is accomplished by placing "WAT++COM" in two words behind a
// branch to the handler code.
//--------------------------------------------------------------------------

    .globl  __wcpp_4_pd_handler_
    .extern __wcpp_4_pd_handler_rtn_


__wcpp_4_pd_handler_:
    br      __wcpp_4_pd_handler_rtn_
    .long   0x2B544157  // "WAT+"
    .long   0x4D4F432B  // "+COM"
