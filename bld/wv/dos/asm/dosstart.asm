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



        name    DOSSTART

        DOSSEG

public          _big_code_
_big_code_      equ 0fffH
public          _fltused_
_fltused_       equ 0fffH

                extrn   ProcCmd_                :far
                extrn   MemInit_                :far
                extrn   DebugMain_              :far
                extrn   _ActFontTbls            :byte

DGROUP          group   CONST,_DATA,_BSS,STACK

_TEXT           segment byte public 'CODE'

assume          ds:dgroup

_DATA            segment para public 'DATA'
public          __psp
__psp           dw      ?

public          __osmajor
__osmajor       db      ?

public          __8087
__8087          db      0

public          "C",__FPE_handler
__FPE_handler   dd      0
_DATA            ends

CONST           segment word public 'DATA'
public          __get_ovl_stack
__get_ovl_stack     dd  _TEXT:dummy_ret

public          __restore_ovl_stack
__restore_ovl_stack dd  _TEXT:dummy_ret
CONST           ends


_BSS            segment word public 'BSS'
        extrn   _edata:byte
        extrn   _end:byte
_BSS            ends

STKDEPTH        equ     0800H

STACK           segment word stack 'STACK'
stk_bot db STKDEPTH dup(?)
public          stk_top,stk_bot
stk_top         equ $
STACK           ends


assume  nothing
assume  CS:_TEXT


Int10Vect       dw      ?,?
Int1bVect       dw      ?,?
Int23Vect       dw      ?,?
Int24Vect       dw      ?,?
PendingBreak    db      ?


VideoInt:
                cmp     ax,1103h                ; is it change font request?
                jne     do_int                  ; if not do normal processing
                push    ds                      ; - save DS
                mov     ax,seg DGROUP           ; - get access to data seg
                mov     ds,ax                   ; - . . .
                mov     ds:_ActFontTbls,bl      ; - save active font
                mov     ax,1103h                ; - restore AX
                pop     ds                      ; - restore DS
do_int:                                         ; endif
                jmp     dword ptr cs:Int10Vect  ; do BIOS request

UsrInt:
                mov     cs:PendingBreak,-1
                iret

CritErrDos3X:
                mov     AL,3                    ; fail operation
                iret                            ; return to caller

CritErrDos2X:
                mov     AL,0                    ; ignore operation
                iret                            ; return to caller


SetVect         proc near
                push    ds              ; save registers
                push    cx
                sub     bh,bh           ; turn vector num into offset
                add     bx,bx
                add     bx,bx
                sub     cx,cx           ; get int vector segment
                mov     ds,cx
                cli                     ; interrupts off
                xchg    ax,0[bx]        ; exchange offsets
                xchg    dx,2[bx]        ; exchange segments
                sti                     ; interrupts on
                pop     cx              ; restore registers
                pop     ds
                ret                     ; return to caller
SetVect         endp



public          GrabHandlers_
GrabHandlers_   proc    far
                push    bx              ; save registers
                push    dx
                mov     bl,10H          ; set video interrupt vector
                mov     dx,cs
                mov     ax,offset VideoInt
                call    SetVect
                mov     CS:Int10Vect+0,ax
                mov     CS:Int10Vect+2,dx
                mov     bl,1bh          ; set ctrl/break vector
                mov     dx,cs
                mov     ax,offset UsrInt
                call    SetVect
                mov     CS:Int1bVect+0,ax
                mov     CS:Int1bVect+2,dx
                mov     bl,23h          ; set ctrl/C vector
                mov     dx,cs
                mov     ax,offset UsrInt
                call    SetVect
                mov     CS:Int23Vect+0,ax
                mov     CS:Int23Vect+2,dx
                cmp     ds:__osmajor,2  ; set critical error vector
                jae     dos_3
                  mov     ax,offset CritErrDos2X
                jmp     short set_int   ; else
dos_3:            mov     ax,offset CritErrDos3X
set_int:
                mov     bl,24h
                mov     dx,cs
                call    SetVect
                mov     CS:Int24Vect+0,ax
                mov     CS:Int24Vect+2,dx

                pop     dx
                pop     bx
dummy_ret:      ret
GrabHandlers_   endp

public          RestoreHandlers_
RestoreHandlers_ proc   far
                push    bx              ; save registers
                push    dx
                ; check the keyboard status to clear any pending
                ; Ctrl/C, Ctrl/Break requests before handing back
                ; the vector
                mov     ah,0bh          ; check keyboard status
                int     21H
                mov     bl,10h
                mov     ax,cs:Int10Vect+0
                mov     dx,cs:Int10Vect+2
                call    SetVect
                mov     bl,1bh          ; set ctrl/break vector
                mov     ax,cs:Int1bVect+0
                mov     dx,cs:Int1bVect+2
                call    SetVect
                mov     bl,23h          ; set ctrl/C vector
                mov     ax,cs:Int23Vect+0
                mov     dx,cs:Int23Vect+2
                call    SetVect
                mov     bl,24h          ; set critical error vector
                mov     ax,cs:Int24Vect+0
                mov     dx,cs:Int24Vect+2
                call    SetVect
                pop     dx
                pop     bx
                ret
RestoreHandlers_ endp


public  TBreak_
TBreak_         proc    far             ; return terminal break status
                xor     AL,AL
                xchg    AL,CS:PendingBreak
                cbw
                ret
TBreak_         endp


public          Startup
Startup         equ     $
                sti
                cld

                mov     DX,DS:2H        ; get maximum available memory
                ; set up data segment
                mov     BX,DGROUP
                mov     DS,BX
                mov     DS:__psp,ES     ; save PSP address
                mov     ES,BX
                cli
                mov     SS,BX
                mov     SP,offset DGROUP:stk_top
                sti

                mov     AH,30H          ; get version number
                int     21H

                mov     DS:__osmajor,AL

assume  CS:_TEXT, DS:DGROUP, DS:DGROUP, SS:DGROUP

                mov     di,offset dgroup:_edata
                mov     al,0
                mov     cx, offset dgroup:_end
                sub     cx,di
                rep     stosb           ; zero _BSS segment variables

                call    GrabHandlers_

                call    ProcCmd_        ; process command line

                mov     AX,SP           ; AX <== first available offset
                call    MemInit_

                jmp     DebugMain_      ; never returns


public          KillDebugger_
KillDebugger_   proc    far
                ; terminate debugger
                push    AX                  ; save return code
                call    RestoreHandlers_
                pop     AX                  ; restore return code
                mov     AH,4CH
                int     21H
KillDebugger_   endp


_TEXT   ends

        end     Startup
