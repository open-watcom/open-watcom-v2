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


.386p
include mdef.inc
include struct.inc

        codeptr         "C",__longjmp_handler

        modstart        setjmp

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
        mov     [eax],ebx       ; save registers
        mov     4[eax],ecx
        mov     8[eax],edx
        mov     12[eax],esi
        mov     16[eax],edi
        mov     20[eax],ebp
        pop     24[eax]         ; get return address
        mov     28[eax],esp
        push    24[eax]         ; push return address back on stack
        mov     32[eax],es      ; save segment registers
        mov     34[eax],ds
        mov     36[eax],cs
        mov     38[eax],fs
        mov     40[eax],gs
        mov     42[eax],ss
ifdef __NT__
        push    fs:[0]          ; touched this line to cause recompile
        pop     44[eax]
endif
ifdef __386__
ifdef __OS2__
        push    fs:[0]          ; get exception chain
        pop     44[eax]         ; ...
endif
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
        pop     eax             ; get address of jmp_buf
        pop     edx             ; get return code
endif
        ; Warning, warning!
        ; the profiler knows about the stack frame that longjmp generates.
        ; do not change these 3 instructions without also changing the findLongJmpStack
        ; pragma in profilog.c
        ;
        push    edx
        push    eax             ; save jmp_buf & retval in safe place
        mov     ebp,esp
        ;
        ; end warning
        ;
ifdef __NT__
        push    eax             ; save address of jmp_buf
        mov     eax,[eax+44]
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
        mov     eax,0[ebp]
        mov     edx,4[ebp]
endif
ifdef __386__
ifdef __OS2__
        push    eax             ; save address of jmp_buf
        push    0
;;      push    offset unwind
        mov     eax, offset unwind
        push    eax
        mov     eax,8[esp]
        push    44[eax]
        call    DosUnwindException
unwind: add     esp,12
        pop     eax             ; restore address of jmp_buf
endif
endif
        push    eax                     ; save parm regs
        push    edx
        mov     dx,42[eax]              ; setup old ss:esp as a parm
        mov     eax,28[eax]
        call    __longjmp_handler       ; call handler
        pop     edx                     ; restore parm regs
        pop     eax

        mov     ss,42[eax]      ; load old ss:esp
        mov     esp,28[eax]     ; ...
        push    24[eax]         ; push saved eip (our return address)
        or      edx,edx         ; if return code is 0
        _if     e               ; then
          inc   edx             ; - set it to 1
        _endif                  ; endif
        push    edx             ; save return code
        mov     ebx,[eax]       ; load up the saved registers
        mov     ecx,4[eax]
        mov     esi,12[eax]
        mov     edi,16[eax]
        mov     ebp,20[eax]
        mov     dx,32[eax]
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
        mov     es,dx
        mov     dx,38[eax]
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
        mov     fs,dx
        mov     dx,40[eax]
        verr    dx              ; verify segment
        _if     ne
          sub   edx,edx
        _endif
        mov     gs,dx
        mov     edx,8[eax]
        mov     ds,34[eax]
        pop     eax             ; get return code
        ret                     ; return to point following setjmp call
longjmp endp

_TEXT   ends
        end
