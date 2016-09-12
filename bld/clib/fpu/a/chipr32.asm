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


; static char sccs_id[] = "@(#)fprem32.asm      1.5  12/22/94  12:48:07";
;
; This code is being published by Intel to users of the Pentium(tm)
; processor.  Recipients are authorized to copy, modify, compile, use and
; distribute the code.
;
; Intel makes no warranty of any kind with regard to this code, including
; but not limited to, implied warranties or merchantability and fitness for
; a particular purpose. Intel assumes no responsibility for any errors that
; may appear in this code.
;
; No patent licenses are granted, express or implied.
;
;
include mdef.inc

        .386
        .387

;
;  PRELIMINARY VERSION of the software patch for the floating
;  point remainder.
;


CHECKSW MACRO
ifdef   DEBUG
        fnstsw  [fpsw]
        fnstcw  [fpcw]
endif
ENDM


DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'

;
;  Stack variables for remainder routines.
;

FLT_SIZE        EQU     12
DENOM           EQU     0
DENOM_SAVE      EQU     DENOM + FLT_SIZE
NUMER           EQU     DENOM_SAVE + FLT_SIZE
PREV_CW         EQU     NUMER + FLT_SIZE
PATCH_CW        EQU     PREV_CW + 4
FPREM_SW        EQU     PATCH_CW + 4
STACK_SIZE      EQU     FPREM_SW + 4
RET_SIZE        EQU     4
PUSH_SIZE       EQU     4

MAIN_FUDGE      EQU     RET_SIZE + PUSH_SIZE + PUSH_SIZE + PUSH_SIZE

MAIN_DENOM              EQU     DENOM + MAIN_FUDGE
MAIN_DENOM_SAVE         EQU     DENOM_SAVE + MAIN_FUDGE
MAIN_NUMER              EQU     NUMER + MAIN_FUDGE
MAIN_PREV_CW            EQU     PREV_CW + MAIN_FUDGE
MAIN_PATCH_CW           EQU     PATCH_CW + MAIN_FUDGE
MAIN_FPREM_SW           EQU     FPREM_SW + MAIN_FUDGE

ONESMASK        EQU     700h

fprem_risc_table        DB      0, 1, 0, 0, 4, 0, 0, 7, 0, 0, 10, 0, 0, 13, 0, 0
fprem_scale             DB      0, 0, 0, 0, 0, 0, 0eeh, 03fh
one_shl_64              DB      0, 0, 0, 0, 0, 0, 0f0h, 043h
one_shr_64              DB      0, 0, 0, 0, 0, 0, 0f0h, 03bh
one                     DB      0, 0, 0, 0, 0, 0, 0f0h, 03fh
half                    DB      0, 0, 0, 0, 0, 0, 0e0h, 03fh
big_number              DB      0, 0, 0, 0, 0, 0, 0ffh, 0ffh, 0feh, 07fh

ifdef   DEBUG
        public  fpcw
        public  fpsw
fpcw    dw      0
fpsw    dw      0
endif

FPU_STATE       STRUC
        CONTROL_WORD    DW      ?
        reserved_1      DW      ?
        STATUS_WORD     DD      ?
        TAG_WORD        DW      ?
        reserved_3      DW      ?
        IP_OFFSET       DD      ?
        CS_SLCT         DW      ?
        OPCODE          DW      ?
        DATA_OFFSET     DD      ?
        OPERAND_SLCT    DW      ?
        reserved_4      DW      ?
FPU_STATE       ENDS

ENV_SIZE        EQU     28


DATA  ENDS

_TEXT  SEGMENT DWORD USE32 PUBLIC 'CODE'
_TEXT  ENDS

DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA   ENDS

CONST  SEGMENT DWORD USE32 PUBLIC 'DATA'
CONST  ENDS

BSS    SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS    ENDS

DGROUP  GROUP CONST, BSS, DATA



_TEXT   SEGMENT   DWORD USE32 PUBLIC 'CODE'

        assume cs:_TEXT, ds:DGROUP, es:DGROUP, ss:nothing


fprem_common    PROC    NEAR

        push    eax
        push    ebx
        push    ecx
        mov     eax, [MAIN_DENOM+6+esp] ; exponent and high 16 bits of mantissa
        xor     eax, ONESMASK           ; invert bits that have to be one
        test    eax, ONESMASK           ; check bits that have to be one
        jnz     remainder_hardware_ok
        shr     eax, 11
        and     eax, 0fh
        cmp     byte ptr fprem_risc_table[eax], 0     ; check for (1,4,7,a,d)
        jz      remainder_hardware_ok

