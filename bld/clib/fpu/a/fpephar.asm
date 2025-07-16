;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  FPE interrupt handler hook for PharLap DOS extender.
;*                (32-bit DOS code only)
;*
;*****************************************************************************


.8087
.386p

include struct.inc
include mdef.inc
include extender.inc
include fpeint.inc
include int21.inc

        xrefp           __FPEHandler_   ; handle exceptions
        xrefp           __FPE2Handler_  ; handle exceptions w/o OUT's

        modstart        fpephar

        datasegment

        extrn   "C",_Extender   : byte
        extrn   "C",__RMAddr    : dword
        extrn   "C",__PMAddr    : dword
        extrn   "C",__PMSeg     : word

        enddata


        xdefp   __Phar_hook_init_
defp    __Phar_hook_init_
        push    DS                      ; save DS
ifdef __WATCOM_PC98__
        cmp     byte ptr MST_PIC,IBM_MST_PIC ; if IBM PC master PIC port number
        _if     e                       ; then
          lea   EDX,__FPEHandler_       ; - point at handler with OUT's
        _else                           ; else (NEC or some other non-IBM PC)
          lea   EDX,__FPE2Handler_      ; - point at handler with no OUT's
        _endif                          ; endif
else
        lea     EDX,__FPEHandler_       ; - point at handler with OUT's
endif
        cmp     byte ptr _Extender,X_PHARLAP_V3 ; if version 2.x
        _if     b                       ; - then
          mov   CL,50h                  ; - get contents of interrupt 50h
          mov   AX,2503h                ; - get real-mode interrupt vector
          int21h                        ; - ...
          push  EBX                     ; - save rm int 50
          mov   AX,2502h                ; - get protected-mode interrupt vector
          int21h                        ; - ...
          push  EBX                     ; - save pm int 50
          push  ES                      ; - ...
          push  DS                      ; - save DS
          mov   AX,CS                   ; - set DS:EDX pointing at handler
          mov   DS,AX                   ; - (EDX set up above)
          mov   AX,2506h                ; - set new interrupt handler
          int21h                        ; - ...
          pop   DS                      ; - restore DS
        _endif                          ; endif
        mov     AX,250ch                ; get base interrupt vector for
        int21h                          ; ... IRQ8-IRQ15
        mov     CL,AH                   ; get interrupt # for IRQ 13 (8 on NEC)
;;      add     CL,13-8                 ; ... JBS 92/10/05
        add     CL,IRQ_NUM              ; ...
        sub     CL,8                    ; ...
        mov     AX,2502h                ; get protected-mode interrupt vector
        int21h                          ; ...
        mov     __PMAddr,EBX            ; save protected-mode interrupt vector
        mov     __PMSeg,ES              ; ...
        mov     AX,2503h                ; get real-mode interrupt vector
        int21h                          ; ...
        mov     __RMAddr,EBX            ; save real-mode interrupt vector
        cmp     byte ptr _Extender,X_PHARLAP_V3 ; if version 2.x
        _if     b                       ; - then
          sub   ESP,8                   ; - allocate temporary
          sidt  [ESP]                   ; - store address of IDT
          mov   AX,60h                  ; - get segment for GDT
          mov   ES,AX                   ; - ...
          mov   EBX,2[ESP]              ; - get base address of IDT
          add   ESP,8                   ; - deallocate temporary
          mov   EAX,ES:(50h shl 3)[EBX] ; - get contents of IDT entry for 50h
          mov   EDX,ES:(50h shl 3)+4[EBX];-  ...
          movzx ECX,CL                  ; - calculate offset into IDT
          shl   ECX,3                   ; - ...
          mov   ES:[EBX+ECX],EAX        ; - set IDT entry
          mov   ES:4[EBX+ECX],EDX       ; - ...
          mov   CL,50h                  ; - restore interrupt 50
          pop   DS                      ; - set DS=ES saved above
          pop   EDX                     ; - set EDX=EBX saved above (pm int 50)
          pop   EBX                     ; - set EBX=EBX saved above (rm int 50)
          mov   AX,2507h                ; - ...
          int21h                        ; - ...
        _else                           ; else version 3.x or 4.x
          mov   AX,CS                   ; - set DS:EDX pointing to handler
          mov   DS,AX                   ; - (EDX set up above)
          mov   AX,2506h                ; - set interrupt handler
          int21h                        ; - grab IRQ 13 (8 on NEC)
        _endif                          ; endif
        pop     DS                      ; restore
        ret                             ; return
endproc __Phar_hook_init_


        xdefp   __Phar_hook_fini_
defp    __Phar_hook_fini_
        push    DS                      ; save DS
        mov     AX,250ch                ; get base interrupt vector for
        int21h                          ; ... IRQ8-IRQ15
        mov     CL,AH                   ; get interrupt # for IRQ 13 (8 on NEC)
;;      add     CL,13-8                 ; ... JBS 92/10/05
        add     CL,IRQ_NUM              ; ...
        sub     CL,8                    ; ...
        mov     EBX,__RMAddr            ; set EBX pointing to real-mode addr
        mov     EDX,__PMAddr            ; set DS:EDX pointing to old handler
        mov     DS,__PMSeg              ; ...
        mov     AX,2507h                ; ...
        int21h                          ; restore previous interrupt handler
        pop     DS                      ; restore DS
        ret                             ; return
endproc __Phar_hook_fini_

        endmod
        end
