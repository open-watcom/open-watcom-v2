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
;* Description:  Routines to handle calls to parallel overlays
;*
;*****************************************************************************

        include ovlstd.inc


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

XNAME   public, OVLSETRTN
XNAME   <public "C",>, CHPOVLLDR
XNAME   <public "C",>, CheckRetAddr
XNAME   public, OVLPARINIT
XNAME   <extrn "C",>, OVLLOAD,     :near

        extrn   "C",__BankStack__:word
        extrn   __BankBeg__  :byte
        extrn   __SaveRegs__ :word
        extrn   "C",__OVLCAUSE__ :word
        extrn   "C",__OVLISRET__ :byte

XPROC   CHPOVLLDR, near
        mov     __SaveRegs__+0,AX   ; save registers
        mov     __SaveRegs__+2,BX   ; ...
        pop     BX                  ; remove return address offset
        mov     AX,CS:[BX]          ; get overlay to load
        pushf                       ; save flags

XNAME   call,   OVLSETRTN           ; change the next return address.

        add     BX,2                ; skip the overlay number
        popf                        ; restore flags
        push    BX                  ; restore return offset
        mov     BX,__SaveRegs__+2   ; restore registers
        mov     AX,__SaveRegs__+0   ; ...
        ret                         ; return
XENDP   CHPOVLLDR


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

XPROC   OVLSETRTN, near
        push    BP                          ; save BP
        mov     BP,SP                       ; get pointer to stack
        push    BX                          ; save BX
        push    AX                          ; save AX
        mov     BX,__BankStack__            ; get location of bank stack
        add     __BankStack__,ENTRY_SIZE    ; push new entry
        mov     CS:[BX],AX                  ; save overlay number
        mov     AX,[BP+6]                   ; get return offset
        mov     CS:[BX+2],AX                ; save return offset
        mov     word ptr [BP+6],offset OvlReturn; patch in offset of OvlReturn
ifndef OVL_SMALL
        mov     AX,[BP+8]                   ; get return segment
        mov     CS:[BX+4],AX                ; save return segment
        mov     [BP+8],CS                   ; patch in segment of OvlReturn
endif
        pop     AX                          ; restore AX
        pop     BX                          ; restore BX
        pop     BP                          ; restore BP
        ret
XENDP   OVLSETRTN

; this pops a return address and an overlay number off the bank stack. if the
; overlay corresponding to the overlay number is not loaded, it will be loaded
PopEntry proc near
        cmp     BX,offset __BankBeg__       ; compare with bottom of stack
        je      nope                        ; if not at bottom then
        push    AX                          ; - save AX
        mov     AX,CS:[BX-ENTRY_SIZE]       ; - get PREVIOUS overlay number.

XNAME   call,   OVLLOAD                     ; - load the overlay.

        pop     AX                          ; - restore AX
nope:                                       ; endif
        mov     __BankStack__,BX            ; store bank stack pointer
        ret
PopEntry endp


ifdef OVL_SMALL
OvlReturn proc near
else
OvlReturn proc far
endif
        ;       stash where I'm returning to
        mov     __SaveRegs__+0,BX           ; save BX
        pushf                               ; save flags
        mov     BX,__BankStack__            ; get location of bank stack
        sub     BX,ENTRY_SIZE               ; pop top entry
        push    AX                          ; save AX
ifdef OVL_SMALL
        mov     __OVLCAUSE__+2,CS           ; stash return segment
else
        mov     AX,CS:[BX+4]                ; stash return segment
        mov     __OVLCAUSE__+2,AX           ; ...
endif
        mov     AX,CS:[BX+2]                ; stash return offset
        mov     __OVLCAUSE__,AX             ; ...
        mov     __OVLISRET__,1              ; indicate it's a return
        pop     AX                          ; restore AX
        call    PopEntry                    ; reload overlay if required
        popf                                ; restore flags
ifndef OVL_SMALL
        push    CS:[BX+4]                   ; push return segment
endif
        push    CS:[BX+2]                   ; push return offset
        mov     BX,__SaveRegs__+0           ; restore registers
        ret
OvlReturn endp

;; The following routines are used for C setjmp/longjmp support

RetBankStack proc far
        mov     AX,__BankStack__
        ret
RetBankStack endp


PurgeBankStack proc far
        push    BX
lup:    cmp     AX,__BankStack__
        je      done
        mov     BX,__BankStack__            ; get location of bank stack
        sub     BX,ENTRY_SIZE               ; pop top entry
        call    PopEntry
        jmp     lup
done:   pop     BX
        ret
PurgeBankStack endp

assume  DS:DGROUP

XPROC   OVLPARINIT, near
        push    DS
        mov     AX,seg DGROUP
        mov     DS,AX
        mov     word ptr __get_ovl_stack,offset RetBankStack
        mov     word ptr __restore_ovl_stack,offset PurgeBankStack
        mov     word ptr __get_ovl_stack+2,CS
        mov     word ptr __restore_ovl_stack+2,CS
        mov     AL,1                ; signal that || overlay support is in.
        pop     DS
        ret
XENDP   OVLPARINIT

assume  DS:nothing

;; The following routine is used for debugger support

; a far pointer to an ovl_addr is passed in DX:AX


XPROC   CheckRetAddr, near
        push    ds                  ; save registers
        push    bx
        push    si
        mov     ds,dx               ; get segment of data
        mov     bx,ax               ; get offset of data
        xor     ax,ax               ; assume not return address
        cmp     word ptr [bx], offset OvlReturn ; is it the overlay return
        jne     not_ret             ; ... code offset?
        mov     dx,cs               ; is it the overlay return segment?
        cmp     dx,[bx+2]           ; ...
        jne     not_ret             ; ...
        ; The address given is the overlay manager return code
        ; the section id has the number of levels down the overlay stack
        ; that the real address is to be found
        mov     ax,[bx+4]           ; get levels down
        inc     ax                  ; add one
ifdef OVL_SMALL
        shl     ax,1                ; multiply by four
        shl     ax,1                ; ...
else
        shl     ax,1                ; multiply by six
        mov     dx,ax               ; ...
        shl     ax,1                ; ...
        add     ax,dx               ; ...
endif
        mov     si,__BankStack__    ; get current bank stack
        sub     si,ax               ; adjust to proper level
        xor     ax,ax               ; assume at bottom of bank stack
        cmp     si,offset __BankBeg__ ; are we at the bottom of the stack?
        je      bottom              ; if not then
        mov     ax,cs:[si-ENTRY_SIZE] ; - get true (previous)section number
bottom:                             ; endif
        mov     [bx+4],ax           ; save section number
        mov     ax,cs:[si+2]        ; get true offset
        mov     [bx],ax             ; and save it
ifndef OVL_SMALL
        mov     ax,cs:[si+4]        ; get true segment
        mov     [bx+2],ax           ; and save it
endif
        mov     ax,1                ; return TRUE
not_ret:pop     si                  ; restore registers
        pop     bx
        pop     ds
        ret
XENDP   CheckRetAddr

_TEXT   ends

        end
