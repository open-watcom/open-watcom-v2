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
;* Description:  Get/set Alpha FP control register.
;*
;*****************************************************************************


    .text

    .globl  _GetFPCR            // Get current contents of the FPCR
    .globl  _SetFPCR            // Set contents of the FPCR

_GetFPCR:
    lda     $sp,-0x10($sp)      // Make some room on the stack
    excb                        // Make sure all instructions have completed
    mf_fpcr $f0                 // Load the FPCR contents into F0
    excb                        // Wait until we've completed
    stt     $f0, ($sp)          // Write them out to the stack
    ldl     $v0, 0x04($sp)      // Load the high dword into V0 (all we need)
    lda     $sp, 0x10($sp)      // Restore the stack
    ret                         // Return

_SetFPCR:
    lda     $sp,-0x10($sp)      // Make some room on the stack
    stq     $zero, ($sp)
    stl     $a0, 0x04($sp)      // Write A0 to the stack
    ldt     $f0, ($sp)          // Load into F0 (note only high dword matters)
    lda     $sp, 0x10($sp)      // Restore the stack
    excb                        // Make sure all instructions have completed
    mt_fpcr $f0                 // Write F0 out to the FPCR
    excb                        // Wait until we've completed
    ret                         // Return
