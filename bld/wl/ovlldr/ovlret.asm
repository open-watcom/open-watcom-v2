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


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> OVLRET:    Routines to handle calls to parallel overlays
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

ifdef OVL_SMALL
        name    sovlret
ENTRY_SIZE  equ 4

else
        name    lovlret
ENTRY_SIZE  equ 6
endif

        comm        __get_ovl_stack:dword
        comm        __restore_ovl_stack:dword

DGROUP          group   _DATA

_DATA   segment byte 'DATA' PUBLIC
_DATA   ends

_TEXT   segment byte '_OVLCODE' PUBLIC
        assume  CS:_TEXT

ifdef OVL_SMALL
        public  __SOVLSETRTN__
        public  __SCHPOVLLDR__
        public  __SCheckRetAddr__
        public  __SOVLPARINIT__
        extrn   __SOVLLOAD__:near
else
        public  __LOVLSETRTN__
        public  __LCHPOVLLDR__
        public  __LCheckRetAddr__
        public  __LOVLPARINIT__
        extrn   __LOVLLOAD__:near
endif
        extrn   __BankStack__:word
        extrn   __BankBeg__:byte
        extrn   __SaveRegs__:word
        extrn   __OVLCAUSE__:word
        extrn   __OVLISRET__:byte

ifdef OVL_SMALL
__SCHPOVLLDR__ proc near
else
__LCHPOVLLDR__ proc near
endif
        mov     CS:__SaveRegs__+0,AX; save registers
        mov     CS:__SaveRegs__+2,BX; ...
        pop     BX                  ; remove return address offset
        mov     AX,CS:[BX]          ; get overlay to load
        pushf                       ; save flags
ifdef OVL_SMALL
        call    __SOVLSETRTN__      ; change the next return address.
else
        call    __LOVLSETRTN__      ; change the next return address.
endif
        popf                        ; restore flags
        add     BX,2                ; skip the overlay number
        push    BX                  ; restore return offset
        mov     BX,CS:__SaveRegs__+2; restore registers
        mov     AX,CS:__SaveRegs__+0; ...
        ret                         ; return
ifdef OVL_SMALL
__SCHPOVLLDR__ endp
else
__LCHPOVLLDR__ endp
endif


; the stack looks like this:
;
;       high mem:
;           +6  original return (near/far)
;           +4  saved flags
;           +2  __OVLSETRTN__ return address (near)
;       BP ==>  old BP
;       low mem:

; AX contains the overlay number being loaded

; ALL registers MUST be preserved (nuking flags is OK)

ifdef OVL_SMALL
__SOVLSETRTN__ proc near
else
__LOVLSETRTN__ proc near
endif
        push    BP                          ; save BP
        mov     BP,SP                       ; get pointer to stack
        push    BX                          ; save BX
        push    AX                          ; save AX
        mov     BX,CS:__BankStack__         ; get location of bank stack
        add     CS:__BankStack__,ENTRY_SIZE ; push new entry
        mov     CS:0[BX],AX                 ; save overlay number
        mov     AX,6[BP]                    ; get return offset
        mov     CS:2[BX],AX                 ; save return offset
        mov     word ptr 6[BP],offset OvlReturn; patch in offset of OvlReturn
ifndef OVL_SMALL
        mov     AX,8[BP]                    ; get return segment
        mov     CS:4[BX],AX                 ; save return segment
        mov     8[BP],CS                    ; patch in segment of OvlReturn
endif
        pop     AX                          ; restore AX
        pop     BX                          ; restore BX
        pop     BP                          ; restore BP
        ret
ifdef OVL_SMALL
__SOVLSETRTN__ endp
else
__LOVLSETRTN__ endp
endif

; this pops a return address and an overlay number off the bank stack. if the
; overlay corresponding to the overlay number is not loaded, it will be loaded
PopEntry proc near
        cmp     BX,offset __BankBeg__       ; compare with bottom of stack
        je      nope                        ; if not at bottom then
        push    AX                          ; - save AX
        mov     AX,CS:[BX-ENTRY_SIZE]       ; - get PREVIOUS overlay number.
ifdef OVL_SMALL
        call    __SOVLLOAD__                ; - load the overlay.
else
        call    __LOVLLOAD__                ; - load the overlay.
endif
        pop     AX                          ; - restore AX
nope:                                       ; endif
        mov     CS:__BankStack__,BX         ; store bank stack pointer
        ret
PopEntry endp

