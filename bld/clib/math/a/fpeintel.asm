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

        xref            __FPEHandler_   ; handle exceptions
        xref            __FPE2Handler_  ; handle exceptions w/o OUT's

        modstart        fpeintel

        datasegment

        extrn   "C",__RMAddr    : dword
        extrn   "C",__PMAddr    : dword
        extrn   "C",__PMSeg     : word
        extrn   "C",__FPE_int   : byte  ; defined in \clib\math\c\fpeint.c
        extrn   "C",__MST_pic   : byte  ; defined in \clib\math\c\fpeint.c

        enddata

        xdefp   __Intel_hook_init_
defp    __Intel_hook_init_
        mov     AX,0204h                ; get old interrupt contents
        mov     BL,__FPE_int            ; get interrupt number
        int     31h                     ; ...
        mov     __PMAddr,EDX            ; save protected mode interrupt vector
        mov     __PMSeg,CX              ; ...
        mov     AX,0200h                ; get contents of real-mode vector
        mov     BL,__FPE_int            ; get interrupt number
        int     31h                     ; ...
        shl     ECX,16                  ; shift segment to top
        mov     CX,DX                   ; put in offset
        mov     __RMAddr,ECX            ; save real-mode interrupt
        mov     CX,CS                   ; get address of handler
        cmp     byte ptr __MST_pic,20h  ; if IBM PC master PIC port number
        _if     e                       ; then
          lea   EDX,__FPEHandler_       ; - point at handler with OUT's
        _else                           ; else (NEC or some other non-IBM PC)
          lea   EDX,__FPE2Handler_      ; - point at handler with no OUT's
        _endif                          ; endif
        mov     AX,0205h                ; set interrupt vector
        mov     BL,__FPE_int            ; get interrupt number
        int     31h                     ; ...
        ret                             ; return
endproc __Intel_hook_init_


        xdefp   __Intel_hook_fini_
defp    __Intel_hook_fini_
        ret                             ; return
endproc __Intel_hook_fini_

        endmod
        end