; The denominator has the bit pattern. Weed out the funny cases like NaNs
; before applying the software version. Our caller guarantees that the
; denominator is not a denormal. Here we check for:
;       denominator     inf, NaN, unnormal
;       numerator       inf, NaN, unnormal, denormal

        mov     eax, [MAIN_DENOM+6+esp] ; exponent and high 16 bits of mantissa
        and     eax, 07fff0000h         ; mask the exponent only
        cmp     eax, 07fff0000h         ; check for INF or NaN
        je      remainder_hardware_ok
        mov     eax, [MAIN_NUMER+6+esp] ; exponent and high 16 bits of mantissa
        and     eax, 07fff0000h         ; mask the exponent only
        jz      remainder_hardware_ok   ; jif numerator denormal
        cmp     eax, 07fff0000h         ; check for INF or NaN
        je      remainder_hardware_ok
        mov     eax, [esp + MAIN_NUMER + 4]     ; high mantissa bits - numerator
        add     eax, eax                ; set carry if explicit bit set
        jnz     remainder_hardware_ok   ; jmp if numerator is unnormal
        mov     eax, [esp + MAIN_DENOM + 4] ; high mantissa bits - denominator
        add     eax, eax                ; set carry if explicit bit set
        jnz     remainder_hardware_ok   ; jmp if denominator is unnormal

rem_patch:
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        and     eax, 07fffh              ; clear sy
        add     eax, 63                  ; evaluate ey + 63
        mov     ebx, [MAIN_NUMER+8+esp]  ; sign and exponent of x (numerator)
        and     ebx, 07fffh              ; clear sx
        sub     ebx, eax                 ; evaluate the exponent difference (ex - ey)
        ja      rem_large               ; if ex > ey + 63, case of large arguments
rem_patch_loop:
        mov     eax, [MAIN_DENOM+8+esp]  ; sign and exponent of y (denominator)
        and     eax, 07fffh             ; clear sy
        add     eax, 10                 ; evaluate ey + 10
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        sub     ebx, eax                ; evaluate the exponent difference (ex - ey)
        js      remainder_hardware_ok   ; safe if ey + 10 > ex
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        mov     ecx, ebx
        sub     ebx, eax
        and     ebx, 07h
        or      ebx, 04h
        sub     ecx, ebx
        mov     ebx, eax
        and     ebx, 08000h             ; keep sy
        or      ecx, ebx                ; merge the sign of y
        mov     dword ptr [MAIN_DENOM+8+esp], ecx
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the shifted denominator
        mov     dword ptr [MAIN_DENOM+8+esp], eax       ; restore the initial denominator
        fxch
        fprem                           ; this rem is safe
        fstp    tbyte ptr [MAIN_NUMER+esp]      ; update the numerator
        fstp    st(0)                   ; pop the stack
        jmp rem_patch_loop
rem_large:
        test    edx, 02h                ; is denominator already saved
        jnz     already_saved
        fld     tbyte ptr[esp + MAIN_DENOM]
        fstp    tbyte ptr[esp + MAIN_DENOM_SAVE]        ; save denominator
already_saved:
        ; Save user's precision control and institute 80.  The fp ops in
        ; rem_large_loop must not round to user's precision (if it is less
        ; than 80) because the hardware would not have done so.  We are
        ; aping the hardware here, which is all extended.

        fnstcw  [esp+MAIN_PREV_CW]      ; save caller's control word
        mov     eax, dword ptr[esp + MAIN_PREV_CW]
        or      eax, 033fh              ; mask exceptions, pc=80
        mov     [esp + MAIN_PATCH_CW], eax
        fldcw   [esp + MAIN_PATCH_CW]

        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        and     eax, 07fffh             ; clear sy
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        sub     ebx, eax                ; evaluate the exponent difference
        and     ebx, 03fh
        or      ebx, 020h
        add     ebx, 1
        mov     ecx, ebx
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        and     eax, 08000h             ; keep sy
        or      ebx, eax                ; merge the sign of y
        mov     dword ptr[MAIN_DENOM+8+esp], ebx        ; make ey equal to ex (scaled denominator)
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the scaled denominator
        fabs
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        fabs
rem_large_loop:
        fcom
        fstsw  ax
        and     eax, 00100h
        jnz     rem_no_sub
        fsub    st, st(1)