ifdef OVL_SMALL
OvlReturn proc near
else
OvlReturn proc far
endif
        ;       stash where I'm returning to
        mov     CS:__SaveRegs__+0,BX        ; save BX
        pushf                               ; save flags
        mov     BX,CS:__BankStack__         ; get location of bank stack
        sub     BX,ENTRY_SIZE               ; pop top entry
        push    AX                          ; save AX
ifdef OVL_SMALL
        mov     CS:__OVLCAUSE__+2,CS        ; stash return segment
else
        mov     AX,CS:4[BX]                 ; stash return segment
        mov     CS:__OVLCAUSE__+2,AX        ; ...
endif
        mov     AX,CS:2[BX]                 ; stash return offset
        mov     CS:__OVLCAUSE__,AX          ; ...
        mov     byte ptr CS:__OVLISRET__,1  ; indicate it's a return
        pop     AX                          ; restore AX
        call    PopEntry                    ; reload overlay if required
        popf                                ; restore flags
ifndef OVL_SMALL
        push    CS:4[BX]                    ; push return segment
endif
        push    CS:2[BX]                    ; push return offset
        mov     BX,CS:__SaveRegs__+0        ; restore registers
        ret
OvlReturn endp

;; The following routines are used for C setjmp/longjmp support

RetBankStack proc far
        mov     AX,CS:__BankStack__
        ret
RetBankStack endp


PurgeBankStack proc far
        push    BX
lup:    cmp     AX,CS:__BankStack__
        je      done
        mov     BX,CS:__BankStack__         ; get location of bank stack
        sub     BX,ENTRY_SIZE               ; pop top entry
        call    PopEntry
        jmp     lup
done:   pop     BX
        ret
PurgeBankStack endp

ifdef OVL_SMALL
__SOVLPARINIT__ proc near
else
__LOVLPARINIT__ proc near
endif
            push    DS
            mov     AX,seg DGROUP
            mov     DS,AX
            mov     word ptr DS:__get_ovl_stack,offset RetBankStack
            mov     word ptr DS:__restore_ovl_stack,offset PurgeBankStack
            mov     word ptr DS:__get_ovl_stack+2,CS
            mov     word ptr DS:__restore_ovl_stack+2,CS
            mov     AL,1            ; signal that || overlay support is in.
            pop     DS
            ret
ifdef OVL_SMALL
__SOVLPARINIT__ endp
else
__LOVLPARINIT__ endp
endif

;; The following routine is used for debugger support

; a far pointer to an ovl_addr is passed in DX:AX

ifdef OVL_SMALL
__SCheckRetAddr__ proc near
else
__LCheckRetAddr__ proc near
endif
            push    ds          ; save registers
            push    bx
            push    si
            mov     ds,dx       ; get segment of data
            mov     bx,ax       ; get offset of data
            xor     ax,ax       ; assume not return address
            cmp     word ptr [bx], offset OvlReturn ; is it the overlay return
            jne     not_ret     ; ... code offset?
            mov     dx,cs       ; is it the overlay return segment?
            cmp     dx,2[bx]    ; ...
            jne     not_ret     ; ...
            ; The address given is the overlay manager return code
            ; the section id has the number of levels down the overlay stack
            ; that the real address is to be found
            mov     ax,4[bx]    ; get levels down
            inc     ax          ; add one
ifdef OVL_SMALL
            shl     ax,1        ; multiply by four
            shl     ax,1        ; ...
else
            shl     ax,1        ; multiply by six
            mov     dx,ax       ; ...
            shl     ax,1        ; ...
            add     ax,dx       ; ...
endif
            mov     si,CS:__BankStack__; get current bank stack
            sub     si,ax       ; adjust to proper level
            xor     ax,ax       ; assume at bottom of bank stack
            cmp     si,offset __BankBeg__; are we at the bottom of the stack?
            je      bottom      ; if not then
            mov     ax,cs:[si-ENTRY_SIZE] ; - get true (previous)section number
bottom:                         ; endif
            mov     4[bx],ax    ; save section number
            mov     ax,cs:2[si] ; get true offset
            mov     0[bx],ax    ; and save it
ifndef OVL_SMALL
            mov     ax,cs:4[si] ; get true segment
            mov     2[bx],ax    ; and save it
endif
            mov     ax,1        ; return TRUE
not_ret:    pop     si          ; restore registers
            pop     bx
            pop     ds
            ret
ifdef OVL_SMALL
__SCheckRetAddr__ endp
else
__LCheckRetAddr__ endp
endif

_TEXT   ends

        end
