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
; PType - Determine Processor Type                                     *
;                                                                      *
; int PType()                                                          *
;       returns:                                                       *
;               0 - 8086/8088/NECV30/NECV20                            *
;               1 - 80186/80188                                        *
;               2 - 80286                                              *
;               3 - 80386                                              *
;               4 - 486                                                *
;               5 - Pentium                                            *
;                                                                      *
;***********************************************************************
.386
DOS     equ     21h

_TEXT   segment byte public 'code' use16
        assume  CS:_TEXT

        public  PType_
PType_  proc    near
        PUSH    BP                      ; save BP
        PUSH    ES                      ; save our ES value
        PUSH    DS                      ; save our DS value
        PUSH    DX                      ; save DX
        PUSH    CX                      ; save CX
        PUSH    BX                      ; save BX
        MOV     BP,SP                   ; set new frame pointer
        MOV     AX,0FFFFh               ; do the shift trick
        MOV     CL,33                   ; shift right 33 times
        SHR     AX,CL                   ; AX will be 0 for 8088/8086 types
        JZ      short PTExit            ; quit if 808x processor
        MOV     AX,1                    ; assume 8018x processor
        PUSH    SP                      ; do the stack trick
        POP     BX
        CMP     BX,SP                   ; quit if stack value was post dec value
        JNZ     short PTExit            ; quit if 8018x processor
;       MOV     AX,3506h                ; get current INT 6 handler
;       INT     DOS
;       MOV     word ptr CS:PTOff,BX    ; save old handler offset
;       MOV     word ptr CS:PTSeg,ES    ; save old handler segment
;       PUSH    CS                      ; set DS=CS
;       POP     DS
;       MOV     DX,offset _TEXT:PTBadIns ; get address of our INT 6 handler
;       MOV     AX,2506h                ; install our INT 6 handler
;       INT     DOS
;       DB      66h                     ; execute an 80386 length instruction
;       NOP                             ; no-op won't use length modifier
;       MOV     AX,3                    ; processor is an 386/486 if we get here
;PTRest:PUSH    AX                      ; save processor type
;       LDS     DX,dword ptr CS:PTOff   ; get old segment:offset values
;       MOV     AX,2506h                ; restore previous INT 6 handler
;       INT     DOS
;       POP     AX                      ; restore processor type

        PUSHF                           ; save flags
        MOV     AX,0F000H               ; try to turn on top bits in flags
        PUSH    AX                      ; ...
        POPF                            ; ...
        PUSHF                           ; see if any turned on
        POP     CX                      ; ...
        POPF                            ; restore flags
        MOV     AX,2                    ; assume 286
        TEST    CX,0F000H               ; are any bits on?
        JE      short PTExit            ; if not, it's a 286
        MOV     DX,SP                   ; save sp
        AND     SP,not 3                ; align to dword boundary
        PUSHFD                          ; pushfd
        PUSHFD                          ; pushfd
        POP     BX                      ; get flag values
        POP     CX                      ; ...
        XOR     CX,4                    ; flip AC bit
        PUSH    CX                      ; put new flags back
        PUSH    BX
        POPFD                           ; POPFD
        PUSHFD                          ; PUSHFD
        POP     BX                      ; get flipped flags
        POP     BX                      ; ...
        POPFD                           ; restore flags
        MOV     SP,DX                   ; restore SP
        MOV     AX,3                    ; assume it's a 386
        XOR     BX,CX                   ; did AC stayed flipped?
        TEST    BX,4                    ; ???
        JNE     short PTExit            ; quit if not
        AND     SP,not 3                ; align to dword boundary
        PUSHFD                          ; pushfd
        PUSHFD                          ; pushfd
        POP     BX                      ; get flag values
        POP     CX                      ; ...
        XOR     CX,20h                  ; flip ID bit
        PUSH    CX                      ; put new flags back
        PUSH    BX
        POPFD                           ; POPFD
        PUSHFD                          ; PUSHFD
        POP     BX                      ; get flipped flags
        POP     BX                      ; ...
        POPFD                           ; restore flags
        MOV     SP,DX                   ; restore SP
        MOV     AX,4                    ; assume it's a 486
        XOR     BX,CX                   ; did ID stayed flipped?
        TEST    BX,20h                  ; ???
        JNE     short PTExit            ; quit if not
        mov     eax,1                   ; get processor info
        db      0fh,0a2h                ; CPUID
        mov     al,ah                   ; get cpu number
        mov     ah,0                    ; ...

PTExit: POP     BX                      ; restore BX
        POP     CX                      ; restore CX
        POP     DX                      ; restore DX
        POP     DS                      ; restore old DS value
        POP     ES                      ; restore old ES value
        POP     BP                      ; restore BP
        RET                             ; return to caller

;       Illegal instruction trap handler

;PTBadIns:
;        STI
;        MOV     AX,2                    ; processor is an 80286 if we get here
;        JMP     PTRest                  ; since illegal instruction executed
;
;PTOff:  dw      0
;PTSeg:  dw      0

PType_  endp
_TEXT   ends
        end
