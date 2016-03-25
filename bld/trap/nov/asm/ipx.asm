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

        xrefp   SAPWait_
        xrefp   SAPBroad_
        xrefp   ServResp_

        modstart        IPX

DSSeg   dw      ?
        public          IPXLoc
IPXLoc  dd      ?

        public  SAPWaitESR_
SAPWaitESR_     proc far
        mov     DS,CS:DSSeg     ; restore data segment value
        sti                     ; enable interrupts
        call    SAPWait_        ; respond to the broadcast
        ret                     ; return to caller
SAPWaitESR_     endp

        public  SAPBroadESR_
SAPBroadESR_    proc far
        mov     DS,CS:DSSeg     ; restore data segment value
        sti                     ; enable interrupts
        call    SAPBroad_       ; respond to the broadcast
        ret                     ; return to caller
SAPBroadESR_    endp

        public  ServRespESR_
ServRespESR_    proc far
        mov     DS,CS:DSSeg     ; restore data segment value
        sti                     ; enable interrupts
        call    ServResp_       ; respond to the server request
        ret                     ; return to caller
ServRespESR_    endp

IPXCall proc    far
        int     7aH             ; interrupt into IPX (old-fashioned)
        ret
IPXCall endp

        xdefp   _IPXInitialize_
        defp    _IPXInitialize_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    ES
        mov     CS:DSSeg,DS     ; save data segment value
        mov     AX,7a00H        ; see if IPX is installed
        int     2fH             ; ...
        _guess                  ; guess
        cmp     AL,0ffH         ; - is indirect call location there ?
        _quif   ne              ; - quit if not
        mov     word ptr cs:IPXLoc+0,DI; - save indirect call pointer
        mov     word ptr cs:IPXLoc+2,ES; - ...
        xor     AX,AX           ; - return success
        _admit                  ; admit
        clc                     ; - clear carry
        mov     CX,0ffffH       ; - set CX to illegal value
        mov     AX,0DCffH       ; - novell GetConnectionNumber function
        int     21H             ; - check if novell is present
        _quif   c               ; - quit if carry is set
        aam                     ; - convert number in AL to BCD in AX
        add     AX,3030H        ; - convert BCD to ASCII
        xchg    CL,CH           ; - get value in CX in right order
        cmp     CX,AX           ; - do the numbers match ?
        _quif   ne              ; - . . .
        ; we have a working novell network - use 7aH interrupt
        mov     AX,offset IPXCall;- setup pointer to
        mov     word ptr cs:IPXLoc+0,AX;- interface routinte
        mov     word ptr cs:IPXLoc+2,CS;- ...
        xor     AX,AX           ; - return success
        _admit                  ; admit
          mov   AX,0f0H         ; - return not installed error
        _endguess               ; endguess
        pop     ES              ; restore registers
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXInitialize_

        xdefp   _IPXOpenSocket_
        defp    _IPXOpenSocket_
        push    CX              ; save registers
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        push    DX              ; save address of socket variable
        push    AX
        push    BX              ; save socket type
        mov     BX,AX           ; get pointer to socket var
        mov     DS,DX
        pop     AX              ; set socket type
        mov     DX,[BX]         ; get requested socket number
        mov     BX,00H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     BX              ; restore pointer to socket variable
        pop     DS
        mov     [BX],DX         ; store given socket number
        xor     AH,AH           ; clear high byte of return code
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     CX
        ret
        endproc _IPXOpenSocket_

        xdefp   _IPXCloseSocket_
        defp    _IPXCloseSocket_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    ES
        mov     DX,AX           ; set socket number
        mov     BX,01H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXCloseSocket_

        xdefp   _IPXGetLocalTarget_
        defp    _IPXGetLocalTarget_
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,AX           ; set address to ECB
        mov     DS,DX           ; ...
        mov     DI,BX           ; set address to local node buffer
        mov     ES,CX           ; ...
        mov     BX,02H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        mov     BP,SP
        mov     DS,14[BP]
        mov     BX,12[BP]
        mov     [BX],CX         ; store transit time result
        xor     AH,AH           ; clear high byte of return value
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        ret     4
        endproc _IPXGetLocalTarget_

;        xdefp   IPXGetLocalTarget_
;        defp    IPXGetLocalTarget_
;        push    CX              ; save registers
;        push    SI
;        push    DI
;        push    BP
;        push    ES
;        mov     SI,AX           ; set address to ECB
;        mov     DI,DX           ; set address to local node buffer
;        push    DS              ; ...
;        pop     ES              ; ...
;        push    BX              ; save pointer to transit time
;        mov     BX,02H          ; set function number
;        call    dword ptr cs:IPXLoc; invoke function
;        pop     BX              ; restore pointer to transit time
;        mov     [BX],CX         ; store transit time result
;        xor     AH,AH           ; clear high byte of return value
;        pop     ES              ; restore registers
;        pop     BP
;        pop     DI
;        pop     SI
;        pop     CX
;        ret
;        endproc IPXGetLocalTarget_

        xdefp   _IPXSendPacket_
        defp    _IPXSendPacket_
        push    BX              ; save registers
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,AX           ; set address to ECB
        mov     DS,DX           ; ...
        mov     ES,DX           ; ...
        mov     BX,03H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     BX
        ret
        endproc _IPXSendPacket_

        xdefp   _IPXListenForPacket_
        defp    _IPXListenForPacket_
        push    BX              ; save registers
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,AX           ; set address to ECB
        mov     DS,DX           ; ...
        mov     ES,DX           ; ...
        mov     BX,04H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     BX
        ret
        endproc _IPXListenForPacket_

        xdefp   _IPXScheduleIPXEvent_
        defp    _IPXScheduleIPXEvent_
        push    BX              ; save registers
        push    CX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     SI,BX           ; set address to ECB
        mov     DS,CX           ; ...
        mov     ES,CX           ; ...
        mov     BX,05H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     CX
        pop     BX
        ret
        endproc _IPXScheduleIPXEvent_

        xdefp   _IPXCancelEvent_
        defp    _IPXCancelEvent_
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
        mov     BX,06H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        xor     AH,AH           ; clear high byte of return code
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXCancelEvent_

        xdefp   _IPXGetIntervalMarker_
        defp    _IPXGetIntervalMarker_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    ES
        mov     BX,08H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXGetIntervalMarker_

        xdefp   _IPXGetInternetworkAddress_
        defp    _IPXGetInternetworkAddress_
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
        mov     BX,09H          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXGetInternetworkAddress_

        xdefp   _IPXRelinquishControl_
        defp    _IPXRelinquishControl_
        push    BX              ; save registers
        push    CX
        push    DX
        push    SI
        push    DI
        push    BP
        push    DS
        push    ES
        mov     BX,0aH          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXRelinquishControl_

        xdefp   _IPXDisconnectFromTarget_
        defp    _IPXDisconnectFromTarget_
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
        mov     BX,0bH          ; set function number
        call    dword ptr cs:IPXLoc; invoke function
        pop     ES              ; restore registers
        pop     DS
        pop     BP
        pop     DI
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
        endproc _IPXDisconnectFromTarget_

        endmod
        end
