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
;*** TIMERCB.ASM - MMSYSTEM timeSetEvent interrupt callback function      ***
;***                                                                      ***
;****************************************************************************
.386p

extrn   _DataSelector   :WORD
extrn   _StackSelector  :WORD
extrn   TimerCB32       :DWORD

DGROUP group _DATA
_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment word public 'CODE' use16
assume  cs:_TEXT

        public  TimerCallBack_
TimerCallBack_ proc far
        push    BP                      ; save BP
        and     ESP,0000FFFFh           ; make sure high word of ESP is 0
        mov     EBP,ESP                 ; point to parms
        push    DS                      ; save DS
        push    ES                      ; save ES
        mov     AX,DGROUP               ; get access to DGROUP
        mov     DS,AX                   ; ...
        push    word ptr TimerCB32+4    ; push addr of 32-bit timer callback
        push    dword ptr TimerCB32+0   ; ...
        sub     EAX,EAX                 ; zero 32-bit reg
        mov     AX,20[BP]               ; get timer ID
        push    EAX                     ; push it
        mov     AX,18[BP]               ; get msg
        push    EAX                     ; push it
        push    dword ptr 14[BP]        ; push dwUser
        push    dword ptr 10[BP]        ; push dw1
        push    dword ptr 6[BP]         ; push dw2
        mov     ES,_DataSelector        ; point to 32-bit segment
        mov     DS,_DataSelector        ; ...
        call    fword ptr 24[BP]        ; call 32-bit timer callback
        mov     DS,-2[BP]               ; restore DS
        mov     ES,-4[BP]               ; restore ES
        mov     SP,BP                   ; clean up stack
        pop     BP                      ; restore BP
        ret                             ; return
TimerCallBack_ endp

_TEXT   ends
        end
