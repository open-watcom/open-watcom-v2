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


;****************************************************************************
;***                                                                      ***
;*** INT.ASM - handle interrupts for trap file                            ***
;***                                                                      ***
;****************************************************************************
.386p

extrn _FaultHandler:near
extrn TERMINATEAPP:far

DGROUP group _DATA
_DATA segment word public 'DATA'  use16
extrn _CSAlias:word
public _WasInt32
_WasInt32       dw      0
_DATA ends

;*
;*** the interrupt code lies here
;*
_TEXT segment word public 'CODE'  use16

assume cs:_TEXT
NewStack        LABEL FWORD
_NewESP dd      0
_NewSS  dw      0
_NewAX  dw      0
_NewIP  dw      0
_NewCS  dw      0
_NewFLAGS dw    0
_OldretCS dw    0
_OldretIP dw    0
_Oldintnumber dw        0
_Oldhandle dw   0
_RetHow dw      0
public _NewSS,_NewESP,_NewAX, _NewIP, _NewCS, _NewFLAGS, _OldretCS
public _OldretIP, _Oldintnumber, _Oldhandle,_RetHow
;**************************************************************************
;***                                                                    ***
;*** IntHandler - takes all interrupts                                  ***
;***                                                                    ***
;*** entry frame:                                                       ***
;***                                                                    ***
;***     BP---->|  Old BP  |  [BP + 00h]                                ***
;***            |  Ret IP  |  [BP + 02h]                                ***
;***            |  Ret CS  |  [BP + 04h]                                ***
;***            |    AX    |  [BP + 06h]                                ***
;***            |Exception#|  [BP + 08h]                                ***
;***            |  Handle  |  [BP + 0Ah]                                ***
;***            |    IP    |  [BP + 0Ch]                                ***
;***            |    CS    |  [BP + 0Eh]                                ***
;***            |   Flags  |  [BP + 10h]                                ***
;***                                                                    ***
;**************************************************************************

public INTHANDLER
INTHANDLER PROC far

        push    ebp
        mov     bp,sp
        pushad
        push    ds
        push    es
        push    fs
        push    gs
        push    ss

;        mov     ds,ax

        ;* Stack frame to function is
        ;*
                ;*       BP---->|  Old BP  |
        ;*              |    SS    |
        ;*              |    GS    |
        ;*              |    FS    |
        ;*              |    ES    |
        ;*              |    DS    |
        ;*              |    EDI   |
        ;*              |    ESI   |
        ;*              |    EBP   |
        ;*              | Dummy ESP|
        ;*              |    EBX   |
        ;*              |    EDX   |
        ;*              |    ECX   |
        ;*              |    EAX   |
        ;*   Old BP---->|Old,Old BP|
        ;*              |  Ret IP  |
        ;*              |  Ret CS  |
        ;*              |    AX    |
        ;*              |Exception#|
        ;*              |  Handle  |
        ;*              |    IP    |
        ;*              |    CS    |
        ;*              |   Flags  |
                ;**
        call    _FaultHandler

        pop     ax                      ; was SS
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        pop     ebp

        cmp     cs:_NewSS,0             ; is there a new stack?
        je      skiprl                  ; nope
        lss     esp,cs:NewStack         ; yes, load it
        push    cs:_NewFlags            ; push on all the values
        push    cs:_NewCS               ;   that were on the
        push    cs:_NewIP               ;      stack before
        push    cs:_Oldhandle
        push    cs:_Oldintnumber
        push    cs:_NewAX
        push    cs:_OldretCS
        push    cs:_OldretIP
skiprl:
        cmp     cs:_RetHow,0            ; kill application?
        je      KillApp
        cmp     cs:_RetHow,1
        je      RestartApp              ; restart application?

        retf                            ; neither, must chain

RestartApp:
        add     sp,10                   ; Point to IRET frame
        iret                            ; Restart instruction

KillApp:
        add     sp,10                   ; Point to IRET frame
        push    0                       ; kill current task
        push    1                       ; NO_UAE_BOX
        call    TERMINATEAPP
        retf

INTHANDLER endp

_TEXT ends
        end
