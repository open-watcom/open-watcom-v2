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
;* Description:  C library setjmp/longjmp support for 386 processors
;*
;*****************************************************************************

.386p
include mdef.inc
include struct.inc

        codeptr         "C",__longjmp_handler

        modstart        setjmp

if _MODEL and _BIG_DATA
SEGM_PRFX   equ es:
else
SEGM_PRFX   equ ds:
endif

ifdef __386__
ifdef __OS2__
        xref    DosUnwindException
endif
endif
ifdef __NT__
        xref    _RtlUnwind@16
endif

        xdefp   "C",_setjmp
        defpe   _setjmp
; save es register
if _MODEL and _BIG_DATA
        push    es
        mov     es,edx
        pop     SEGM_PRFX [eax+32]
else
        mov     SEGM_PRFX [eax+32],es   ; save es register
endif
; save ds register
        mov     SEGM_PRFX [eax+34],ds
; save return address offset
        pop     SEGM_PRFX [eax+24]
; save return address segment
if _MODEL and _BIG_CODE
        pop     SEGM_PRFX [eax+36]
else
        mov     SEGM_PRFX [eax+36],cs
endif
; save original esp register
        mov     SEGM_PRFX [eax+28],esp
; push return address back on stack
if _MODEL and _BIG_CODE
        push    SEGM_PRFX [eax+36]      ; push address segment
endif
        push    SEGM_PRFX [eax+24]      ; push address offset
; save rest of registers
        mov     SEGM_PRFX [eax],ebx
        mov     SEGM_PRFX [eax+4],ecx
        mov     SEGM_PRFX [eax+8],edx
        mov     SEGM_PRFX [eax+12],esi
        mov     SEGM_PRFX [eax+16],edi
        mov     SEGM_PRFX [eax+20],ebp
        mov     SEGM_PRFX [eax+38],fs
        mov     SEGM_PRFX [eax+40],gs
        mov     SEGM_PRFX [eax+42],ss
ifdef __NT__
        push    fs:[0]                  ; get exception chain
        pop     SEGM_PRFX [eax+44]      ; ...
endif
ifdef __386__
ifdef __OS2__
        push    fs:[0]                  ; get exception chain
        pop     SEGM_PRFX [eax+44]      ; ...
endif
endif
if _MODEL and _BIG_DATA
        mov     es,SEGM_PRFX [eax+32]   ; restore es register
endif
        sub     eax,eax         ; return 0
        ret                     ; return
_setjmp endp

;
;       There is a pragma for longjmp, saying that it aborts, therefore
;       the code generator does a jmp to here as opposed to a call.

        xdefp   "C",longjmp
        defpe   longjmp
ifdef __STACK__
        pop     eax             ; get address offset of jmp_buf
if _MODEL and _BIG_DATA
        pop     es              ; get address segment of jmp_buf
endif
        pop     edx             ; get return code
elseif _MODEL and _BIG_DATA
        mov     es,edx
        mov     edx,ebx
endif
        ; Warning, warning!
        ; the profiler knows about the stack frame that longjmp generates.
        ; do not change these 3 instructions without also changing the findLongJmpStack
        ; pragma in profilog.c
        ;
        push    edx             ; save return code in safe place
        push    eax             ; save jmp_buf address offset in safe place
        mov     ebp,esp
        ;
        ; end warning
        ;
ifdef __NT__
        push    eax             ; save address offset of jmp_buf
        mov     eax,SEGM_PRFX [eax+44]
        cmp     eax,fs:[0]
        jne     dounwind
        jmp     short done_unwind
dounwind:
        push    0
        push    offset done_unwind
        push    eax             ; unwind up to but not including SEH active
                                ; at setjmp()
        call    _RtlUnwind@16   ; trashes most registers except for ebp
done_unwind:
        mov     esp,ebp
        mov     eax,[ebp]
        mov     edx,[ebp+4]
endif
ifdef __386__
ifdef __OS2__
        push    eax             ; save address offset of jmp_buf
        push    0
;;      push    offset unwind
        mov     eax, offset unwind
        push    eax
        mov     eax,[esp+8]
        push    SEGM_PRFX [eax+44]
        call    DosUnwindException
unwind: add     esp,12
        pop     eax             ; restore address offset of jmp_buf
endif
endif
        push    eax                     ; save parm regs
        push    edx                     ; ...
        mov     dx,SEGM_PRFX [eax+42]             ; setup old ss:esp as a parm
        mov     eax,SEGM_PRFX [eax+28]
        call    __longjmp_handler       ; call handler
        pop     edx                     ; restore parm regs
        pop     eax                     ; ...

        mov     ss,SEGM_PRFX [eax+42]   ; load old ss:esp
        mov     esp,SEGM_PRFX [eax+28]  ; ...
if _MODEL and _BIG_CODE
        push    SEGM_PRFX [eax+36]      ; push saved cs (our return address segment)
endif
        push    SEGM_PRFX [eax+24]      ; push saved eip (our return address offset)
        or      edx,edx         ; if return code is 0
        _if     e               ; then
          inc   edx             ; - set it to 1
        _endif                  ; endif
        push    edx             ; save return code
        mov     ebx,SEGM_PRFX [eax]       ; load up the saved registers
        mov     ecx,SEGM_PRFX [eax+4]
        mov     esi,SEGM_PRFX [eax+12]
        mov     edi,SEGM_PRFX [eax+16]
        mov     ebp,SEGM_PRFX [eax+20]
        ;
        mov     edx,SEGM_PRFX [eax+38]
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
        mov     fs,edx
        mov     edx,SEGM_PRFX [eax+40]
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
        mov     gs,edx
if _MODEL and _BIG_DATA
        mov     edx,SEGM_PRFX [eax+34]
else
        mov     edx,SEGM_PRFX [eax+32]
endif
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
if _MODEL and _BIG_DATA
        mov     ds,edx
else
        mov     es,edx
endif
        mov     edx,SEGM_PRFX [eax+8]
if _MODEL and _BIG_DATA
        mov     es,SEGM_PRFX [eax+32]
else
        mov     ds,SEGM_PRFX [eax+34]
endif
        pop     eax             ; get return code
        ret                     ; return to point following setjmp call
longjmp endp

_TEXT   ends
        end
