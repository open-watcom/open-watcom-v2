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


;***********************************************************************
;                                                                      *
; PBus - Determine Processor Bus Width                                 *
;        for 8088/8086/80188/80186/NECV30/NECV20 Processors            *
;                                                                      *
; int PBus()                                                           *
;       returns:                                                       *
;               0 - 16 bits                                            *
;               1 - 8 bits                                             *
;                                                                      *
;***********************************************************************
NOPINS  equ     90h
DECBXINS equ    4Bh

_TEXT   segment byte public 'code'
        assume  CS:_TEXT

        public  PBus_
PBus_   proc    near
        PUSH    ES                      ; save ES
        PUSH    DI                      ; save DI
        PUSH    CX                      ; save CX
        PUSH    BX                      ; save BX
        PUSH    CS                      ; set ES=CS
        POP     ES                      ; ...
        CLD                             ; set direction forward
        CLI                             ; disable interrupts
        MOV     DI,offset _TEXT:PBFini  ; target for our STOSB
        MOV     byte ptr ES:[DI],DECBXINS ; self-modifying (yech)
        MOV     CX,0101h                ; the job to do
        MOV     BX,1                    ; assume 8-bit bus
        MOV     AL,NOPINS
        MUL     CX
        STOSB                           ; while we are waiting let's
        NOP                             ; start filling the instruction queue
        NOP                             ; 4-byte queue it's an 8088/80188
        NOP                             ; 6-byte queue it's an 8086/80186
        NOP
PBFini: DEC     BX                      ; instruction to zonk
        MOV     AX,BX                   ; ta-da! BX is either 1 or it's 0
        STI                             ; re-enable interrupts
        POP     BX                      ; restore BX
        POP     CX                      ; restore CX
        POP     DI                      ; restore DI
        POP     ES                      ; restore ES
        RET                             ; return to caller

PBus_   endp
_TEXT   ends
        end
