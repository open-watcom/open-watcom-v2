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


; static char sccs_id[] = "@(#)fprem16.asm      1.6  12/22/94  12:48:05";
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

_TEXT  SEGMENT   DWORD USE16 PUBLIC 'CODE'

CHECKSW MACRO
ifdef   DEBUG
        fnstsw  [_fpsw]
        fnstcw  [_fpcw]
endif
ENDM


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
RET_SIZE        EQU     2
PUSH_SIZE       EQU     2

MAIN_FUDGE      EQU     RET_SIZE + 4 * PUSH_SIZE

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


comment ~**************************************************************
__fprem_result          DQ      0
__fprem1_result         DQ      0
**********************************************************************~

ifdef   DEBUG
        public  _fpcw
        public  _fpsw
_fpcw   dw      0
_fpsw   dw      0
endif

FPU_STATE       STRUC
        CONTROL_WORD    DW      ?
        STATUS_WORD     DW      ?
        TAG_WORD        DW      ?
        IP_OFFSET       DW      ?
        CS_SLCT         DW      ?
        OPERAND_OFF     DW      ?
        OPERAND_SLCT    DW      ?
FPU_STATE       ENDS

ENV_SIZE        EQU     14


        assume cs:_TEXT, ds:nothing, es:nothing, ss:nothing


fprem_common    PROC    NEAR

        push    ax
        push    bx
        push    cx
        push    bp
        mov     bp, sp
        mov     ax, [MAIN_DENOM+6+bp]   ; high 16 bits of mantissa
        xor     ax, ONESMASK            ; invert bits that have to be one
        test    ax, ONESMASK            ; check bits that have to be one
        jnz     remainder_hardware_ok
        shr     ax, 11
        and     ax, 0fh
        mov     bx, ax
        cmp     byte ptr fprem_risc_table[bx], 0     ; check for (1,4,7,a,d)
        jz      remainder_hardware_ok

; The denominator has the bit pattern. Weed out the funny cases like NaNs
; before applying the software version. Our caller guarantees that the
; denominator is not a denormal. Here we check for:
;       denominator     inf, NaN, unnormal
;       numerator       inf, NaN, unnormal, denormal

        mov     ax, [MAIN_DENOM+8+bp]   ; exponent
        and     ax, 7fffh               ; mask the exponent only
        cmp     ax, 7fffh               ; check for INF or NaN
        je      remainder_hardware_ok
        mov     ax, [MAIN_NUMER+8+bp]   ; exponent
        and     ax, 07fffh              ; mask the exponent only
        jz      remainder_hardware_ok   ; jif numerator denormal
        cmp     ax, 07fffh              ; check for INF or NaN
        je      remainder_hardware_ok
        mov     ax, [bp + MAIN_NUMER + 6]       ; high mantissa bits - numerator
        add     ax, ax                  ; set carry if explicit bit set
        jnz     remainder_hardware_ok   ; jmp if numerator is unnormal
        mov     ax, [bp + MAIN_DENOM + 6] ; high mantissa bits - denominator
        add     ax, ax          ; set carry if explicit bit set
        jnz     remainder_hardware_ok   ; jmp if denominator is unnormal
rem_patch:
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        add     ax, 63                  ; evaluate ey + 63
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference (ex - ey)
        ja      rem_large               ; if ex > ey + 63, case of large arguments
rem_patch_loop:
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        add     ax, 10                  ; evaluate ey + 10
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference (ex - ey)
        js      remainder_hardware_ok   ; safe if ey + 10 > ex
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        mov     cx, bx
        sub     bx, ax
        and     bx, 07h
        or      bx, 04h
        sub     cx, bx
        mov     bx, ax
        and     bx, 08000h              ; keep sy
        or      cx, bx                  ; merge the sign of y
        mov     word ptr [MAIN_DENOM+8+bp], cx
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the shifted denominator
        mov     word ptr [MAIN_DENOM+8+bp], ax  ; restore the initial denominator
        fxch
        fprem                           ; this rem is safe
        fstp    tbyte ptr [MAIN_NUMER+bp]       ; update the numerator
        fstp    st(0)                   ; pop the stack
        jmp rem_patch_loop
