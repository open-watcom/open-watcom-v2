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
;* Description:  routine for checking FPU type
;*
;*****************************************************************************

include mdef.inc

        modstart init8087

datasegment
        xred    __8087cw, word
ifdef __DOS__
        xred    __dos87emucall, word
endif
enddata

        xref    __init_8087_            ; in chk8087.c

        xdefp   __init_8087_emu
        xdefp   __x87id

__init_8087_emu proc near
        call    __init_8087_
        ret
__init_8087_emu endp

__x87id proc near
        push    BP                      ; save BP
        mov     BP,SP                   ; get access to stack
        sub     AX,AX
        push    AX                      ; allocate space for status word
        finit                           ; use default infinity mode
        fstcw   word ptr [BP-2]         ; save control word
        fwait
        pop     AX
        mov     AL,0
        cmp     AH,3
        jnz     nox87
        push    AX                      ; allocate space for status word
        fld1                            ; generate infinity by
        fldz                            ;   dividing 1 by 0
        fdiv                            ; ...
        fld     st                      ; form negative infinity
        fchs                            ; ...
        fcompp                          ; compare +/- infinity
        fstsw   word ptr [BP-2]         ; equal for 87/287
        fwait                           ; wait fstsw to complete
        pop     AX                      ; get NDP status word
        mov     AL,2                    ; assume 80287
        sahf                            ; store condition bits in flags
        jz      not387                  ; it's 287 if infinities equal
        mov     AL,3                    ; indicate 80387
not387: finit                           ; re-initialize the 8087
nox87:  mov     AH,0
        mov     SP,BP                   ; clean up stack
        pop     BP                      ; restore BP
        ret                             ; return
__x87id endp

        endmod
        end
