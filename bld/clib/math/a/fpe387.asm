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


;
; FPE387        : connect/disconnect 80x87 interrupt handler
;
.8087
.386p

include struct.inc
include mdef.inc
include extender.inc

        if __WASM__ ge 100
            xref        "C",__Enable_FPE        ; enable exceptions
        else
            xref        <"C",__Enable_FPE>      ; enable exceptions
        endif
        xref    __Phar_hook_init_       ; setup interupts for Pharlap
        xref    __Phar_hook_fini_       ; restore interupts for Pharlap
        xref    __DOS4G_hook_init_      ; setup interupts for Rational
        xref    __DOS4G_hook_fini_      ; restore interupts for Rational
        xref    __Ergo_hook_init_       ; setup interupts for Ergo
        xref    __Ergo_hook_fini_       ; restore interupts for Ergo
        xref    __Intel_hook_init_
        xref    __Intel_hook_fini_

        modstart        fpe387

        datasegment

        extrn   __8087          : byte
        extrn   "C",_Extender   : byte

        public  "C",__PMSeg
        public  "C",__PMAddr
        public  "C",__RMAddr
__PMSeg         dw      0
__PMAddr        dd      0
__RMAddr        dd      0

        enddata

        if __WASM__ ge 100
            xdefp       "C",__Init_FPE_handler
        else
            xdefp       <"C",__Init_FPE_handler>
        endif
defp    __Init_FPE_handler
        _guess                          ; guess initialization required
          cmp   word ptr __PMSeg,0      ; - quit if already initialized
          _quif ne                      ; - ...
          cmp   byte ptr __8087,0       ; - quit if no 80x87 present
          _quif e                       ; - ...
        _admit                          ; admit: already initialized
           ret                          ; - return
        _endguess                       ; endguess
        push    EAX                     ; save registers
        push    EBX                     ; ...
        push    ECX                     ; ...
        push    EDX                     ; ...
        push    ES                      ; ...
        push    DS                      ; ...
        _guess                          ; guess: Ergo OS/386
          cmp   byte ptr _Extender,X_ERGO ; - quit if not OS/386 from Ergo
          _quif ne                      ; - ...
          call __Ergo_hook_init_        ; - setup interupt vector
        _admit                          ; guess: Rational DOS/4G
          cmp   byte ptr _Extender,X_RATIONAL ; - quit if not DOS/4G
          _quif ne                      ; - ...
          call __DOS4G_hook_init_       ; - setup interupt vector
        _admit                          ; guess: Intel Code Builder
          cmp   byte ptr _Extender,X_INTEL ; - quit if not Code Builder
          _quif ne                      ; - ...
          call __Intel_hook_init_       ; - setup interupt vector
        _admit                          ; admit: Pharlap
          call __Phar_hook_init_        ; - setup interupt vector
        _endguess                       ; endguess
        pop     DS                      ; restore DS
        call    __Enable_FPE            ; enable exceptions
        pop     ES                      ; restore registers
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
        pop     EAX                     ; ...
        ret                             ; return
endproc __Init_FPE_handler


        if __WASM__ ge 100
            xdefp       "C",__Fini_FPE_handler
        else
            xdefp       <"C",__Fini_FPE_handler>
        endif
defp    __Fini_FPE_handler
        cmp     word ptr __PMSeg,0      ; if not initialized
        _if     e                       ; - then
          ret                           ; - return
        _endif                          ; endif
        push    EAX                     ; save registers
        push    EBX                     ; ...
        push    EDX                     ; ...
        push    DS                      ; ...
        sub     ESP,4                   ; allocate space for control word
        fstcw   word ptr [ESP]          ; get control word
        fwait                           ; ...
        mov     byte ptr [ESP],7Fh      ; disable exception interrupts
        fldcw   word ptr [ESP]          ; ...
        fwait                           ; ...
        add     ESP,4                   ; remove temporary
        _guess                          ; guess: Ergo OS/386
          cmp   byte ptr _Extender,X_ERGO ; quit if not OS/386 from Ergo
          _quif ne                      ; - ...
          call __Ergo_hook_fini_        ; - restore int vector
        _admit                          ; guess: Rational DOS/4G
          cmp byte ptr _Extender,X_RATIONAL ; - quit if not DOS/4G
          _quif ne                      ; - ...
          call __DOS4G_hook_fini_       ; - restore int vector
        _admit                          ; guess: Intel Code Builder
          cmp byte ptr _Extender,X_INTEL ; quit if not Code Builder
          _quif ne                      ; - ...
          call __Intel_hook_fini_       ; - restore int vector
        _admit                          ; admit: Pharlap
          call __Phar_hook_fini_        ; - restore int vector
        _endguess                       ; endguess
        pop     DS                      ; restore registers
        pop     EDX                     ; ...
        pop     EBX                     ; ...
        pop     EAX                     ; ...
        ret
endproc __Fini_FPE_handler

        endmod
        end
