;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                         Open Watcom Project
;
;   Copyright (c) 2002-2010 Open Watcom Contributors. All Rights Reserved.
;   Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;   Copyright (c) 1987-91, 1992 Rational Systems, Incorporated. All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   tsf32exc.asm -- support for debugger interface library
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        name    tsf32exc

TSF32           struc           ; Transfer stack frame layout
ts32_prev       dw      ?       ; Previous TSF pointer
ts32_ss         dw      ?
ts32_ds         dw      ?
ts32_es         dw      ?
ts32_fs         dw      ?
ts32_gs         dw      ?
ts32_edi        dd      ?
ts32_esi        dd      ?
ts32_ebp        dd      ?
ts32_esp        dd      ?
ts32_ebx        dd      ?
ts32_edx        dd      ?
ts32_ecx        dd      ?
ts32_eax        dd      ?
ts32_id         dw      ?
ts32_code       dd      ?
ts32_eip        dd      ?
ts32_cs         dw      ?
ts32_proc       dw      ?
ts32_eflags     dd      ?
TSF32           ends

        .CODE
        .386p

;
; This code is 16-bit code called from a 16 bit segment.
; Saves the 32 bit context of the processor.
;
; void __cdecl tsf32_exec( TSF32 FarPtr, TSF32 FarPtr )
;
tsf32_exec proc C public from_tsfp:dword, to_tsfp:dword
        ; Establish addressability to save existing registers
        push    es                      ; 2 +
        push    edi                     ; 4 +
        pushfd                          ; 4 +
        les     di, from_tsfp
        push    eax                     ; 4 =
        mov     ax, es
        verw    ax
        jz short tsf32_save
        add     sp, 14                  ; 14
        jmp short tsf32_load

; Can write to from_tsp, save the processor's context
tsf32_save:
        ; Save existing registers
        pop     es:[di].ts32_eax
        pop     es:[di].ts32_eflags
        pop     es:[di].ts32_edi
        pop     es:[di].ts32_es
        mov     es:[di].ts32_ebx, ebx
        mov     es:[di].ts32_ecx, ecx
        mov     es:[di].ts32_edx, edx
        mov     es:[di].ts32_esi, esi
        mov     es:[di].ts32_ebp, ebp
        mov     es:[di].ts32_ds, ds
        mov     es:[di].ts32_fs, fs
        mov     es:[di].ts32_gs, gs
        mov     es:[di].ts32_ss, ss
        ; Create space for return IFRAME32
        sub     esp, 12
        mov     es:[di].ts32_esp, esp
        add     esp, 12
        mov     es:[di].ts32_cs, cs
        mov     dword ptr es:[di].ts32_eip, offset tsf32_exec_continue
tsf32_load:
        les     di, to_tsfp
        mov     ax, es
        verw    ax
        jnz short tsf32_exec_continue 
        ; move the easy ones first
        mov     eax, es:[di].ts32_eax
        mov     ebx, es:[di].ts32_ebx
        mov     ecx, es:[di].ts32_ecx
        mov     edx, es:[di].ts32_edx
        mov     esi, es:[di].ts32_esi
        mov     ebp, es:[di].ts32_ebp
        mov     ds, es:[di].ts32_ds
        push    es:[di].ts32_fs
        pop     fs
        push    es:[di].ts32_gs
        pop     gs
        ; Establish new stack
        mov     ss, es:[di].ts32_ss
        mov     esp, es:[di].ts32_esp
        ; NEEDWORK: For now, IFRAME16 is on the stack
;       add     esp, 6                  ; Drop old Iret frame

        ; Get interrupts right under DPMI
        push    eax
        push    es:[di].ts32_eflags
        pop     eax
        and     eax, 200H
        jnz short setit
        cli
        jmp short popax

setit:  sti
popax:  pop     eax
        ; and push the rest
        push    es:[di].ts32_eflags     ; Establish IRETD frame
        push    0                       ; filler
        push    es:[di].ts32_cs
        push    es:[di].ts32_eip
        push    es:[di].ts32_es
        mov     edi, es:[di].ts32_edi   ; Lose addressability to TSF
        pop     es
        iretdf                          ; ... and restart execution

tsf32_exec_continue:
        ret
tsf32_exec endp

        end