rem_no_sub:
        fxch
        fmul    qword ptr half
        fxch
        sub     ecx, 1                  ; decrement the loop counter
        jnz     rem_large_loop
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        fstp    tbyte ptr[esp + MAIN_NUMER]     ; save result
        fstp    st                      ; toss modified denom
        fld     tbyte ptr[esp + MAIN_DENOM_SAVE]
        fld     tbyte ptr[big_number]   ; force C2 to be set
        fprem
        fstp    st
        fld     tbyte ptr[esp + MAIN_NUMER]     ; restore saved result

        fldcw   [esp + MAIN_PREV_CW]    ; restore caller's control word
        and     ebx, 08000h             ; keep sx
        jz      rem_done
        fchs
        jmp     rem_done
remainder_hardware_ok:
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the denominator
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        fprem                           ; and finally do a remainder
; prem_main_routine end
rem_done:
        test    edx, 03h
        jz      rem_exit
        fnstsw  [esp + MAIN_FPREM_SW]   ; save Q0 Q1 and Q2
        test    edx, 01h
        jz      do_not_de_scale
; De-scale the result. Go to pc=80 to prevent from fmul
; from user precision (fprem does not round the result).
        fnstcw  [esp + MAIN_PREV_CW]    ; save callers control word
        mov     eax, [esp + MAIN_PREV_CW]
        or      eax, 0300h              ; pc = 80
        mov     [esp + MAIN_PATCH_CW], eax
        fldcw   [esp + MAIN_PATCH_CW]
        fmul    qword ptr one_shr_64
        fldcw   [esp + MAIN_PREV_CW]    ; restore callers CW
do_not_de_scale:
        mov     eax, [esp + MAIN_FPREM_SW]
        fxch
        fstp    st
        fld     tbyte ptr[esp + MAIN_DENOM_SAVE]
        fxch
        and     eax, 04300h             ; restore saved Q0, Q1, Q2
        sub     esp, ENV_SIZE
        fnstenv [esp]
        and     [esp].STATUS_WORD, 0bcffh
        or      [esp].STATUS_WORD, eax
        fldenv  [esp]
        add     esp, ENV_SIZE
rem_exit:
        pop     ecx
        pop     ebx
        pop     eax
        CHECKSW                         ; debug only: save status
        ret
fprem_common    ENDP

comment ~****************************************************************

;
; float frem_chk (float numer, float denom)
;
        public  frem_chk
