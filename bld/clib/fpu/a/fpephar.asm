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
;* Description:  FPE interrupt handler hook for PharLap DOS extender.
;*
;*****************************************************************************


;
; FPEPHAR       : connect/disconnect 80x87 interrupt handler under
;                 the Pharlap 386|DOS-Extender (RUN386)
;
; For the IBM PC, we use IRQ13 (math coprocessor exception)
; Level     Function
; NMI       Parity, Watchdog Timer,
;           Arbitration time-out,
;           Channel Check
; IRQ 0     Timer
; IRQ 1     Keyboard
; IRQ 2     Cascaded Interrupt Control -- IRQ  8 Real Time Clock
;                                         IRQ  9 Redirect Cascade
;                                         IRQ 10 Reserved
;                                         IRQ 11 Reserved
;                                         IRQ 12 Mouse
;                                         IRQ 13 Math Coprocessor Exception
;                                         IRQ 14 Fixed Disk
;                                         IRQ 15 Reserved
; IRQ 3     Serial Alternate
; IRQ 4     Serial Primary
; IRQ 5     Reserved
; IRQ 6     Diskette
; IRQ 7     Parallel Port
; IRQ 8 through 15 are cascaded through IRQ 2
;
; IRQ 0 through  7 map to INT 08 through INT 0F
; IRQ 8 through 15 map to INT 70 through INT 77
;
;
; For the NEC PC (286, 386), we use IRQ 8 (math coprocessor exception)
; Level     Function                                Vector Number
; IRQ  0     Timer                                  08
; IRQ  1     Keyboard                               09
; IRQ  2     CTRV                                   0A
; IRQ  3     Expansion bus INT0                     0B
; IRQ  4     Serial Primary (RS-232C ch0)           0C
; IRQ  5     Expansion bus INT1                     0D
; IRQ  6     Expansion bus INT2                     0E
; IRQ  7     Slave                                  0F
; IRQ  8     Math coprocessor (286, 386, ...)       10
; IRQ  9     Expansion bus INT3 (HD, HDLC)          11
; IRQ 10     Expansion bus INT41 (640KB FD)         12
; IRQ 11     Expansion bus INT42 (1MB FD)           13
; IRQ 12     Expansion bus INT5 (RS-232C ch1, ch2)  14
; IRQ 13     Expansion bus INT6 (mouse)             15
; IRQ 14     Reserved (math coprocessor in 70116)   16
; IRQ 15     Reserved                               17
;
; IRQ 0 through  7 map to INT 08 through INT 0F
; IRQ 8 through 15 map to INT 10 through INT 17

.8087
.386p

include struct.inc
include mdef.inc
include extender.inc

        xrefp           __FPEHandler_   ; handle exceptions
        xrefp           __FPE2Handler_  ; handle exceptions w/o OUT's

        modstart        fpephar

        datasegment

        extrn   "C",_Extender   : byte
        extrn   "C",__RMAddr    : dword
        extrn   "C",__PMAddr    : dword
        extrn   "C",__PMSeg     : word
        extrn   "C",__IRQ_num   : byte  ; defined in \clib\fpu\c\fpeint.c
        extrn   "C",__MST_pic   : byte  ; defined in \clib\fpu\c\fpeint.c

        enddata


        xdefp   __Phar_hook_init_
defp    __Phar_hook_init_
        push    DS                      ; save DS
        cmp     byte ptr __MST_pic,20h  ; if IBM PC master PIC port number
        _if     e                       ; then
          lea   EDX,__FPEHandler_       ; - point at handler with OUT's
        _else                           ; else (NEC or some other non-IBM PC)
          lea   EDX,__FPE2Handler_      ; - point at handler with no OUT's
        _endif                          ; endif
        cmp     byte ptr _Extender,X_PHARLAP_V3 ; if version 2.x
        _if     b                       ; - then
          mov   CL,50h                  ; - get contents of interrupt 50h
          mov   AX,2503h                ; - get real-mode interrupt vector
          int   21h                     ; - ...
          push  EBX                     ; - save rm int 50
          mov   AX,2502h                ; - get protected-mode interrupt vector
          int   21h                     ; - ...
          push  EBX                     ; - save pm int 50
          push  ES                      ; - ...
          push  DS                      ; - save DS
          mov   AX,CS                   ; - set DS:EDX pointing at handler
          mov   DS,AX                   ; - (EDX set up above)
          mov   AX,2506h                ; - set new interrupt handler
          int   21h                     ; - ...
          pop   DS                      ; - restore DS
        _endif                          ; endif
        mov     AX,250ch                ; get base interrupt vector for
        int     21h                     ; ... IRQ8-IRQ15
        mov     CL,AH                   ; get interrupt # for IRQ 13 (8 on NEC)
;;      add     CL,13-8                 ; ... JBS 92/10/05
        add     CL,__IRQ_num            ; ...
        sub     CL,8                    ; ...
        mov     AX,2502h                ; get protected-mode interrupt vector
        int     21h                     ; ...
        mov     __PMAddr,EBX            ; save protected-mode interrupt vector
        mov     __PMSeg,ES              ; ...
        mov     AX,2503h                ; get real-mode interrupt vector
        int     21h                     ; ...
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
          int   21h                     ; - ...
        _else                           ; else version 3.x or 4.x
          mov   AX,CS                   ; - set DS:EDX pointing to handler
          mov   DS,AX                   ; - (EDX set up above)
          mov   AX,2506h                ; - set interrupt handler
          int   21h                     ; - grab IRQ 13 (8 on NEC)
        _endif                          ; endif
        pop     DS                      ; restore
        ret                             ; return
endproc __Phar_hook_init_


        xdefp   __Phar_hook_fini_
defp    __Phar_hook_fini_
        push    DS                      ; save DS
        mov     AX,250ch                ; get base interrupt vector for
        int     21h                     ; ... IRQ8-IRQ15
        mov     CL,AH                   ; get interrupt # for IRQ 13 (8 on NEC)
;;      add     CL,13-8                 ; ... JBS 92/10/05
        add     CL,__IRQ_num            ; ...
        sub     CL,8                    ; ...
        mov     EBX,__RMAddr            ; set EBX pointing to real-mode addr
        mov     EDX,__PMAddr            ; set DS:EDX pointing to old handler
        mov     DS,__PMSeg              ; ...
        mov     AX,2507h                ; ...
        int     21h                     ; restore previous interrupt handler
        pop     DS                      ; restore DS
        ret                             ; return
endproc __Phar_hook_fini_

        endmod
        end
