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


.386

include struct.inc

                name            timer

extrn           dbg_rdides      : near
extrn           dbg_wrides      : near

idtentry        struc
                loffs   dw      0
                select  dw      0
                wcount  db      0
                arights db      0
                hoffs   dw      0
idtentry        ends

extrn           RecordSample_   : near
extrn           DummyCGraph_    : near
extrn           _XVersion       : byte

_data segment word public 'data'
timer           idtentry        <0,0,0,0,0>

tmloffs         dw              0
tmhoffs         dw              0
tmselect        dw              0
tmarights       db              0,0

dataseg         dw              0
savefl          dd              0
savecs          dd              0
saveeip         dd              0
saveesp         dd              0
iTMoff          dd              0
iTMseg          dw              0
extrn           _InitialCS      : word
extrn           _InitialSS      : word
extrn           _InsiderTime    : dword
extrn           _SampleIndex    : dword
extrn           _LastSampleIndex: dword
extrn           _Margin         : dword
extrn           _Ceiling        : dword
extrn           _SamplerOff     : dword
extrn           _CallGraphMode  : byte
extrn           _CGBreak        : byte
extrn           _FakeBreak      : byte
extrn           _SavedByte      : byte
extrn           _LostData       : byte
_data ends


dgroup group _data

assume  ds:dgroup,cs:_text


_text segment byte public 'code'

public          inttm
inttm           proc    near
                push    eax                     ; save some regs
                push    ds                      ; ...
                mov     ds,cs:dataseg           ; get addressability
                inc     _InsiderTime            ; ++InsiderTime
                mov     eax,08H[esp]            ; remember the old return frame
                mov     saveeip,eax             ; ...
                mov     eax,0CH[esp]            ; ...
                mov     savecs,eax              ; ...
                mov     eax,10H[esp]            ; ...
                mov     savefl,eax              ; ...
                mov     saveesp,esp             ; remember esp
                add     saveesp,5*4             ; . after we've cleared it
                pop     ds                      ; restore some regs
                pop     eax                     ; ...
                add     esp,0CH                 ; get rid of old return frame

                cmp     cs:_XVersion,3          ; if version >= 3
                _if     ge                      ; - then
                pushfd                          ; - invoke the old handler
                push    cs                      ; - ...
                push    offset return           ; - ...
                jmp     l1                      ; - bug in the chip
