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


; CASMSUPP:     C front-end assembler coded support
;
; - these routines are easier to code in assembler than in C

        name    casmsupp

        public  InitHandlers_
        public  FiniHandlers_
        public  SetBreak_
        public  TBreak_


BREAK_INT       =       1bh
CRITICAL_INT    =       24h
DOS_SET_INT     =       25h
DOS_GET_INT     =       35h


; must be para because local CS data is accessed
casmsupp_TEXT   segment para public 'CODE'
                assume  cs:casmsupp_TEXT

BreakCount      dw      0
SaveBreakOff    dw      0
SaveBreakSeg    dw      0
SaveCriticalOff dw      0
SaveCriticalSeg dw      0

ifdef __EXPRESSC__
BreakKeyInterrupt       proc    far

        sti                     ; increment BreakCount
        inc     CS:BreakCount   ; sti, cli are necessary for atomic increment
        cli                     ;  operation
        iret

BreakKeyInterrupt       endp
endif
;
;       called from Ctrl_C_Handler
;

SetBreak_ proc  far
        inc     CS:BreakCount   ; increment BreakCount
        ret                     ; return
SetBreak_ endp


CriticalErrorInterrupt  proc    far

        sub     al,al           ; ignore critical error
        iret

CriticalErrorInterrupt  endp


TBreak_ proc    far

        sub     ax,ax
        xchg    ax,cs:BreakCount
        ret

TBreak_ endp


InitHandlers_   proc    far

        push    ax
        push    bx
        push    dx
        push    es
        push    ds
        mov     cs:BreakCount,0 ; BreakCount = 0
ifdef __EXPRESSC__
        mov     ah,DOS_GET_INT  ; get break key handler
        mov     al,BREAK_INT
        int     21h             ; returns in es:bx
        mov     cs:SaveBreakOff,bx ; save in SaveBreak
        mov     cs:SaveBreakSeg,es
        mov     dx,offset BreakKeyInterrupt
        mov     ax,cs
        mov     ds,ax
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,BREAK_INT    ; (might by modified by INT 21h)
        int     21h
endif
        mov     ah,DOS_GET_INT  ; get critical error handler
        mov     al,CRITICAL_INT
        int     21h             ; returns in es:bx
        mov     cs:SaveCriticalOff,bx ; save in SaveCritical
        mov     cs:SaveCriticalSeg,es
        mov     dx,offset CriticalErrorInterrupt
        mov     ax,cs
        mov     ds,ax
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,CRITICAL_INT ; (might by modified by INT 21h)
        int     21h
        pop     ds
        pop     es
        pop     dx
        pop     bx
        pop     ax
        ret

InitHandlers_   endp


FiniHandlers_   proc    far
        push    ax
        push    dx
        push    ds
ifdef __EXPRESSC__
        mov     dx,cs:SaveBreakOff
        mov     ds,cs:SaveBreakSeg
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,BREAK_INT
        int     21h
endif
        mov     dx,cs:SaveCriticalOff
        mov     ds,cs:SaveCriticalSeg
        mov     ah,DOS_SET_INT  ; set critical error handler
        mov     al,CRITICAL_INT
        int     21h
        pop     ds
        pop     dx
        pop     ax
        ret

FiniHandlers_   endp


casmsupp_TEXT    ends

        end
