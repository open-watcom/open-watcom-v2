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
;* Description:  Blip DOS implementation
;*
;*****************************************************************************


        name            blip

include extender.inc

extrn "C",_Extender : byte

_data segment byte public 'data'
        screenmem       dd 0
        selector        dw 0
_data ends

dgroup group _data

_text segment byte public 'code'

assume cs:_text,ds:_data

;
;               BlipInit()
;               - initialization to be done before calling Blip()
;
        public BlipInit_
BlipInit_ proc near
        cmp     ds:_Extender,X_RATIONAL ; rational?
        je      okinit
        ret
okinit:
        push    bx                      ; save bx
        mov     ah,0FH                  ; ...
        int     10H                     ; ...
        cmp     al,7                    ; if mode 7
        je      short ismono
        mov     ds:screenmem,0b8000h
        jmp     short done
ismono:
        mov     ds:screenmem,0b0000h
done:
        pop     bx                      ; restore bx
        ret                             ; return to caller
BlipInit_ endp

;
; Blip( location : int, char : byte )
;       - blip character to location in screen memory (row+col*80)
;
        public Blip_
Blip_   proc near
        cmp     ds:_Extender,X_RATIONAL ; rational?
        jne     pharlap
        add     eax,eax                 ; double location (for attrib)
        add     eax,ds:screenmem        ; screen memory!
        mov     ds:[eax],dl
        ret
pharlap:
        cmp     selector,0
        jne     havesel
        push    eax
        push    ebx
        push    ecx
        push    edx
        sub     esp,512
        mov     ebx,esp
        mov     ecx,esp
        mov     ax,2526h                ; ask pharlap for selector for 1st meg
        int     21h
        mov     ax,10ah[ebx]
        mov     selector,ax
        add     esp,512
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
havesel:
        push    es                      ; save regs
        push    esi                     ; ...
        add     eax,eax                 ; double location (for attrib)
        mov     esi,eax                 ; move to si
        mov     ax,selector             ; get seg of screen memory
        mov     es,ax                   ; ...
        mov     es:[si],dl              ; blip the character
        pop     esi                     ; restore regs
        pop     es                      ; ...
        ret                             ; return to caller
Blip_   endp

_text   ends

        end