rem_large:
        test    dx, 02h                 ; is denominator already saved
        jnz     already_saved
        fld     tbyte ptr[bp + MAIN_DENOM]
        fstp    tbyte ptr[bp + MAIN_DENOM_SAVE] ; save denominator
already_saved:
        ; Save user's precision control and institute 80.  The fp ops in
        ; rem_large_loop must not round to user's precision (if it is less
        ; than 80) because the hardware would not have done so.  We are
        ; aping the hardware here, which is all extended.

        fnstcw  [bp+MAIN_PREV_CW]       ; save caller's control word
        mov     ax, word ptr[bp + MAIN_PREV_CW]
        or      ax, 033fh               ; mask exceptions, pc=80
        mov     [bp + MAIN_PATCH_CW], ax
        fldcw   [bp + MAIN_PATCH_CW]
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference
        and     bx, 03fh
        or      bx, 020h
        add     bx, 1
        mov     cx, bx
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        and     ax, 08000h              ; keep sy
        or      bx, ax                  ; merge the sign of y
        mov     word ptr[MAIN_DENOM+8+bp], bx   ; make ey equal to ex (scaled denominator)
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the scaled denominator
        fabs
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        fabs
rem_large_loop:
        fcom
        fstsw  ax
        and     ax, 00100h
        jnz     rem_no_sub
        fsub    st, st(1)
rem_no_sub:
        fxch
        fmul    qword ptr half
        fxch
        sub     cx, 1                   ; decrement the loop counter
        jnz     rem_large_loop
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)

        fstp    tbyte ptr[bp + MAIN_NUMER]      ; save result
        fstp    st                      ; toss modified denom
        fld     tbyte ptr[bp + MAIN_DENOM_SAVE]
        fld     tbyte ptr[big_number]   ; force C2 to be set
        fprem
        fstp    st
        fld     tbyte ptr[bp + MAIN_NUMER]      ; restore saved result

        fldcw   [bp + MAIN_PREV_CW]     ; restore caller's control word
        and     bx, 08000h              ; keep sx
        jz      rem_done
        fchs
        jmp     rem_done
remainder_hardware_ok:
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the denominator
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        fprem                           ; and finally do a remainder

; prem_main_routine end
rem_done:
        test    dx, 3h
        jz      rem_exit
        fnstsw  [bp + MAIN_FPREM_SW]    ; save Q0 Q1 and Q2
        test    edx, 01h
        jz      do_not_de_scale
; De-scale the result. Go to pc=80 to prevent from fmul
; from user precision (fprem does not round the result).
        fnstcw  [bp + MAIN_PREV_CW]     ; save callers control word
        mov     ax, [bp + MAIN_PREV_CW]
        or      eax, 0300h              ; pc = 80
        mov     [bp + MAIN_PATCH_CW], ax
        fldcw   [bp + MAIN_PATCH_CW]
        fmul    qword ptr one_shr_64
        fldcw   [bp + MAIN_PREV_CW]     ; restore callers CW
do_not_de_scale:
        mov     ax, [bp + MAIN_FPREM_SW]
        fxch
        fstp    st
        fld     tbyte ptr[bp + MAIN_DENOM_SAVE]
        fxch
        and     ax, 04300h              ; restore saved Q0, Q1, Q2
        sub     sp, ENV_SIZE
        mov     bp, sp
        fnstenv [bp]
        and     [bp].STATUS_WORD, 0bcffh
        or      [bp].STATUS_WORD, ax
        fldenv  [bp]
        add     sp, ENV_SIZE
rem_exit:
        pop     bp
        pop     cx
        pop     bx
        pop     ax
        CHECKSW                         ; debug only: save status
        ret
fprem_common    ENDP


comment ~**************************************************************
;
; float _frem_chk (float numer, float denom)
;
        public  _frem_chk
