;/****************************************************************************
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
;* Description:  FPU related routines - read/write FPU state, get FPU type
;*                       16-bit version
;*
;****************************************************************************/


_TEXT segment byte public 'CODE'

ifndef __WINDOWS__

        public  "C", NPXType
NPXType proc    near
        push    bp                      ; save bp
        sub     ax,ax                   ; set initial control word to 0
        push    ax                      ; push it on stack
        mov     bp,sp                   ; point to control word
        finit                           ; initialize math coprocessor
        fstcw   [bp]                    ; store control word in memory
        fwait
        mov     al,0                    ; assume no coprocessor present
        mov     ah,[bp + 1]             ; upper byte is 03h if
        cmp     ah,03h                  ;   coprocessor is present
        jne     exit                    ; exit if no coprocessor present
        mov     al,1                    ; assume it is an 8087
        and     word ptr [bp],NOT 80h   ; turn interrupts on (IEM=0)
        fldcw   [bp]                    ; load control word
        fdisi                           ; disable interrupts (IEM=1)
        fstcw   [bp]                    ; store control word
        fwait
        test    word ptr [bp],80h       ; if IEM=1, then 8087
        jnz     exit                    ;
        finit                           ; use default infinity mode
        fld1                            ; generate infinity by
        fldz                            ;   dividing 1 by 0
        fdiv                            ; ...
        fld     st                      ; form negative infinity
        fchs                            ; ...
        fcompp                          ; compare +/- infinity
        fstsw   [bp]                    ; equal for 87/287
        fwait                           ; wait fstsw to complete
        mov     ax,[bp]                 ; get NDP control word
        mov     al,2                    ; assume 80287
        sahf                            ; store condition bits in flags
        jz      exit                    ; it's 287 if infinities equal
        mov     al,3                    ; indicate 80387
exit:   cbw                             ; zero ah
        fninit                          ; initialize math coprocessor
        pop     bp                      ; throw away control word
        pop     bp                      ; restore bp
        ret                             ; return
NPXType endp

endif

        public  "C", FPUExpand
FPUExpand  proc    near
        push    ds
        push    es
        push    si
        push    di
        push    cx
        mov     ds,dx
        mov     es,dx
        mov     si,ax
        std
        lea     di,[si + 28 + 80 - 2]
        lea     si,[si + 14 + 80 - 2]
        mov     cx,40
        rep     movsw
        xor     ax,ax
        mov     cx,6
loop1:  stosw
        movsw
        loop    loop1
        stosw
        cld
        pop     cx
        pop     di
        pop     si
        pop     es
        pop     ds
        ret
FPUExpand  endp

        public  "C", FPUContract
FPUContract  proc    near
        push    ds
        push    es
        push    si
        push    di
        push    cx
        mov     ds,dx
        mov     es,dx
        mov     si,ax
        lea     di,[si + 2]
        lea     si,[si + 4]
        mov     cx,6
loop2:  movsw
        add     si,2
        loop    loop2
        mov     cx,40
        rep     movsw
        pop     cx
        pop     di
        pop     si
        pop     es
        pop     ds
        ret
FPUContract  endp

ifndef __OS2__

Read8087 PROC
        public  "C",Read8087
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
ifdef REAL_MODE
        fnsave  [bx]
else
        fsave   [bx]
        frstor  [bx]
endif
        fwait
        pop     bx
        pop     ds
        call    FPUExpand
        ret
Read8087 ENDP

Write8087 PROC
        public  "C",Write8087
        call    FPUContract
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        frstor  [bx]
        fwait
        pop     bx
        pop     ds
        ret
Write8087 ENDP

comment ~
    These routines read/write the FPU or emulator state when we're on a 386
    or better. They're a bit wierd so here's a explanation. There are operand
    size overrides on the fnsave and frstor so that a real FPU will save the
    full 32-bit state (this code is in a 16-bit segment). The emulator does
    not respect the operand size, but since it always stores the full state
    anyway that's OK. However, the 32-bit emulator does not know how to decode
    a 16-bit addressing mode - it always interprets things as if they were in
    the 32-bit form. That means that the [bx] addressing mode gets interpreted
    as [edi], which is why there are "movzx edi,bx" instructions. before the
    coprocessor instructions.
~

ifndef REAL_MODE

        .386

Read387 PROC
        public  "C",Read387
        push    ds
        push    bx
	push    edi
        mov     ds,dx
        mov     bx,ax
	movzx   edi,bx
        fsaved  ds:[bx]
	fwait
        frstord ds:[bx]
        fwait
	pop     edi
        pop     bx
        pop     ds
        ret
Read387 ENDP

Write387 PROC
        public  "C",Write387
        push    ds
        push    bx
	push    edi
        mov     ds,dx
        mov     bx,ax
	movzx   edi,bx
	fwait
        frstord ds:[bx]
        fwait
	pop     edi
        pop     bx
        pop     ds
        ret
Write387 endp

endif

endif

_TEXT           ENDS

                END
