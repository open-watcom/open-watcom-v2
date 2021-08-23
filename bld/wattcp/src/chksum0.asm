;
; Copyright (c) 1985-1998 Microsoft Corporation
; This file is part of the Microsoft Research IPv6 Network Protocol Stack.
; You should have received a copy of the Microsoft End-User License Agreement
; for this software along with this release; see the file "license.txt".
; If not, please see http://www.research.microsoft.com/msripv6/license.htm,
; or write to Microsoft Research, One Microsoft Way, Redmond, WA 98052-6399.
;
; Abstract:
;
; This module implements a function to compute the internet checksum
; of a buffer.
;

ifdef DOSX    ; only for DOSX-targets

PAGE 66, 132

PUBLIC __w32_inchksum_fast, _w32_inchksum_fast

LOOP_UNROLLING_BITS  equ  5
LOOP_UNROLLING       equ  (1 SHL LOOP_UNROLLING_BITS)

.386
.MODEL FLAT,C

.CODE


buf  equ 12                        ; stack offset to source address
len  equ 16                        ; stack offset to length in words

to_checksum_last_word:
       jmp  checksum_last_word

to_checksum_done:
       jmp  checksum_done

to_checksum_dword_loop_done:
       jmp  checksum_dword_loop_done

;
; WORD cdecl inchksum_fast (const void *buf, int len);
;

 _w32_inchksum_fast:
__w32_inchksum_fast:
       push ebx                     ; save nonvolatile register
       push esi                     ; save nonvolatile register

       mov  ecx, [esp+len]          ; get length in bytes
       sub  eax, eax                ; clear computed checksum
       test ecx, ecx                ; any bytes to checksum at all?
       jz   short to_checksum_done  ; no bytes to checksum
;
; if the checksum buffer is not word aligned, then add the first byte of
; the buffer to the input checksum.
;
       mov  esi, [esp+buf]          ; get source address
       sub  edx, edx                ; set up to load word into EDX below
       test esi, 1                  ; check if buffer word aligned
       jz   short checksum_word_aligned ; if zf, buffer word aligned
       mov  ah, [esi]               ; get first byte (we know we'll have
                                    ;  to swap at the end)
       inc  esi                     ; increment buffer address
       dec  ecx                     ; decrement number of bytes
       jz   short to_checksum_done  ; if zf set, no more bytes

;
; If the buffer is not an even number of of bytes, then initialize
; the computed checksum with the last byte of the buffer.
;

checksum_word_aligned:
       shr  ecx,1                   ; convert to word count
       jnc  short checksum_start    ; if nc, even number of bytes
       mov  al, [esi+ecx*2]         ; initialize the computed checksum
       jz   short to_checksum_done  ; if zf set, no more bytes

;
; Compute checksum in large blocks of dwords, with one partial word up front if
; necessary to get dword alignment, and another partial word at the end if
; needed.
;

;
; Compute checksum on the leading word, if that's necessary to get dword
; alignment.
;

checksum_start:
       test esi, 2                  ; check if source dword aligned
       jz   short checksum_dword_aligned ; source is already dword aligned
       mov  dx, [esi]               ; get first word to checksum
       add  esi, 2                  ; update source address
       add  eax, edx                ; update partial checksum
                                    ;  (no carry is possible, because EAX
                                    ;  and EDX are both 16-bit values)
       dec  ecx                     ; count off this word (zero case gets
                                    ;  picked up below)

;
; Checksum as many words as possible by processing a dword at a time.
;

checksum_dword_aligned:
       push ecx                     ; so we can tell if there's a trailing
                                    ;  word later
       shr  ecx, 1                  ; # of dwords to checksum
       jz   short to_checksum_last_word ; no dwords to checksum

       mov  edx, [esi]              ; preload the first dword
       add  esi, 4                  ; point to the next dword
       dec  ecx                     ; count off the dword we just loaded
       jz   short to_checksum_dword_loop_done
                                    ; skip the loop if that was the only
                                    ;  dword
       mov  ebx, ecx                ; EBX = # of dwords left to checksum
       add  ecx, LOOP_UNROLLING-1   ; round up loop count
       shr  ecx, LOOP_UNROLLING_BITS; convert from word count to unrolled
                                    ;  loop count
       and  ebx, LOOP_UNROLLING-1   ; # of partial dwords to do in first
                                    ;  loop
       jz   short checksum_dword_loop ; special-case when no partial loop,
                                    ;  because fixup below doesn't work
                                    ;  in that case (carry flag is
                                    ;  cleared at this point, as required
                                    ;  at loop entry)
       lea  esi, [esi+ebx*4-(LOOP_UNROLLING*4)]
                                    ; adjust buffer pointer back to
                                    ;  compensate for hardwired displacement
                                    ;  at loop entry point
                                    ; ***doesn't change carry flag***
       jmp  loop_entries[ebx*4]     ; enter the loop to do the first,
                                    ; partial iteration, after which we can
                                    ; just do 64-word blocks
                                    ; ***doesn't change carry flag***

