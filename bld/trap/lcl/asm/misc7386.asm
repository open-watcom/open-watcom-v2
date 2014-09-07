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
;*                       32-bit version
;*
;****************************************************************************/


_TEXT segment byte public 'CODE'


NPXType proc    "C" public
ifdef __NETWARE__
        push    ebx
        mov     eax,cr0                 ; get control word
        push    eax                     ; save it
        and     eax,NOT 4               ; turn off EM bit
        mov     cr0,eax                 ; ...
endif
        sub     eax,eax                 ; set initial control word to 0
        push    eax                     ; push it on stack
;
        fninit                          ; initialize math coprocessor
        fnstcw  [esp]                   ; store control word in memory
        xor     al,al                   ; assume no coprocessor present
        mov     ah,[esp + 1]            ; upper byte is 03h if
        cmp     ah,03h                  ;   coprocessor is present
        jne     exit                    ; exit if no coprocessor present
        mov     al,1                    ; assume it is an 8087
        and     word ptr [esp],NOT 80h  ; turn interrupts on (IEM=0)
        fldcw   [esp]                   ; load control word
        fdisi                           ; disable interrupts (IEM=1)
        fstcw   [esp]                   ; store control word
        fwait
        test    byte ptr [esp], 80h     ; if IEM=1, then 8087
        jnz     exit                    ;
        finit                           ; use default infinity mode
        fld1                            ; generate infinity by
        fldz                            ;   dividing 1 by 0
        fdiv                            ; ...
        fld     st                      ; form negative infinity
        fchs                            ; ...
        fcompp                          ; compare +/- infinity
        fstsw   [esp]                   ; equal for 87/287
        fwait                           ; wait fstsw to complete
        mov     ax,[esp]                ; get NDP control word
        mov     al,2                    ; assume 80287
        sahf                            ; store condition bits in flags
        jz      exit                    ; it's 287 if infinities equal
        mov     al,3                    ; indicate 80387
exit:   fninit                          ; initialize math coprocessor
        add     esp,4                   ; clear the stack
ifdef __NETWARE__
        pop     ebx                     ; restore control word
        mov     cr0,ebx                 ; ...
        pop     ebx
endif
        ret                             ; return
NPXType endp

ifdef __QNX__

FPUExpand  proc "C" public
        push    es
        push    esi
        push    edi
        push    ecx
        push    ds
        pop     es
        std
        lea     edi,[eax + 28 + 80 - 1]
        lea     esi,[eax + 14 + 80 - 1]
        mov     ecx,80
        rep     movsb
        xor     eax,eax
        mov     ecx,6
loop1:  stosw
        movsw
        loop    loop1
        stosw
        cld
        pop     ecx
        pop     edi
        pop     esi
        pop     es
        ret
FPUExpand  endp

FPUContract  proc   "C" public
        push    es
        push    esi
        push    edi
        push    ecx
        push    ds
        pop     es
        lea     edi,[eax + 2]
        lea     esi,[eax + 4]
        mov     ecx,6
loop2:  movsw
        add     esi,2
        loop    loop2
        mov     ecx,80
        rep     movsb
        pop     ecx
        pop     edi
        pop     esi
        pop     es
        ret
FPUContract  endp

else

Read387 PROC "C" public
ifdef __STACK__
        mov     eax,4[esp]              ; get buffer address
endif
        fsave   [eax]                   ; save 387 there
ifndef __NETWARE__
        frstor  [eax]                   ; restore it (fsave does finit)
endif
        fwait                           ; wait for fsave
ifdef PHARLAP
        nop
        int     3
else
        ret
endif
Read387 ENDP


Write387 PROC "C" public
ifdef __STACK__
        mov     eax,4[esp]              ; get buffer address
endif
        frstor  [eax]                   ; set 8087 state
        fwait                           ; wait for frstor to finish
ifdef PHARLAP
        nop
        int     3
else
        ret
endif
Write387 endp

endif

_TEXT   ENDS

        END