_frem_chk       PROC    FAR
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fld     dword ptr [STACK_SIZE+8+bp]
        fstp    tbyte ptr [NUMER+bp]
        fld     dword ptr [STACK_SIZE+12+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx                  ; dx = 1 if denormal extended divisor
        call    fprem_common

        fstp    dword ptr [__fprem_result]
        mov     dx,ds
        mov     ax, offset __fprem_result

        fstp    st                      ; clean FP stack
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret
_frem_chk       ENDP
; end _frem_chk

;
; double _drem_chk (double numer, double denom)
;
        public  _drem_chk
_drem_chk       PROC    FAR
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fld     qword ptr [STACK_SIZE+8+bp]
        fstp    tbyte ptr [NUMER+bp]
        fld     qword ptr [STACK_SIZE+16+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx                  ; dx = 1 if denormal extended divisor
        call    fprem_common
        fstp    qword ptr [__fprem_result]
        mov     dx,ds
        mov     ax, offset __fprem_result
        fstp    st                      ; clean FP stack
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

_drem_chk       ENDP
; end drem_chk

;
; long double _lrem_chk(long double number,long double denom)
;
        public  _lrem_chk
_lrem_chk       PROC    NEAR
        push    bp
        mov     bp, sp
        fld     tbyte ptr [16+bp]       ; assumes long double push
                                        ; is 10 bytes, 18 = 8 + long double push                                        ; size.
        fld     tbyte ptr [6+bp]
        call    _fprem_chk
        fxch
        fstp    st
        pop     bp
        ret
_lrem_chk       ENDP
***********************************************************************~

;
; FPREM: ST = remainder(ST, ST(1))
;
; Compiler version of the FPREM must preserve the arguments in the floating
; point stack.

        public  __fprem_chk
        defpe   __fprem_chk
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fstp    tbyte ptr [NUMER+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx
; prem_main_routine begin
        mov     ax,[DENOM+8+bp]         ; exponent
        test    ax,07fffh               ; check for denormal
        jz      denormal
        call    fprem_common
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

denormal:
        fld     tbyte ptr [DENOM+bp]    ; load the denominator
        fld     tbyte ptr [NUMER+bp]    ; load the numerator
        mov     eax, dword ptr[DENOM+bp]        ; test for whole mantissa == 0
        or      eax, dword ptr[DENOM+4+bp]      ; test for whole mantissa == 0
        jz      remainder_hardware_ok_l ; denominator is zero
        fxch
        fstp    tbyte ptr[bp + DENOM_SAVE]      ; save org denominator
        fld     tbyte ptr[bp + DENOM]
        fxch
        or      dx, 02h
;
; For this we need pc=80.  Also, mask exceptions so we don't take any
; denormal operand exceptions.  It is guaranteed that the descaling
; later on will take underflow, which is what the hardware would have done
; on a normal fprem.
;
        fnstcw  [PREV_CW+bp]            ; save caller's control word
        mov     ax, [PREV_CW+bp]
        or      ax, 0033fh              ; mask exceptions, pc=80
        mov     [PATCH_CW+bp], ax
        fldcw   [PATCH_CW+bp]           ; mask exceptions & pc=80

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

        mov     ax, [bp + NUMER + 8]    ; load numerator exponent
        and     ax, 7fffh               ; isolate numerator exponent
        cmp     ax, 7fbeh               ; compare Nexp to Maxexp-64
        ja      big_numer_rem_de        ; jif big numerator

; So the numerator is not large scale both numerator and denominator

        or      dx, 1                   ; dx = 1, if denormal extended divisor
        fmul    qword ptr one_shl_64    ; make numerator not denormal
        fstp    tbyte ptr[bp + NUMER]
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[bp + DENOM]
        jmp     scaling_done

; The numerator is large.  Scale only the denominator, which will not
; change the result which we know will be partial.  Set the scale flag
; to false.
big_numer_rem_de:
        ; We must do this with pc=80 to avoid rounding to single/double.
        ; In this case we do not mask exceptions so that we will take
        ; denormal operand, as would the hardware.
        fnstcw  [PREV_CW+bp]            ; save caller's control word
        mov     ax, [PREV_CW+bp]
        or      ax, 00300h              ; pc=80
        mov     [PATCH_CW+bp], ax
        fldcw   [PATCH_CW+bp]           ; pc=80

        fstp    st                      ; Toss numerator
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[bp + DENOM]

; Restore the control word which was fiddled to scale at 80-bit precision.
; Then call the common code.
scaling_done:
        fldcw   [bp + PREV_CW]          ; restore callers control word
        call    fprem_common
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

remainder_hardware_ok_l:
        fprem                           ; and finally do a remainder

        CHECKSW

        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret
__fprem_chk      ENDP
; end _fprem_chk

;
;       FPREM1 CODE BEGINS
;


fprem1_common   PROC    NEAR

        push    ax
        push    bx
        push    cx
        push    bp
        mov     bp, sp
        mov     ax, [MAIN_DENOM+6+bp]   ; high 16 bits of mantissa
        xor     ax, ONESMASK            ; invert bits that have to be one
        test    ax, ONESMASK            ; check bits that have to be one
        jnz     remainder1_hardware_ok
        shr     ax, 11
        and     ax, 0fh
        mov     bx, ax
        cmp     byte ptr fprem_risc_table[bx], 0     ; check for (1,4,7,a,d)
        jz      remainder1_hardware_ok

; The denominator has the bit pattern. Weed out the funny cases like NaNs
; before applying the software version. Our caller guarantees that the
; denominator is not a denormal. Here we check for:
;       denominator     inf, NaN, unnormal
;       numerator       inf, NaN, unnormal, denormal

        mov     ax, [MAIN_DENOM+8+bp]   ; exponent
        and     ax, 7fffh               ; mask the exponent only
        cmp     ax, 7fffh               ; check for INF or NaN
        je      remainder1_hardware_ok
        mov     ax, [MAIN_NUMER+8+bp]   ; exponent
        and     ax, 07fffh              ; mask the exponent only
        jz      remainder1_hardware_ok  ; jif numerator denormal
        cmp     ax, 07fffh              ; check for INF or NaN
        je      remainder1_hardware_ok
        mov     ax, [bp + MAIN_NUMER + 6]       ; high mantissa bits - numerator
        add     ax, ax                  ; set carry if explicit bit set
        jnz     remainder1_hardware_ok  ; jmp if numerator is unnormal
        mov     ax, [bp + MAIN_DENOM + 6] ; high mantissa bits - denominator
        add     ax, ax          ; set carry if explicit bit set
        jnz     remainder1_hardware_ok  ; jmp if denominator is unnormal
rem1_patch:
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        add     ax, 63                  ; evaluate ey + 63
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference (ex - ey)
        ja      rem1_large              ; if ex > ey + 63, case of large arguments
rem1_patch_loop:
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        add     ax, 10                  ; evaluate ey + 10
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference (ex - ey)
        js      remainder1_hardware_ok  ; safe if ey + 10 > ex
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        mov     cx, bx
        sub     bx, ax
        and     bx, 07h
        or      bx, 04h
        sub     cx, bx
        mov     bx, ax
        and     bx, 08000h              ; keep sy
        or      cx, bx                  ; merge the sign of y
        mov     word ptr [MAIN_DENOM+8+bp], cx
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the shifted denominator
        mov     word ptr [MAIN_DENOM+8+bp], ax  ; restore the initial denominator
        fxch
        fprem                           ; this rem is safe
        fstp    tbyte ptr [MAIN_NUMER+bp]       ; update the numerator
        fstp    st(0)                   ; pop the stack
        jmp rem1_patch_loop
rem1_large:
        test    dx, 02h                 ; is denominator already saved
        jnz     already_saved1
        fld     tbyte ptr[bp + MAIN_DENOM]
        fstp    tbyte ptr[bp + MAIN_DENOM_SAVE] ; save denominator
already_saved1:
        ; Save user's precision control and institute 80.  The fp ops in
        ; rem1_large_loop must not round to user's precision (if it is less
        ; than 80) because the hardware would not have done so.  We are
        ; aping the hardware here, which is all extended.

        fnstcw  [bp+MAIN_PREV_CW]       ; save caller's control word
        mov     ax, word ptr[bp + MAIN_PREV_CW]
        or      ax, 033fh               ; mask exceptions, pc=80
        mov     [bp + MAIN_PATCH_CW], ax
        fldcw   [bp + MAIN_PATCH_CW]
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        and     ax, 07fffh              ; clear sy
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        sub     bx, ax                  ; evaluate the exponent difference
        and     bx, 03fh
        or      bx, 020h
        add     bx, 1
        mov     cx, bx
        mov     ax, [MAIN_DENOM+8+bp]   ; sign and exponent of y (denominator)
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)
        and     bx, 07fffh              ; clear sx
        and     ax, 08000h              ; keep sy
        or      bx, ax                  ; merge the sign of y
        mov     word ptr[MAIN_DENOM+8+bp], bx   ; make ey equal to ex (scaled denominator)
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the scaled denominator
        fabs
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        fabs
rem1_large_loop:
        fcom
        fstsw  ax
        and     ax, 00100h
        jnz     rem1_no_sub
        fsub    st, st(1)
rem1_no_sub:
        fxch
        fmul    qword ptr half
        fxch
        sub     cx, 1                   ; decrement the loop counter
        jnz     rem1_large_loop
        mov     bx, [MAIN_NUMER+8+bp]   ; sign and exponent of x (numerator)

        fstp    tbyte ptr[bp + MAIN_NUMER]      ; save result
        fstp    st                      ; toss modified denom
        fld     tbyte ptr[bp + MAIN_DENOM_SAVE]
        fld     tbyte ptr[big_number]   ; force C2 to be set
        fprem1
        fstp    st
        fld     tbyte ptr[bp + MAIN_NUMER]      ; restore saved result

        fldcw   [bp + MAIN_PREV_CW]     ; restore caller's control word
        and     bx, 08000h              ; keep sx
        jz      rem1_done
        fchs
        jmp     rem1_done
remainder1_hardware_ok:
        fld     tbyte ptr [MAIN_DENOM+bp]   ; load the denominator
        fld     tbyte ptr [MAIN_NUMER+bp]   ; load the numerator
        fprem1                           ; and finally do a remainder

; prem1_main_routine end
rem1_done:
        test    dx, 3h
        jz      rem1_exit
        fnstsw  [bp + MAIN_FPREM_SW]    ; save Q0 Q1 and Q2
        test    edx, 01h
        jz      do_not_de_scale1
; De-scale the result. Go to pc=80 to prevent from fmul
; from user precision (fprem does not round the result).
        fnstcw  [bp + MAIN_PREV_CW]     ; save callers control word
        mov     ax, [bp + MAIN_PREV_CW]
        or      eax, 0300h              ; pc = 80
        mov     [bp + MAIN_PATCH_CW], ax
        fldcw   [bp + MAIN_PATCH_CW]
        fmul    qword ptr one_shr_64
        fldcw   [bp + MAIN_PREV_CW]     ; restore callers CW
do_not_de_scale1:
        mov     ax, [bp + MAIN_FPREM_SW]
        fxch
        fstp    st
        fld     tbyte ptr[bp + MAIN_DENOM_SAVE]
        fxch
        and     ax, 04300h              ; restore saved Q0, Q1, Q2
        sub     sp, ENV_SIZE
        mov     bp, sp
        fnstenv [bp]
        and     [bp].STATUS_WORD, 0bcffh
        or      [bp].STATUS_WORD, ax
        fldenv  [bp]
        add     sp, ENV_SIZE
rem1_exit:
        pop     bp
        pop     cx
        pop     bx
        pop     ax
        CHECKSW                         ; debug only: save status
        ret
fprem1_common   ENDP


comment ~*****************************************************************
;
; float _frem1_chk (float numer, float denom)
;
        public  _frem1_chk
_frem1_chk      PROC    FAR
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fld     dword ptr [STACK_SIZE+8+bp]
        fstp    tbyte ptr [NUMER+bp]
        fld     dword ptr [STACK_SIZE+12+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx                  ; dx = 1 if denormal extended divisor
        call    fprem1_common

        fstp    dword ptr [__fprem1_result]
        mov     dx,ds
        mov     ax, offset __fprem1_result

        fstp    st                      ; clean FP stack
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret
_frem1_chk      ENDP
; end _frem1_chk

;
; double _drem1_chk (double numer, double denom)
;
        public  _drem1_chk
_drem1_chk      PROC    FAR
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fld     qword ptr [STACK_SIZE+8+bp]
        fstp    tbyte ptr [NUMER+bp]
        fld     qword ptr [STACK_SIZE+16+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx                  ; dx = 1 if denormal extended divisor
        call    fprem1_common
        fstp    qword ptr [__fprem1_result]
        mov     dx,ds
        mov     ax, offset __fprem1_result
        fstp    st                      ; clean FP stack
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

_drem1_chk      ENDP
; end drem1_chk

;
; long double _lrem1_chk(long double number,long double denom)
;
        public  _lrem1_chk
_lrem1_chk      PROC    NEAR
        push    bp
        mov     bp, sp
        fld     tbyte ptr [16+bp]       ; assumes long double push
                                        ; is 10 bytes, 18 = 8 + long double push                                        ; size.
        fld     tbyte ptr [6+bp]
        call    _fprem1_chk
        fxch
        fstp    st
        pop     bp
        ret
_lrem1_chk      ENDP
**********************************************************************~

;
; FPREM: ST = remainder(ST, ST(1))
;
; Compiler version of the FPREM must preserve the arguments in the floating
; point stack.

        public  __fprem1_chk
        defpe   __fprem1_chk
        push    dx
        push    bp
        sub     sp, STACK_SIZE
        mov     bp, sp
        fstp    tbyte ptr [NUMER+bp]
        fstp    tbyte ptr [DENOM+bp]
        xor     dx, dx
; prem1_main_routine begin
        mov     ax,[DENOM+8+bp]         ; exponent
        test    ax,07fffh               ; check for denormal
        jz      denormal1
        call    fprem1_common
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

denormal1:
        fld     tbyte ptr [DENOM+bp]    ; load the denominator
        fld     tbyte ptr [NUMER+bp]    ; load the numerator
        mov     eax, dword ptr[DENOM+bp]        ; test for whole mantissa == 0
        or      eax, dword ptr[DENOM+4+bp]      ; test for whole mantissa == 0
        jz      remainder1_hardware_ok_l ; denominator is zero
        fxch
        fstp    tbyte ptr[bp + DENOM_SAVE]      ; save org denominator
        fld     tbyte ptr[bp + DENOM]
        fxch
        or      dx, 02h
;
; For this we need pc=80.  Also, mask exceptions so we don't take any
; denormal operand exceptions.  It is guaranteed that the descaling
; later on will take underflow, which is what the hardware would have done
; on a normal fprem.
;
        fnstcw  [PREV_CW+bp]            ; save caller's control word
        mov     ax, [PREV_CW+bp]
        or      ax, 0033fh              ; mask exceptions, pc=80
        mov     [PATCH_CW+bp], ax
        fldcw   [PATCH_CW+bp]           ; mask exceptions & pc=80

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

        mov     ax, [bp + NUMER + 8]    ; load numerator exponent
        and     ax, 7fffh               ; isolate numerator exponent
        cmp     ax, 7fbeh               ; compare Nexp to Maxexp-64
        ja      big_numer_rem1_de       ; jif big numerator

; So the numerator is not large scale both numerator and denominator

        or      dx, 1                   ; dx = 1, if denormal extended divisor
        fmul    qword ptr one_shl_64    ; make numerator not denormal
        fstp    tbyte ptr[bp + NUMER]
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[bp + DENOM]
        jmp     scaling_done1

; The numerator is large.  Scale only the denominator, which will not
; change the result which we know will be partial.  Set the scale flag
; to false.
big_numer_rem1_de:
        ; We must do this with pc=80 to avoid rounding to single/double.
        ; In this case we do not mask exceptions so that we will take
        ; denormal operand, as would the hardware.
        fnstcw  [PREV_CW+bp]            ; save caller's control word
        mov     ax, [PREV_CW+bp]
        or      ax, 00300h              ; pc=80
        mov     [PATCH_CW+bp], ax
        fldcw   [PATCH_CW+bp]           ; pc=80

        fstp    st                      ; Toss numerator
        fmul    qword ptr one_shl_64    ; make denominator not denormal
        fstp    tbyte ptr[bp + DENOM]

; Restore the control word which was fiddled to scale at 80-bit precision.
; Then call the common code.
scaling_done1:
        fldcw   [bp + PREV_CW]          ; restore callers control word
        call    fprem1_common
        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret

remainder1_hardware_ok_l:
        fprem                           ; and finally do a remainder

        CHECKSW

        add     sp, STACK_SIZE
        pop     bp
        pop     dx
        ret
__fprem1_chk     ENDP
; end _fprem1_chk

ifdef   DEBUG
        public  _fpinit
_fpinit PROC    FAR
        fninit
        ret
_fpinit ENDP
endif

_TEXT ENDS
       END
