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


extrn           BackFromProtMode_       : near
extrn           _BackFromFork           : byte

                name            fork

DGROUP GROUP _text

assume cs:DGROUP

_text segment byte public 'code'

public          GetPSP_
GetPSP_         proc    near
                push    bx                      ; save regs
                pushf                           ; save flags
                mov     ah,51H                  ; get PSP request
                int     21H                     ; do the call
                mov     ax,bx                   ; put in AX
                popf                            ; restore flags
                pop     bx                      ; restore regs
                ret                             ; return
GetPSP_         endp

public          SetPSP_
SetPSP_         proc    near
                push    BX              ; save BX
                pushf                   ; save flags
                mov     BX,AX           ; set PSP to set to
                mov     AH,50H          ; set PSP request (internal DOS call)
                int     21H             ; do the call
                popf                    ; restore flags
                pop     BX              ; restore BX
                ret                     ; return
SetPSP_         endp

public          SaveVectors_
SaveVectors_    proc    near
                push    ds              ; save registers
                push    es              ; ...
                push    si              ; ...
                push    di              ; ...
                push    cx              ; ...
                xor     si,si           ; src is 0:0
                mov     ds,si           ; ...
                mov     es,dx           ; set up dest
                mov     di,ax           ; ...
                mov     cx,512          ; size is 1K
                cli                     ; interrupts off
                rep     movsw           ; save the interrupt vector table
                sti                     ; interrupts back on
                pop     cx              ; ...
                pop     di              ; ...
                pop     si              ; ...
                pop     es              ; ...
                pop     ds              ; ...
                ret                     ; return to caller
SaveVectors_    endp

public          RestoreVectors_
RestoreVectors_ proc    near
                push    ds              ; save registses
                push    es              ; ...
                push    si              ; ...
                push    di              ; ...
                push    cx              ; ...
                xor     di,di           ; dest is 0:0
                mov     es,di           ; ...
                mov     ds,dx           ; set up source
                mov     si,ax           ; ...
                mov     cx,512          ; size is 1K
                cli                     ; interrupts off
                rep     movsw           ; save the interrupt vector table
                sti                     ; interrupts back on
                pop     cx              ; restore registers
                pop     di              ; ...
                pop     si              ; ...
                pop     es              ; ...
                pop     ds              ; ...
                ret                     ; return to caller
RestoreVectors_ endp

DebugPSP        dw  ?

public          DbgPSP_
DbgPSP_         proc    near
                mov     ax,cs:DebugPSP
                ret
DbgPSP_         endp

public          InitPSP_
InitPSP_        proc    near
                call    GetPSP_
                mov     cs:DebugPSP,ax
                ret
InitPSP_        endp



                public  _fork_                  ; spawn off a subprocess
_fork_          proc    near                    ; ...
                push    bp                      ; save all registers
                push    ds                      ; ...
                push    es                      ; ...
                push    si                      ; ...
                push    di                      ; ...
                push    dx                      ; ...
                push    cx                      ; ...
                push    bx                      ; ...
                push    ax
                mov     bp,sp                   ; ...
                mov     cs:saveds,ds            ; save ss and bp since exec
                mov     cs:savess,ss            ; save ss and bp since exec
                mov     cs:savebp,bp            ; . kills all regs except cs:ip
                mov     si,cs:DebugPSP          ; get debugger psp
                push    si                      ; ...
                mov     ax,006cH                ; ... for exec dos call
                push    ax                      ; ...
                push    si                      ; ...
                mov     ax,005cH                ; ...
                push    ax                      ; ...
                push    4[bp]                   ; ... points to parm
                push    2[bp]                   ; ... ...
                sub     ax,ax                   ; ...
                push    ax                      ; ...
                mov     ax,ss                   ; point es:bx at control block
                mov     es,ax                   ; ...
                mov     bx,sp                   ; ...
                mov     dx,0[bp]                ; ds:dx points to task
                mov     ds,6[bp]                ; ...
                mov     ss,si                   ; point stack at PSP:100 for int
                mov     sp,0100H                ; ...
                mov     ax,4b00H                ; DOS exec function
                int     21H                     ; ...
                mov     ds,cs:saveds            ; restore ds
                mov     byte ptr ds:_BackFromFork,1 ; we're back
                jc      normalret               ;
                jmp     BackFromProtMode_       ; unwind the stack (eeek!)
normalret:      mov     bp,cs:savebp            ; restore bp
                mov     ss,cs:savess            ; and ss
                mov     sp,bp                   ; restore work regs
                pop     bx                      ; discard ax image
                pop     bx                      ; restore registers
                pop     cx                      ; ...
                pop     dx                      ; ...
                pop     di                      ; ...
                pop     si                      ; ...
                pop     es                      ; ...
                pop     ds                      ; ...
                pop     bp                      ; ...
doreturn:       ret                             ; return to caller
_fork_          endp

saveds          dw      0
savess          dw      0
savebp          dw      0

_text           ends

                end