frem_chk        PROC    NEAR
        push    edx
        sub     esp, STACK_SIZE
        fld     dword ptr [STACK_SIZE+8+esp]
        fstp    tbyte ptr [NUMER+esp]
        fld     dword ptr [STACK_SIZE+12+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx                ; dx = 1 if denormal extended divisor
        call    fprem_common
        fxch
        fstp    st
        add     esp, STACK_SIZE
        pop     edx
        ret
frem_chk        ENDP
; end frem_chk

;
; double drem_chk (double numer, double denom)
;
        public  drem_chk
drem_chk        PROC    NEAR
        push    edx
        sub     esp, STACK_SIZE
        fld     qword ptr [STACK_SIZE+8+esp]
        fstp    tbyte ptr [NUMER+esp]
        fld     qword ptr [STACK_SIZE+16+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx                ; dx = 1 if denormal extended divisor
        call    fprem_common
        fxch
        fstp    st
        add     esp, STACK_SIZE
        pop     edx
        ret

drem_chk        ENDP
; end drem_chk

;
; long double lrem_chk(long double number,long double denom)
;
        public  lrem_chk
lrem_chk        PROC    NEAR
        fld     tbyte ptr [20+esp]
        fld     tbyte ptr [4+esp]
        call    fprem_chk
        fxch
        fstp    st
        ret
lrem_chk        ENDP

**********************************************************************~

;
; FPREM: ST = remainder(ST, ST(1))
;
; Compiler version of the FPREM must preserve the arguments in the floating
; point stack.

        public  __fprem_chk
        defpe   __fprem_chk
        push    edx
        sub     esp, STACK_SIZE
        fstp    tbyte ptr [NUMER+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx
; prem_main_routine begin
        mov     eax,[DENOM+6+esp]       ; exponent and high 16 bits of mantissa
        test    eax,07fff0000h          ; check for denormal
        jz      denormal
        call    fprem_common
        add     esp, STACK_SIZE
        pop     edx
        ret

denormal:
        fld     tbyte ptr [DENOM+esp]   ; load the denominator
        fld     tbyte ptr [NUMER+esp]   ; load the numerator
        mov     eax, [DENOM+esp]        ; test for whole mantissa == 0
        or      eax, [DENOM+4+esp]      ; test for whole mantissa == 0
        jz      remainder_hardware_ok_l ; denominator is zero
        fxch
        fstp    tbyte ptr[esp + DENOM_SAVE]     ; save org denominator
        fld     tbyte ptr[esp + DENOM]
        fxch
        or      edx, 02h
;
; For this we need pc=80.  Also, mask exceptions so we don't take any
; denormal operand exceptions.  It is guaranteed that the descaling
; later on will take underflow, which is what the hardware would have done
; on a normal fprem.
;
        fnstcw  [PREV_CW+esp]           ; save caller's control word
        mov     eax, [PREV_CW+esp]
        or      eax, 0033fh             ; mask exceptions, pc=80
        mov     [PATCH_CW+esp], eax
        fldcw   [PATCH_CW+esp]          ; mask exceptions & pc=80

; The denominator is a denormal.  For most numerators, scale both numerator
; and denominator to get rid of denormals.  Then execute the common code
; with the flag set to indicate that the result must be de-scaled.
; For large numerators this won't work because the scaling would cause
; overflow.  In this case we know the numerator is large, the denominator
; is small (denormal), so the exponent difference is also large.  This means
; the rem_large code will be used and this code depends on the difference
; in exponents modulo 64.  Adding 64 to the denominators exponent
; doesn't change the modulo 64 difference.  So we can scale the denominator
; by 64, making it not denormal, and this won't effect the result.
;
; To start with, figure out if numerator is large

        mov     eax, [esp + NUMER + 8]  ; load numerator exponent
        and     eax, 7fffh              ; isolate numerator exponent
        cmp     eax, 7fbeh              ; compare Nexp to Maxexp-64
        ja      big_numer_rem_de        ; jif big numerator

; So the numerator is not large scale both numerator and denominator

        or      edx, 1                  ; edx = 1, if denormal extended divisor
        fmul    qword ptr one_shl_64    ; make numerator not denormal
        fstp    tbyte ptr[esp + NUMER]
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[esp + DENOM]
        jmp     scaling_done

; The numerator is large.  Scale only the denominator, which will not
; change the result which we know will be partial.  Set the scale flag
; to false.
big_numer_rem_de:
        ; We must do this with pc=80 to avoid rounding to single/double.
        ; In this case we do not mask exceptions so that we will take
        ; denormal operand, as would the hardware.
        fnstcw  [PREV_CW+esp]           ; save caller's control word
        mov     eax, [PREV_CW+esp]
        or      eax, 00300h             ; pc=80
        mov     [PATCH_CW+esp], eax
        fldcw   [PATCH_CW+esp]          ;  pc=80

        fstp    st                      ; Toss numerator
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[esp + DENOM]

; Restore the control word which was fiddled to scale at 80-bit precision.
; Then call the common code.
scaling_done:
        fldcw   [esp + PREV_CW]         ; restore callers control word
        call    fprem_common
        add     esp, STACK_SIZE
        pop     edx
        ret

remainder_hardware_ok_l:
        fprem                           ; and finally do a remainder

        CHECKSW

        add     esp, STACK_SIZE
        pop     edx
        ret
__fprem_chk       ENDP
; end fprem_chk


;
; FPREM1 code begins here
;


fprem1_common   PROC    NEAR

        push    eax
        push    ebx
        push    ecx
        mov     eax, [MAIN_DENOM+6+esp] ; exponent and high 16 bits of mantissa
        xor     eax, ONESMASK           ; invert bits that have to be one
        test    eax, ONESMASK           ; check bits that have to be one
        jnz     remainder1_hardware_ok
        shr     eax, 11
        and     eax, 0fh
        cmp     byte ptr fprem_risc_table[eax], 0     ; check for (1,4,7,a,d)
        jz      remainder1_hardware_ok

; The denominator has the bit pattern. Weed out the funny cases like NaNs
; before applying the software version. Our caller guarantees that the
; denominator is not a denormal. Here we check for:
;       denominator     inf, NaN, unnormal
;       numerator       inf, NaN, unnormal, denormal

        mov     eax, [MAIN_DENOM+6+esp] ; exponent and high 16 bits of mantissa
        and     eax, 07fff0000h         ; mask the exponent only
        cmp     eax, 07fff0000h         ; check for INF or NaN
        je      remainder1_hardware_ok
        mov     eax, [MAIN_NUMER+6+esp] ; exponent and high 16 bits of mantissa
        and     eax, 07fff0000h         ; mask the exponent only
        jz      remainder1_hardware_ok  ; jif numerator denormal
        cmp     eax, 07fff0000h         ; check for INF or NaN
        je      remainder1_hardware_ok
        mov     eax, [esp + MAIN_NUMER + 4]     ; high mantissa bits - numerator
        add     eax, eax                ; set carry if explicit bit set
        jnz     remainder1_hardware_ok  ; jmp if numerator is unnormal
        mov     eax, [esp + MAIN_DENOM + 4] ; high mantissa bits - denominator
        add     eax, eax                ; set carry if explicit bit set
        jnz     remainder1_hardware_ok  ; jmp if denominator is unnormal

rem1_patch:
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        and     eax, 07fffh              ; clear sy
        add     eax, 63                  ; evaluate ey + 63
        mov     ebx, [MAIN_NUMER+8+esp]  ; sign and exponent of x (numerator)
        and     ebx, 07fffh              ; clear sx
        sub     ebx, eax                 ; evaluate the exponent difference (ex - ey)
        ja      rem1_large              ; if ex > ey + 63, case of large arguments
rem1_patch_loop:
        mov     eax, [MAIN_DENOM+8+esp]  ; sign and exponent of y (denominator)
        and     eax, 07fffh             ; clear sy
        add     eax, 10                 ; evaluate ey + 10
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        sub     ebx, eax                ; evaluate the exponent difference (ex - ey)
        js      remainder1_hardware_ok  ; safe if ey + 10 > ex
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        mov     ecx, ebx
        sub     ebx, eax
        and     ebx, 07h
        or      ebx, 04h
        sub     ecx, ebx
        mov     ebx, eax
        and     ebx, 08000h             ; keep sy
        or      ecx, ebx                ; merge the sign of y
        mov     dword ptr [MAIN_DENOM+8+esp], ecx
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the shifted denominator
        mov     dword ptr [MAIN_DENOM+8+esp], eax       ; restore the initial denominator
        fxch
        fprem                           ; this rem is safe
        fstp    tbyte ptr [MAIN_NUMER+esp]      ; update the numerator
        fstp    st(0)                   ; pop the stack
        jmp rem1_patch_loop
rem1_large:
        test    ebx, 02h                ; is denominator already saved
        jnz     already_saved1
        fld     tbyte ptr[esp + MAIN_DENOM]
        fstp    tbyte ptr[esp + MAIN_DENOM_SAVE]        ; save denominator
already_saved1:
        ; Save user's precision control and institute 80.  The fp ops in
        ; rem1_large_loop must not round to user's precision (if it is less
        ; than 80) because the hardware would not have done so.  We are
        ; aping the hardware here, which is all extended.

        fnstcw  [esp+MAIN_PREV_CW]      ; save caller's control word
        mov     eax, dword ptr[esp + MAIN_PREV_CW]
        or      eax, 033fh              ; mask exceptions, pc=80
        mov     [esp + MAIN_PATCH_CW], eax
        fldcw   [esp + MAIN_PATCH_CW]

        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        and     eax, 07fffh             ; clear sy
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        sub     ebx, eax                ; evaluate the exponent difference
        and     ebx, 03fh
        or      ebx, 020h
        add     ebx, 1
        mov     ecx, ebx
        mov     eax, [MAIN_DENOM+8+esp] ; sign and exponent of y (denominator)
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        and     ebx, 07fffh             ; clear sx
        and     eax, 08000h             ; keep sy
        or      ebx, eax                ; merge the sign of y
        mov     dword ptr[MAIN_DENOM+8+esp], ebx        ; make ey equal to ex (scaled denominator)
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the scaled denominator
        fabs
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        fabs
rem1_large_loop:
        fcom
        fstsw  ax
        and     eax, 00100h
        jnz     rem1_no_sub
        fsub    st, st(1)
rem1_no_sub:
        fxch
        fmul    qword ptr half
        fxch
        sub     ecx, 1                  ; decrement the loop counter
        jnz     rem1_large_loop
        mov     ebx, [MAIN_NUMER+8+esp] ; sign and exponent of x (numerator)
        fstp    tbyte ptr[esp + MAIN_NUMER]     ; save result
        fstp    st                      ; toss modified denom
        fld     tbyte ptr[esp + MAIN_DENOM_SAVE]
        fld     tbyte ptr[big_number]   ; force C2 to be set
        fprem1
        fstp    st
        fld     tbyte ptr[esp + MAIN_NUMER]     ; restore saved result

        fldcw   [esp + MAIN_PREV_CW]    ; restore caller's control word
        and     ebx, 08000h             ; keep sx
        jz      rem1_done
        fchs
        jmp     rem1_done
remainder1_hardware_ok:
        fld     tbyte ptr [MAIN_DENOM+esp]   ; load the denominator
        fld     tbyte ptr [MAIN_NUMER+esp]   ; load the numerator
        fprem1                           ; and finally do a remainder
; prem1_main_routine end
rem1_done:
        test    edx, 03h
        jz      rem1_exit
        fnstsw  [esp + MAIN_FPREM_SW]   ; save Q0 Q1 and Q2
        test    edx, 01h
        jz      do_not_de_scale1
; De-scale the result. Go to pc=80 to prevent from fmul
; from user precision (fprem does not round the result).
        fnstcw  [esp + MAIN_PREV_CW]    ; save callers control word
        mov     eax, [esp + MAIN_PREV_CW]
        or      eax, 0300h              ; pc = 80
        mov     [esp + MAIN_PATCH_CW], eax
        fldcw   [esp + MAIN_PATCH_CW]
        fmul    qword ptr one_shr_64
        fldcw   [esp + MAIN_PREV_CW]    ; restore callers CW
do_not_de_scale1:
        mov     eax, [esp + MAIN_FPREM_SW]
        fxch
        fstp    st
        fld     tbyte ptr[esp + MAIN_DENOM_SAVE]
        fxch
        and     eax, 04300h             ; restore saved Q0, Q1, Q2
        sub     esp, ENV_SIZE
        fnstenv [esp]
        and     [esp].STATUS_WORD, 0bcffh
        or      [esp].STATUS_WORD, eax
        fldenv  [esp]
        add     esp, ENV_SIZE
rem1_exit:
        pop     ecx
        pop     ebx
        pop     eax
        CHECKSW                         ; debug only: save status
        ret
fprem1_common   ENDP


comment ~***************************************************************
;
; float frem1_chk (float numer, float denom)
;
        public  frem1_chk
frem1_chk       PROC    NEAR
        push    edx
        sub     esp, STACK_SIZE
        fld     dword ptr [STACK_SIZE+8+esp]
        fstp    tbyte ptr [NUMER+esp]
        fld     dword ptr [STACK_SIZE+12+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx                ; dx = 1 if denormal extended divisor
        call    fprem1_common
        fxch
        fstp    st
        add     esp, STACK_SIZE
        pop     edx
        ret
frem1_chk       ENDP
; end frem1_chk

;
; double drem1_chk (double numer, double denom)
;
        public  drem1_chk
drem1_chk       PROC    NEAR
        push    edx
        sub     esp, STACK_SIZE
        fld     qword ptr [STACK_SIZE+8+esp]
        fstp    tbyte ptr [NUMER+esp]
        fld     qword ptr [STACK_SIZE+16+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx                ; dx = 1 if denormal extended divisor
        call    fprem1_common
        fxch
        fstp    st
        add     esp, STACK_SIZE
        pop     edx
        ret

drem1_chk       ENDP
; end drem1_chk

;
; long double lrem1_chk(long double number,long double denom)
;
        public  lrem1_chk
lrem1_chk       PROC    NEAR
        fld     tbyte ptr [20+esp]
        fld     tbyte ptr [4+esp]
        call    fprem1_chk
        fxch
        fstp    st
        ret
lrem1_chk       ENDP
********************************************************************~

;
; FPREM1: ST = remainder(ST, ST(1)) - IEEE version of rounding
;
; Compiler version of the FPREM must preserve the arguments in the floating
; point stack.

        public  __fprem1_chk
        defpe   __fprem1_chk
        push    edx
        sub     esp, STACK_SIZE
        fstp    tbyte ptr [NUMER+esp]
        fstp    tbyte ptr [DENOM+esp]
        xor     edx, edx
; prem1_main_routine begin
        mov     eax,[DENOM+6+esp]       ; exponent and high 16 bits of mantissa
        test    eax,07fff0000h          ; check for denormal
        jz      denormal1
        call    fprem1_common
        add     esp, STACK_SIZE
        pop     edx
        ret

denormal1:
        fld     tbyte ptr [DENOM+esp]   ; load the denominator
        fld     tbyte ptr [NUMER+esp]   ; load the numerator
        mov     eax, [DENOM+esp]        ; test for whole mantissa == 0
        or      eax, [DENOM+4+esp]      ; test for whole mantissa == 0
        jz      remainder1_hardware_ok_l ; denominator is zero
        fxch
        fstp    tbyte ptr[esp + DENOM_SAVE]     ; save org denominator
        fld     tbyte ptr[esp + DENOM]
        fxch
        or      edx, 02h
;
; For this we need pc=80.  Also, mask exceptions so we don't take any
; denormal operand exceptions.  It is guaranteed that the descaling
; later on will take underflow, which is what the hardware would have done
; on a normal fprem.
;
        fnstcw  [PREV_CW+esp]           ; save caller's control word
        mov     eax, [PREV_CW+esp]
        or      eax, 0033fh             ; mask exceptions, pc=80
        mov     [PATCH_CW+esp], eax
        fldcw   [PATCH_CW+esp]          ; mask exceptions & pc=80

; The denominator is a denormal.  For most numerators, scale both numerator
; and denominator to get rid of denormals.  Then execute the common code
; with the flag set to indicate that the result must be de-scaled.
; For large numerators this won't work because the scaling would cause
; overflow.  In this case we know the numerator is large, the denominator
; is small (denormal), so the exponent difference is also large.  This means
; the rem1_large code will be used and this code depends on the difference
; in exponents modulo 64.  Adding 64 to the denominators exponent
; doesn't change the modulo 64 difference.  So we can scale the denominator
; by 64, making it not denormal, and this won't effect the result.
;
; To start with, figure out if numerator is large

        mov     eax, [esp + NUMER + 8]  ; load numerator exponent
        and     eax, 7fffh              ; isolate numerator exponent
        cmp     eax, 7fbeh              ; compare Nexp to Maxexp-64
        ja      big_numer_rem1_de       ; jif big numerator

; So the numerator is not large scale both numerator and denominator

        or      edx, 1                  ; edx = 1, if denormal extended divisor
        fmul    qword ptr one_shl_64    ; make numerator not denormal
        fstp    tbyte ptr[esp + NUMER]
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[esp + DENOM]
        jmp     scaling_done1

; The numerator is large.  Scale only the denominator, which will not
; change the result which we know will be partial.  Set the scale flag
; to false.
big_numer_rem1_de:
        ; We must do this with pc=80 to avoid rounding to single/double.
        ; In this case we do not mask exceptions so that we will take
        ; denormal operand, as would the hardware.
        fnstcw  [PREV_CW+esp]           ; save caller's control word
        mov     eax, [PREV_CW+esp]
        or      eax, 00300h             ; pc=80
        mov     [PATCH_CW+esp], eax
        fldcw   [PATCH_CW+esp]          ;  pc=80

        fstp    st                      ; Toss numerator
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[esp + DENOM]

; Restore the control word which was fiddled to scale at 80-bit precision.
; Then call the common code.
scaling_done1:
        fldcw   [esp + PREV_CW]         ; restore callers control word
        call    fprem1_common
        add     esp, STACK_SIZE
        pop     edx
        ret

remainder1_hardware_ok_l:
        fprem                           ; and finally do a remainder

        CHECKSW

        add     esp, STACK_SIZE
        pop     edx
        ret
__fprem1_chk      ENDP
; end fprem1_chk

ifdef   DEBUG
        public  fpinit
fpinit  PROC    NEAR
        fninit
        ret
fpinit  ENDP
endif

_TEXT  ENDS
       END
