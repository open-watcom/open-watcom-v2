;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                         Open Watcom Project
;
; Copyright (c) 2002-2010 The Open Watcom Contributors. All Rights Reserved.
; Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   rmhdr.asm   - Real mode handlers for RSI debugger interface
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        name    rmhdlr

LOWMEM  segment para public 'data' 

        ASSUME cs:LOWMEM,ds:NOTHING,ss:NOTHING

        PUBLIC  "C", rm15_handler
        PUBLIC  "C", rm1B_handler
        PUBLIC  "C", passup_hotkey

        PUBLIC  "C", hotkey_passup
        PUBLIC  "C", org15_handler

;
; void  rm15_handler( void )
;
rm15_handler:
        pushf
        cmp     ax,8500H
        je short sysreq
        push    cs
        call    near ptr rm_iret
        jmp     dword ptr cs:[org15_handler]

sysreq:
        push    cs
        call    near ptr rm_iret
;
; void  passup_hotkey( void )
;
passup_hotkey:
        jmp     dword ptr cs:[hotkey_passup]

;
; void  rm1B_handler( void )
;
rm1B_handler:
        push    ax
        mov     ax, 20h
        out     20h, al
        pop     ax
        jmp short passup_hotkey

rm_iret:
        iret

hotkey_passup  DD      rm_iret
org15_handler  DD      rm_iret

LOWMEM  ends

        end