l1:             jmp     fword ptr cs:iTMoff     ; - ...
                _else                           ; else
                pushfd                          ; - push flags
                jmp     l2                      ; - BUG IN THE CHIP!!! (don't ask)
l2:             call    fword ptr cs:tmloffs    ; - chain to old handler
                _endif                          ; endif

return:         mov     esp,cs:saveesp          ; old handler might not pop FL!

                push    ds                      ; ...
                push    es                      ; ...
                push    eax                     ; save a register
                push    ebx                     ; and another
                push    edx                     ; and another
                push    ecx                     ; and another
                mov     es,cs:dataseg           ; get addressability
                mov     ds,cs:dataseg           ; ...
                cmp     _XVersion,3             ; if version >= 3
                _if     ge                      ; - then
                mov     ebx,18h[esp]            ; - get return address
                movzx   ecx,word ptr 1ch[esp]   ; - ...
                _else                           ; else
                mov     ebx,saveeip             ; - get return address
                mov     ecx,savecs              ; - ...
                _endif                          ; endif
                cmp     cx,_InitialCS           ; if not in users address space
;               _if     ne                      ; - then
;               mov     edx,1                   ; - set segment to non-existant!
;               xor     eax,eax                 ; - and offset to 0
;               _else                           ; else
                mov     eax,ebx                 ; - parm is return address
                mov     edx,ecx                 ; - ...
;               _endif                          ; endif
                call    RecordSample_           ; remember it
                cmp     _InsiderTime,1          ; if InsiderTime == 1
                _if     e                       ; - then
                cmp     _SamplerOff,0           ; - if SamplerOff == 0
                _if     e                       ; - - then
                cmp     _CallGraphMode,0        ; - - if CallGraphMode
                _if     ne                      ; - - - then
                cmp     cx,_InitialCS           ; - - - if in users code
                _if     e                       ; - - - - then
                mov     _CGBreak,1              ; - - - - indicate a cgraph break
                mov     ax,_InitialSS           ; - - - - get byte at return
                mov     es,ax                   ; - - - - ...
                mov     al,es:[ebx]             ; - - - - ... get byte at return
                mov     _SavedByte,al           ; - - - - remember it
                mov     byte ptr es:[ebx],0CCH  ; - - - - plant a break point
                _else                           ; - - - else
                call    DummyCGraph_            ; - - - - store dummy record
                _endif                          ; - - - endif
                _endif                          ; - - endif
                _endif                          ; - endif
                mov     eax,_SampleIndex        ; - if SampleIndex >= Margin
                cmp     eax,_Margin             ; - ...
                _if     ge                      ; - - then
                cmp     cx,_InitialCS           ; - - if not in users code
                _if     e                       ; - - - then
                mov     _FakeBreak,1            ; - - - indicate a "fake" break
                mov     ax,_InitialSS           ; - - - get byte at return
                mov     es,ax                   ; - - - ...
                mov     al,es:[ebx]             ; - - - ... get byte at return
                mov     _SavedByte,al           ; - - - remember it
                mov     byte ptr es:[ebx],0CCH  ; - - - plant a break point
                _endif                          ; - - endif
                mov     eax,_SampleIndex        ; - - if( SampleIndex >= Ceiling
                cmp     eax,_Ceiling            ; - - - then
                _if     ge                      ; - - - ...
                cmp     _CallGraphMode,0        ; - - - if( CallGraphMode )
                _if     ne                      ; - - - - then
                dec     _SampleIndex            ; - - - - --SampleCount
                mov     eax,_LastSampleIndex    ; - - - - SampleIndex = LastSampleIndex
                mov     _SampleIndex,eax        ; - - - - ...
                _else                           ; - - - else
                dec     _SampleIndex            ; - - - - --SampleIndex
                _endif                          ; - - - endif
                mov     _LostData,1             ; - - - LostData = TRUE
                _endif                          ; - - endif
                _endif                          ; - endif
                _endif                          ; endif
                dec     dword ptr _InsiderTime  ; --InsiderTime
                pop     ecx                     ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     eax                     ; ...
                pop     es                      ; restore registers
                pop     ds                      ; ...
                push    cs:savefl               ; restore stack frame
                push    cs:savecs               ; ...
                push    cs:saveeip              ; ...
                jmp     l3
l3:             iretd
inttm           endp

public          GrabVects_
GrabVects_      proc near
                push    ecx                     ; save registers
                push    ebx                     ; ...
                push    edx                     ; ...
                push    es                      ; ...
                push    ds                      ; ...

                mov     dataseg,ds              ; save data segment

                mov     ebx,'PHAR'              ; get Dos extender version
                mov     ah,30H                  ; ...
                int     21H                     ; ...
                sub     bl,'0'                  ; ...
                mov     _XVersion,bl            ; ...

                mov     ax,250cH                ; get hw interrupt vectors
                int     21H                     ; ...
                movzx   eax,al                  ; ... extend
                push    eax                     ; save it

                cmp     _XVersion,3             ; if version >= 3
                _if     ge                      ; - then
                mov     ax,2502H                ; - get old vector
                pop     ecx                     ; - ...
                push    ecx
                int     21H                     ; - ...
                mov     iTMseg,es               ; - stash it
                mov     iTMoff,ebx              ; - ...
                pop     ecx                     ; - ...
                mov     ax,2504H                ; - set new vector
                lea     edx,inttm               ; - ...
                push    cs                      ; - ...
                pop     ds                      ; - ...
                int     21H                     ; - ...
                _else                           ; else
                pop     ebx                     ; - point to the IDT entry
                shl     ebx,3                   ; - ...
                mov     ax,50h                  ; - ...
                mov     es,ax                   ; - ...
                cli                             ; - interrupts off
                mov     ax,es:[ebx].hoffs       ; - save IDT entry
                mov     tmhoffs,ax              ; - ...
                mov     ax,es:[ebx].loffs       ; - ...
                mov     tmloffs,ax              ; - ...
                mov     ax,es:[ebx].select      ; - ...
                mov     tmselect,ax             ; - ...
                mov     al,es:[ebx].arights     ; - ...
                mov     tmarights,al            ; - ...
                lea     eax,inttm               ; - set new timer handler
                mov     es:[ebx].loffs,ax       ; - ...
                shr     eax,16                  ; - ...
                mov     es:[ebx].hoffs,ax       ; - ...
                mov     es:[ebx].select,cs      ; - ...
                mov     es:[ebx].arights,08EH   ; - ...
                sti                             ; - interrupts back on
                _endif                          ; endif
epi:            pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
GrabVects_      endp

public          ReleVects_
ReleVects_      proc near
                push    ecx                     ; save registers
                push    edx                     ; ...
                push    ebx                     ; ...
                push    ds                      ; ...
                push    es                      ; ...

                cmp     _XVersion,3             ; if version >= 3
                _if     ge                      ; - then
                nop
                mov     ax,250cH                ; - get hw interrupt vectors
                int     21H                     ; - ...
                mov     cl,al                   ; - restore handler
                mov     edx,iTMoff              ; - ...
                mov     ds,iTMseg               ; - ...
                mov     ax,2504H                ; - ...
                int     21H
                _else                           ; else

                cmp     byte ptr tmarights,0    ; - if we have taken it over
                _if     ne                      ; - - then

                mov     ax,250cH                ; - - get hw interrupt vectors
                int     21H                     ; - - ...
                movzx   ebx,al                  ; - - ...
                shl     ebx,3                   ; - - ...
                mov     ax,50h                  ; - - point to IDT
                mov     es,ax                   ; - - ...

                cli                             ; - ...
                mov     ax,tmhoffs              ; - ...
                mov     es:[ebx].hoffs,ax       ; - ...
                mov     ax,tmloffs              ; - ...
                mov     es:[ebx].loffs,ax       ; - save offset
                mov     ax,tmselect             ; - ...
                mov     es:[ebx].select,ax      ; - save offset
                mov     al,tmarights            ; - ...
                mov     es:[ebx].arights,al     ; - ...
                sti                             ; - ...
                _endif                          ; - endif
                _endif                          ; endif

                pop     es                      ; restore registers
                pop     ds                      ; ...
                pop     ebx                     ; ...
                pop     edx                     ; ...
                pop     ecx                     ; ...
                ret                             ; return to caller
ReleVects_      endp

public          ReadRealClk_
ReadRealClk_    proc    near
                push    ebx                     ; save secp
                push    edx                     ; save minp
                push    eax                     ; save hourp
                mov     ah,2                    ; read real time clock
                int     1aH                     ; ...
                pop     eax                     ; *secp = sec
                movzx   ebx,ch                  ; ...
                mov     dword ptr [eax],ebx     ; ...
                pop     eax                     ; *minp = min
                movzx   ebx,cl                  ; ...
                mov     dword ptr [eax],ebx     ; ...
                pop     eax                     ; *hourp = hour
                movzx   ebx,dh                  ; ...
                mov     dword ptr [eax],ebx     ; ...
                ret
ReadRealClk_    endp

public          SetBiosClk_
SetBiosClk_     proc    near
                push    edx
                mov     dx,ax                   ; dx = low part of count
                shr     eax,10H                 ; cx = high part of count
                mov     cx,ax                   ; ...
                mov     ah,1                    ; set bios count
                int     1aH                     ; ...
                pop     edx
                ret
SetBiosClk_     endp

_text   ends

        end
~
