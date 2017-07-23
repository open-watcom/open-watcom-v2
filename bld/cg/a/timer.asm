;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Timer tick DOS implementation
;*
;*****************************************************************************


        name    timer

include extender.inc

extrn "C",_Extender : byte

_text segment byte public 'code'

_data segment byte public 'data'
        meg1    dw      0
_data ends

dgroup group _data

assume cs:_text,ds:_data

;
;       GrabTimer() - grab the 18.2/sec timer interrupt
;

        public  GrabTimer_
GrabTimer_ proc near
        ret                             ; - return to caller
GrabTimer_ endp

;
;       ReleTimer_
;       - give back timer
;

        public  ReleTimer_
ReleTimer_ proc near
        ret                             ; - return to caller
        ret
ReleTimer_ endp

FIRST_MEG_SEL   equ     60H
BIOS_CLOCK      equ     46CH            ; 40:6C

;
;       uint    GetTickCnt(void)
;       - return the BIOS time of day counter
;

public  GetTickCnt_
GetTickCnt_ proc near
        cmp     ds:_Extender,X_RATIONAL ; rational?
        jne     pharlap
        mov     eax,BIOS_CLOCK
        mov     eax,[eax]               ; ds maps first meg directly
        ret
pharlap:
        mov     ax,meg1
        test    ax,ax
        jne     havesel
        push    ecx
        push    ebx
        sub     esp,512
        mov     ebx,esp
        mov     ecx,esp
        mov     ax,2526h                ; ask pharlap for selector for 1st meg
        int     21h
        mov     ax,108h[ebx]
        mov     meg1,ax
        add     esp,512
        pop     ebx
        pop     ecx
havesel:
        push    ds
        mov     ds,ax
        mov     eax,BIOS_CLOCK
        mov     eax,[eax]
        pop     ds
        ret
GetTickCnt_ endp

_text ends

        end
