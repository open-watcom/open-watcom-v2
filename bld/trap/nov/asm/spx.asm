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


.model compact
include mdef.inc
include struct.inc

        xref            _IPXInitialize_
        extrn           IPXLoc : dword

        modstart        SPX


        xdefp   SPXInitialize_
        defp    SPXInitialize_
        push    SI              ; save registers
        push    DI
        push    BP
        push    DS
        push    ES
        mov     BP,SP
        push    AX              ; save pointers for return info
        push    DX
        push    BX
        push    CX
        push    12[BP]          ; copy arg 3 & 4 down stack
        push    14[BP]
        push    16[BP]
        push    18[BP]
        call    _IPXInitialize_  ; initialize IPX system
        _guess
        test    AX,AX           ; - is IPX there?
        _quif   ne              ; - quit if not
        mov     BX,10H          ; - initialize SPX
        call    cs:IPXLoc       ; - . . .
        test    AL,AL           ; - did it init properly?
        _quif   e               ; - quit if not
        pop     DS              ; - get pointer to available connections
        pop     DI
        mov     [DI],DX         ; - save return value
        pop     DS              ; - get pointer to max connections
        pop     DI
        mov     [DI],CX         ; - save return value
        pop     DS              ; - get pointer to minor version num
        pop     DI
        mov     [DI],BL         ; - save return value
        pop     DS              ; - get pointer to major version num
        pop     DI
        mov     [DI],BH         ; - save return value
        mov     AL,0ffH         ; - return success indictor
        _admit                  ; admit
        add     SP,16           ; - clean up stack
        mov     AL,0            ; - return error indicator
        _endguess               ; endguess
        mov     AH,0            ; - zero high word of return
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        ret     8
        endproc SPXInitialize_

        xdefp   _SPXEstablishConnection_
        defp    _SPXEstablishConnection_
        push    SI              ; save registers
        push    DI
        push    BP
        push    DS
        push    ES
        mov     BP,SP
        push    CX              ; save connect id pointer
        push    BX
        mov     AH,DL           ; set watchdog flag
        mov     SI,12[BP]       ; set address to ECB
        mov     DS,14[BP]       ; ...
        mov     ES,14[BP]       ; ...
        mov     BX,11H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     BX              ; restore connect id pointer
        pop     CX
        mov     DS,CX
        mov     [BX],DX         ; store returned connect id
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        ret     4
        endproc _SPXEstablishConnection_

        xdefp   _SPXListenForConnection_
        defp    _SPXListenForConnection_
        push    CX              ; save registers
        push    BX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     AH,DL           ; set watchdog flag
        mov     SI,BX           ; set address to ECB
        mov     DS,CX           ; ...
        mov     ES,CX           ; ...
        mov     BX,12H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     BX
        pop     CX
        ret
        endproc _SPXListenForConnection_

        xdefp   _SPXTerminateConnection_
        defp    _SPXTerminateConnection_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,BX           ; set address to ECB
        mov     DS,CX           ; ...
        mov     ES,CX           ; ...
        mov     DX,AX           ; set connection id number
        mov     BX,13H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _SPXTerminateConnection_

        xdefp   _SPXAbortConnection_
        defp    _SPXAbortConnection_
        push    BX              ; save registers
        push    DX
        push    SI
        push    DI
        push    BP
        push    ES
        mov     DX,AX           ; set connection id number
        mov     BX,14H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     ES              ; restore registers
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     BX
        ret
        endproc _SPXAbortConnection_

        xdefp   _SPXSendSequencedPacket_
        defp    _SPXSendSequencedPacket_
        push    DX              ; save registers
        push    BX
        push    CX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,BX           ; set address to ECB
        mov     DS,CX           ; ...
        mov     ES,CX           ; ...
        mov     DX,AX           ; set connection id number
        mov     BX,16H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     CX
        pop     BX
        pop     DX
        ret
        endproc _SPXSendSequencedPacket_

        xdefp   _SPXListenForSequencedPacket_
        defp    _SPXListenForSequencedPacket_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,AX           ; set address to ECB
        mov     DS,DX           ; ...
        mov     ES,DX           ; ...
        mov     DX,AX           ; set connection id number
        mov     BX,17H          ; set function number
        call    cs:IPXLoc       ; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _SPXListenForSequencedPacket_

        xdefp   _SPXGetConnectionStatus_
        defp    _SPXGetConnectionStatus_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,BX           ; set address to CSB
        mov     DS,CX           ; ...
        mov     ES,CX           ; ...
        mov     DX,AX           ; set connection id number
        mov     BX,15H          ; set function number
        call    cs:IPXLoc       ; invoke function
        mov     AH,0            ; clear high part of return
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _SPXGetConnectionStatus_

        endmod
        end
