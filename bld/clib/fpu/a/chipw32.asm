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


; static char sccs_id[] = "@(#)wrap32.asm       1.1  12/15/94  15:56:32";
;
; This code is being published by Intel to users of the Pentium(tm)
; processor.  Recipients are authorized to copy, modify, compile, use and
; distribute the code.
;
; Intel makes no warranty of any kind with regard to this code, including
; but not limited to, implied warranties or merchantability and fitness for
; a particular purpose. Intel assumes no responsibility for any errors that
; may appear in this code.
;
; No patent licenses are granted, express or implied.
;
;
;  The following code is a PRELIMINARY IMPLEMENTATION of a
;  software patch for the FPATAN instruction replacement.
;
;
include mdef.inc

.386
.387

DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'


FPU_STATE       STRUC
        CONTROL_WORD    DW      ?
        reserved_1      DW      ?
        STATUS_WORD     DW      ?
        reserved_2      DW      ?
        TAG_WORD        DW      ?
        reserved_3      DW      ?
        IP_OFFSET       DD      ?
        CS_SLCT         DW      ?
        OPCODE          DW      ?
        DATA_OFFSET     DD      ?
        OPERAND_SLCT    DW      ?
        reserved_4      DW      ?
        REGISTER_0      DT      ?
        REGISTER_1      DT      ?
        REGISTER_2      DT      ?
        REGISTER_3      DT      ?
        REGISTER_4      DT      ?
        REGISTER_5      DT      ?
        REGISTER_6      DT      ?
        REGISTER_7      DT      ?
        SAVE_REG_0      DT      ?
FPU_STATE       ENDS

ENV_SIZE        EQU     118

DATA   ENDS

DGROUP GROUP DATA


_TEXT  SEGMENT   DWORD USE32 PUBLIC 'CODE'


        assume cs:_TEXT, ds:DGROUP, es:DGROUP, ss:nothing
        public  __fpatan_wrap

;
;  PRELIMINARY VERSION for FPATAN instruction replacement
;

        EXTRN   __fpatan_chk:NEAR

        defpe   __fpatan_wrap
        push    eax
        sub     esp, ENV_SIZE
        fstp    tbyte ptr [esp].SAVE_REG_0
        fnsave  [esp]
        fld     tbyte ptr [esp].REGISTER_0
        fld     tbyte ptr [esp].SAVE_REG_0
        call    __fpatan_chk
        fstsw   ax
        fstp    tbyte ptr [esp].REGISTER_0
        and     ax, 0ffh
        or      ax, [esp].STATUS_WORD
        mov     [esp].STATUS_WORD, ax
        frstor  [esp]
        add     esp, ENV_SIZE
        pop     eax
        ret
__fpatan_wrap     ENDP

_TEXT  ENDS
        end