checksum_dword_loop:

LOOP_ENTRY MACRO X
      loop_entry_&X:
      adc  eax, edx
      mov  edx, [esi+X]
      endm

LOOP_ENTRY 0
LOOP_ENTRY 4
LOOP_ENTRY 8
LOOP_ENTRY 12
LOOP_ENTRY 16
LOOP_ENTRY 20
LOOP_ENTRY 24
LOOP_ENTRY 28
LOOP_ENTRY 32
LOOP_ENTRY 36
LOOP_ENTRY 40
LOOP_ENTRY 44
LOOP_ENTRY 48
LOOP_ENTRY 52
LOOP_ENTRY 56
LOOP_ENTRY 60
LOOP_ENTRY 64
LOOP_ENTRY 68
LOOP_ENTRY 72
LOOP_ENTRY 76
LOOP_ENTRY 80
LOOP_ENTRY 84
LOOP_ENTRY 88
LOOP_ENTRY 92
LOOP_ENTRY 96
LOOP_ENTRY 100
LOOP_ENTRY 104
LOOP_ENTRY 108
LOOP_ENTRY 112
LOOP_ENTRY 116
LOOP_ENTRY 120
LOOP_ENTRY 124   ; 4*(LOOP_UNROLLING-1)


checksum_dword_loop_end:
       lea  esi, [esi+LOOP_UNROLLING*4]  ; update source address
       dec  ecx                     ; count off unrolled loop iteration
       jnz  checksum_dword_loop     ; do more blocks

checksum_dword_loop_done:
       adc  eax, edx                ; finish dword checksum
       mov  edx, 0                  ; prepare to load trailing word
       adc  eax, edx

;
; Compute checksum on the trailing word, if there is one.
; High word of EDX = 0 at this point
; Carry flag set iff there's a trailing word to do at this point
;

checksum_last_word:
       pop  ecx                     ; get back word count
       test ecx, 1                  ; is there a trailing word?
       jz   short checksum_done     ; no trailing word
       add  ax, [esi]               ; add in the trailing word
       adc  eax, 0

checksum_done:
       mov  ecx, eax                ; fold the checksum to 16 bits
       ror  ecx, 16
       add  eax, ecx
       mov  ebx, [esp+buf]
       shr  eax, 16
       test ebx, 1                  ; check if buffer word aligned
       jz   short checksum_combine  ; if zf set, buffer word aligned
       ror  ax,8                    ; byte aligned--swap bytes back

checksum_combine:
       pop  esi                     ; restore nonvolatile register
       adc  eax, 0
       pop  ebx                     ; restore nonvolatile register
       ret


REFLAB macro pre, suf
       dd    pre&suf
       endm

.DATA
align 4

loop_entries label dword
       dd  0
       dd  loop_entry_124
       dd  loop_entry_120
       dd  loop_entry_116
       dd  loop_entry_112
       dd  loop_entry_108
       dd  loop_entry_104
       dd  loop_entry_100
       dd  loop_entry_96
       dd  loop_entry_92
       dd  loop_entry_88
       dd  loop_entry_84
       dd  loop_entry_80
       dd  loop_entry_76
       dd  loop_entry_72
       dd  loop_entry_68
       dd  loop_entry_64
       dd  loop_entry_60
       dd  loop_entry_56
       dd  loop_entry_52
       dd  loop_entry_48
       dd  loop_entry_44
       dd  loop_entry_40
       dd  loop_entry_36
       dd  loop_entry_32
       dd  loop_entry_28
       dd  loop_entry_24
       dd  loop_entry_20
       dd  loop_entry_16
       dd  loop_entry_12
       dd  loop_entry_8
       dd  loop_entry_4

endif ; DOSX

end
