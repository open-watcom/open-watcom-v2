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
;* Description:  Handle interrupts for Protected Mode Windows debugger.
;*
;*****************************************************************************


.386p

;** Set up values for cMacros

;** TOOLHELP.INC is not included in OpenWatcom
;** but we need only the declaration of NO_UAE_BOX
;**        INCLUDE TOOLHELP.INC

;** TerminateApp() flag values (taken from TOOLHELP.INC)
NO_UAE_BOX      EQU     1


extrn _FaultHandler:FAR
extrn TerminateApp:FAR

DGROUP group _DATA
_DATA segment word public 'DATA' use16
_DATA ends

;*
;*** the interrupt code lies here
;*
_TEXT segment word public 'CODE' use16

assume cs:_TEXT


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

PUBLIC IntHandler
IntHandler PROC FAR
        nop
        nop
        nop

        push    ebp                     ;Make a stack frame
        mov     bp,sp
        pushad                          ;Save all registers
        push    ds
        push    es
        push    fs
        push    gs
        push    ss

        mov     ds,ax
;       mov     ax,SEG DGROUP
        mov     ds,ax

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

        cmp     ax,0                    ; kill application?
        je      KillApp
        cmp     ax,1
        je      RestartApp              ; restart application?

        pop     ax                      ; was SS
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        pop     ebp
        retf

RestartApp:
        pop     ax                      ; was SS
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        pop     ebp
        add     sp,10                   ; Clear the return stuff
        iret                            ; Restart instruction

KillApp:
        pop     ax                      ; was SS
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        pop     ebp
        add     sp,10                   ; Point to IRET frame
        push    0
        push    NO_UAE_BOX
        call    TerminateApp
        iret
IntHandler ENDP


_TEXT ends
        end

