;   cpuModel.asm
;
;   This file contains all assembly code for the Intel Cpu identification.
;   It is based on linux cpu detection code.
;
;   Intel also provides public similar code in the book
;   called :
;
;   Pentium Processor Family
;       Developer Family
;   Volume  3 : Architecture and Programming Manual
;
;  At the following place :
;
;   Chapter 5 : Feature determination
;   Chapter 25: CPUID instruction
;
;   COPYRIGHT (c) 1998 valette@crf.canon.fr
;
;   The license and distribution terms for this file may be
;   found in the file LICENSE in this distribution or at
;   http://www.OARcorp.com/rtems/license.html.
;
;   $Id: cpuModel.S,v 1.1 1998/08/05 15:15:46 joel Exp $
;
; Rewritten for tasm/wasm by G. Vanem 2000 for
; the Watt-32 tcp/ip stack.
;

ifdef DOSX  ; only for DOSX targets

PAGE 2000, 132


PUBLIC have_cpuid, x86_type, x86_model, x86_mask, x86_capability
PUBLIC hard_math,  x86_vendor_id, CheckCpuType

ifdef ??version   ; Turbo Assembler
  .486
  .487
  CPUID equ <db 0fh, 0A2h>   ; v3.2 at least lacks CPUID
else
  .586  ; WASM thinks CPUID requires a Pentium !!
  .387
endif

EFLAGS_ALIGN_CHECK = 040000h
EFLAGS_ID          = 200000h

.MODEL FLAT,C

.DATA
align 4

have_cpuid      dd 0
x86_type        db 0
x86_model       db 0
x86_mask        db 0
                db 0
x86_capability  dd 0
x86_vendor_id   db 13 dup (0)
hard_math       db 0


.CODE

;
; check Processor type: 386, 486, 6x86(L) or CPUID capable processor
;
CheckCpuType:

    mov x86_type, 3                 ;  Assume 386 for now

    ; Start using the EFAGS AC bit determination method described in
    ; the book mentionned above page 5.1. If this bit can be set we
    ; have a 486 or above.
    ;
    pushfd                          ; save EFLAGS
    pushfd                          ; Get EFLAGS in EAX
    pop  eax
    
    mov  ecx, eax                   ; save original EFLAGS in ECX
    xor  eax, EFLAGS_ALIGN_CHECK    ; flip AC bit in EAX
    push eax                        ; set EAX as EFLAGS
    popfd
    pushfd                          ; Get new EFLAGS in EAX
    pop  eax
    
    xor  eax, ecx                   ; check if AC bit changed
    and  eax, EFLAGS_ALIGN_CHECK
    je   is386                      ; If not : we have a 386

    mov  x86_type, 4                ; Assume 486 for now
    mov  eax, ecx                   ; Restore orig EFLAGS in EAX
    xor  eax, EFLAGS_ID             ; flip  ID flag
    push eax                        ; set EAX as EFLAGS
    popfd
    pushfd                          ; Get new EFLAGS in EAX
    pop  eax
            
    xor  eax, ecx                   ; check if ID bit changed
    and  eax, EFLAGS_ID

    ;
    ; if we are on a straight 486DX, SX, or 487SX we can't
    ; change it. OTOH 6x86MXs and MIIs check OK.
    ; Also if we are on a Cyrix 6x86(L)
    ;
    je is486x

isnew:  
    popfd                           ; restore original EFLAGS
    inc have_cpuid                  ; we have CPUID instruction

    ; use it to get :
    ;  processor type,
    ;  processor model,
    ;  processor mask,
    ; by using it with EAX = 1

    mov  eax, 1
    CPUID

    mov  cl, al                     ; save reg for future use
    
    and  ah, 0Fh                    ; mask processor family
    mov  x86_type, ah               ; put result in x86_type
    
    and  al, 0F0h                   ; get model
    shr  al, 4
    mov  x86_model, al              ; store it in x86_model

    and  cl, 0Fh                    ; get mask revision
    mov  x86_mask, cl               ; store it in x86_mask
    
    mov  x86_capability, edx        ; store feature flags

    ;
    ; get vendor info by using CPUID with EAX = 0
    ;
    xor eax, eax
    CPUID

    ;
    ; store results contained in ebx, edx, ecx in x86_vendor_id string.
    ;
    mov  dword ptr x86_vendor_id, ebx
    mov  dword ptr x86_vendor_id+4, edx
    mov  dword ptr x86_vendor_id+8, ecx
    call check_x87
    ret

;
; Now we test if we have a Cyrix 6x86(L). We didn't test before to avoid
; clobbering the new BX chipset used with the Pentium II, which has a
; register at the same addresses as those used to access the Cyrix special
; configuration registers (CCRs).

    ;
    ; A Cyrix/IBM 6x86(L) preserves flags after dividing 5 by 2
    ; (and it _must_ be 5 divided by 2) while other CPUs change
    ; them in undefined ways. We need to know this since we may
    ; need to enable the CPUID instruction at least.
    ; We couldn't use this test before since the PPro and PII behave
    ; like Cyrix chips in this respect.
    ;
is486x:
    xor ax, ax
    sahf
    mov ax, 5
    mov bx, 2
    div bl
    lahf
    cmp ah, 2
    jne ncyrix

    ;
    ; N.B. The pattern of accesses to 0x22 and 0x23 is *essential*
    ;      so do not try to "optimize" it! For the same reason we
    ;      do all this with interrupts off.

setCx86 MACRO reg, val
        mov ax, reg
        out 22h, ax
        mov ax, val
        out 23h, ax
        ENDM

getCx86 MACRO reg
        mov ax, reg
        out 22h, ax
        in  ax, 23h
        ENDM

    cli
    getCx86 0C3h             ;  get CCR3
    mov cx, ax               ; Save old value
    mov bx, ax
    and bx, 0Fh              ; Enable access to all config registers
    or  bx, 10h              ; by setting bit 4
    setCx86 0C3h, bx

    getCx86 0E8h             ; now we can get CCR4
    or ax, 80h               ; and set bit 7 (CPUIDEN)
    mov bx, ax               ; to enable CPUID execution
    setCx86 0E8h, bx

    getCx86 0FEh             ; DIR0 : let's check this is a 6x86(L)
    and ax, 0F0h             ; should be 3xh
    cmp ax, 30h
    jne n6x86
    getCx86 0E9h             ; CCR5 : we reset the SLOP bit
    and ax, 0FDh             ; so that udelay calculation
    mov bx, ax               ; is correct on 6x86(L) CPUs
    setCx86 0E9h, bx

    setCx86 0C3h, cx         ; Restore old CCR3
    sti
    jmp isnew                ; We enabled CPUID now

n6x86:
    setCx86 0C3h, cx         ; Restore old CCR3
    sti

ncyrix:
    popfd                    ; restore original EFLAGS
    call check_x87
    ret

is386:
    popfd                    ; restore original EFLAGS
    call check_x87
    ret
    

;
; This checks for 287/387.
;
check_x87:
    fninit
    fstsw ax
    cmp al, 0
    je  is_x87
    mov hard_math, 0
    ret

is_x87:
    mov hard_math, 1
    ret

endif ; DOSX

end

