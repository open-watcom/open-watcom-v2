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
;* Description:  Standard Overlay Manager
;*
;*****************************************************************************

        include ovlstd.inc


ifdef OVL_SMALL
        name    sovlmain
else
        name    lovlmain
endif

        comm    __close_ovl_file:dword

DGROUP  group   _DATA

_DATA   segment byte 'DATA' PUBLIC
_DATA   ends

_TEXT   segment dword '_OVLCODE' PUBLIC

XNAME   public, OVLLDR
XNAME   public, OVLINIT
XNAME   <public "C",>, DBG_HOOK
XNAME   <extrn "C",>, OVLTINIT,   :near
XNAME   <extrn "C",>, OVLLOAD,    :near
XNAME   extrn,  OVLSETRTN,  :near
XNAME   <extrn "C",>, DBG_HANDLER,:far
XNAME   extrn,  OVLPARINIT, :near

        extrn   "C",__OVLPSP__  :word
        extrn   "C",__OVLCAUSE__:word
        extrn   "C",__OVLDOPAR__:byte
        extrn   "C",__OVLISRET__:byte
        extrn   __SaveRegs__:word
        extrn   __TopStack__:byte
        extrn   "C",__OVLFLAGS__:word
        extrn   "C",__CloseOvl__:far

SaveSS          dw      ?
SaveSP          dw      ?

NullHook proc   far
        ret
NullHook endp

XPROC   OVLINIT, far
        jmp     short around

        dw OVL_SIGNATURE

XNAME , DBG_HOOK, <dd NullHook>

XNAME   dw, DBG_HANDLER

around:
        mov     __OVLPSP__,ES               ; save segment address of PSP
        mov     SaveSS,SS                   ; save actual SS:SP
        mov     SaveSP,SP                   ; ...
        cli                                 ; set SS:SP to temporary stack
        mov     AX,CS                       ; ...
        mov     SS,AX                       ; ...
        mov     SP,offset __TopStack__      ; ...
        sti                                 ; ...
        mov     __OVLFLAGS__,0              ; initialize __OVLFLAGS__
        mov     ax,3000h                    ; get dos version number
        int     21h                         ; ...
        cmp     al,3                        ; check if version 3 or greater
        jb      not_dos3                    ; ...
        or      __OVLFLAGS__,2              ; set OVL_DOS3 flag
not_dos3:

XNAME   call,   OVLTINIT                    ; initialize overlays
        mov     BX,AX                       ; save AX register
XNAME   call,   OVLPARINIT                  ; initialize bank stack

        mov     __OVLDOPAR__,AL             ; save status of || overlay support
        cli                                 ; set SS:SP to actual stack
        mov     SS,SaveSS                   ; ...
        mov     SP,SaveSP                   ; ...
        sti                                 ; ...

assume DS:DGROUP

        push    DS                          ; save DS
        mov     ax,seg DGROUP               ; get seg of DGROUP
        mov     ds,ax                       ; ...
        mov     word ptr __close_ovl_file,offset __CloseOvl__
        mov     word ptr __close_ovl_file+2,CS
        pop     DS                          ; restore DS

assume DS:nothing

        push    DX                          ; push actual start segment
        push    BX                          ; push actual start offset

XNAME   <jmp>, DBG_HOOK                     ; hook into debugger if it's there

XENDP   OVLINIT


align 4

XPROC   OVLLDR, near
        mov     __SaveRegs__+0,AX           ; save registers
        mov     __SaveRegs__+2,BP           ; ...
        mov     BP,SP                       ; peek at the stack
        mov     AX,[BP+2]                   ; get cause of overlay load
        mov     __OVLCAUSE__,AX             ; stash it
ifdef OVL_SMALL                             ; ...
        mov     __OVLCAUSE__+2,CS           ; ...
else
        mov     AX,[BP+4]                   ; ...
        mov     __OVLCAUSE__+2,AX           ; stash it
endif
        mov     __OVLISRET__,0              ; indicate not a return
        pop     BP                          ; remove return address offset
        mov     AX,CS:[BP]                  ; get overlay to load
        pushf                               ; save flags

XNAME   call,   OVLSETRTN                   ; change the next ret address.
XNAME   call,   OVLLOAD                     ; load overlay

        add     BP,2                        ; skip overlay # when returning.
        popf                                ; restore flags
        push    BP                          ; restore return offset
        mov     BP,__SaveRegs__+2           ; restore registers
        mov     AX,__SaveRegs__+0           ; ...
        ret                                     ; return
XENDP   OVLLDR

_TEXT   ends

        end
