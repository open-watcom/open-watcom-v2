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


; static char sccs_id[] = "@(#)patch32.asm      1.12  12/21/94  14:53:51";
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

DENOM           EQU     0
NUMER           EQU     12
PREV_CW         EQU     28      ; 24 + 4 (return size)
PATCH_CW        EQU     32      ; 28 + 4 (return size)

DENOM_SAVE      EQU     32

MAIN_DENOM      EQU     4
MAIN_NUMER      EQU     16

SPILL_SIZE      EQU     12
MEM_OPERAND     EQU     8
STACK_SIZE      EQU     44
SPILL_MEM_OPERAND       EQU     20

ONESMASK        EQU     0e000000h

SINGLE_NAN      EQU     07f800000h
DOUBLE_NAN      EQU     07ff00000h

ILLEGAL_OPC     EQU     6

f_stsw  macro   where
        fstsw   where
endm

fdivr_st        MACRO   reg_index, reg_index_minus1
        fstp    tbyte ptr [esp+DENOM]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fstp    tbyte ptr [esp+NUMER]
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     tbyte ptr [esp+NUMER]
        fxch    st(reg_index)
        add     esp, STACK_SIZE
ENDM

fdivr_sti       MACRO   reg_index, reg_index_minus1
        fstp    tbyte ptr [esp+NUMER]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fstp    tbyte ptr [esp+DENOM]
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     tbyte ptr [esp+NUMER]
        add     esp, STACK_SIZE
ENDM

fdivrp_sti      MACRO   reg_index, reg_index_minus1
        fstp    tbyte ptr [esp+NUMER]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fstp    tbyte ptr [esp+DENOM]
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        add     esp, STACK_SIZE
ENDM

fdiv_st         MACRO   reg_index, reg_index_minus1
        fstp    tbyte ptr [esp+NUMER]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     st
        fstp    tbyte ptr [esp+DENOM]
        fstp    tbyte ptr [esp+DENOM_SAVE]      ; save original denom,
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     tbyte ptr [esp+DENOM_SAVE]
        fxch    st(reg_index)
        add     esp, STACK_SIZE
ENDM

fdiv_sti        MACRO   reg_index, reg_index_minus1
        fxch    st(reg_index)
        fstp    tbyte ptr [esp+NUMER]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     st
        fstp    tbyte ptr [esp+DENOM]
        fstp    tbyte ptr [esp+DENOM_SAVE]      ; save original denom,
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fld     tbyte ptr [esp+DENOM_SAVE]
        add     esp, STACK_SIZE
ENDM

fdivp_sti       MACRO   reg_index, reg_index_minus1
        fstp    tbyte ptr [esp+DENOM]
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        fstp    tbyte ptr [esp+NUMER]
        call    fdiv_main_routine
IF      reg_index_minus1 GE 1
        fxch    st(reg_index_minus1)
ENDIF
        add     esp, STACK_SIZE
ENDM

_TEXT  SEGMENT DWORD USE32 PUBLIC 'CODE'
_TEXT  ENDS

DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA   ENDS

CONST  SEGMENT DWORD USE32 PUBLIC 'DATA'
CONST  ENDS

BSS    SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS    ENDS

DGROUP  GROUP CONST, BSS, DATA


DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'

fdiv_risc_table DB      0, 1, 0, 0, 4, 0, 0, 7, 0, 0, 10, 0, 0, 13, 0, 0
fdiv_scale_1    DD      03f700000h              ;0.9375
fdiv_scale_2    DD      03f880000h              ;1.0625
one_shl_63      DD      05f000000h


dispatch_table DD       offset label0
        DD      offset label1
        DD      offset label2
        DD      offset label3
        DD      offset label4
        DD      offset label5
        DD      offset label6
        DD      offset label7
        DD      offset label8
        DD      offset label9
        DD      offset label10
        DD      offset label11
        DD      offset label12
        DD      offset label13
        DD      offset label14
        DD      offset label15
        DD      offset label16
        DD      offset label17
        DD      offset label18
        DD      offset label19
        DD      offset label20
        DD      offset label21
        DD      offset label22
        DD      offset label23
        DD      offset label24
        DD      offset label25
        DD      offset label26
        DD      offset label27
        DD      offset label28
        DD      offset label29
        DD      offset label30
        DD      offset label31
        DD      offset label32
        DD      offset label33
        DD      offset label34
        DD      offset label35
        DD      offset label36
        DD      offset label37
        DD      offset label38
        DD      offset label39
        DD      offset label40
        DD      offset label41
        DD      offset label42
        DD      offset label43
        DD      offset label44
        DD      offset label45
        DD      offset label46
        DD      offset label47
        DD      offset label48
        DD      offset label49
        DD      offset label50
        DD      offset label51
        DD      offset label52
        DD      offset label53
        DD      offset label54
        DD      offset label55
        DD      offset label56
        DD      offset label57
        DD      offset label58
        DD      offset label59
        DD      offset label60
        DD      offset label61
        DD      offset label62
        DD      offset label63

DATA   ENDS


_TEXT  SEGMENT   DWORD USE32 PUBLIC 'CODE'


        assume cs:_TEXT, ds:DGROUP, es:DGROUP, ss:nothing

;
;  PRELIMINARY VERSION for register-register divides.
;


                                        ; In this implementation the
                                        ; fdiv_main_routine is called,
                                        ; therefore all the stack frame
                                        ; locations are adjusted for the
                                        ; return pointer.

fdiv_main_routine PROC  NEAR

        fld     tbyte ptr [esp+MAIN_NUMER]      ; load the numerator
        fld     tbyte ptr [esp+MAIN_DENOM]      ; load the denominator
retry:

;  The following three lines test for denormals and zeros.
;  A denormal or zero has a 0 in the explicit digit to the left of the
;  binary point.  Since that bit is the high bit of the word, adding
;  it to itself will produce a carry if and only if the number is not
;  denormal or zero.
;
        mov     eax, [esp+MAIN_DENOM+4] ; get mantissa bits 32-64
        add     eax,eax                 ; shift the one's bit onto carry
        jnc     denormal                ; if no carry, we're denormal

;  The following three lines test the three bits after the four bit
;  pattern (1,4,7,a,d).  If these three bits are not all one, then
;  the denominator cannot expose the flaw.  This condition is tested by
;  inverting the bits and testing that all are equal to zero afterward.

        xor     eax, ONESMASK           ; invert the bits that must be ones
        test    eax, ONESMASK           ; and make sure they are all ones
        jz      scale_if_needed         ; if all are one scale numbers
        fdivp   st(1), st               ; use of hardware is OK.
        ret

;
;  Now we test the four bits for one of the five patterns.
;
scale_if_needed:
        shr     eax, 28                 ; keep first 4 bits after point
        cmp     byte ptr fdiv_risc_table[eax], 0        ; check for (1,4,7,a,d)
        jnz     divide_scaled           ; are in potential problem area
        fdivp   st(1), st               ; use of hardware is OK.
        ret

divide_scaled:
        mov     eax, [esp + MAIN_DENOM+8]       ; test denominator exponent
        and     eax, 07fffh             ; if pseudodenormal ensure that only
        jz      invalid_denom           ; invalid exception flag is set
        cmp     eax, 07fffh             ; if NaN or infinity  ensure that only
        je      invalid_denom           ; invalid exception flag is set
;
;  The following six lines turn off exceptions and set the
;  precision control to 80 bits.  The former is necessary to
;  force any traps to be taken at the divide instead of the scaling
;  code.  The latter is necessary in order to get full precision for
;  codes with incoming 32 and 64 bit precision settings.  If
;  it can be guaranteed that before reaching this point, the underflow
;  exception is masked and the precision control is at 80 bits, these
;  six lines can be omitted.
;
        fnstcw  [esp+PREV_CW]           ; save caller's control word
        mov     eax, [esp+PREV_CW]
        or      eax, 033fh              ; mask exceptions, pc=80
        and     eax, 0f3ffh             ; set rounding mode to nearest
        mov     [esp+PATCH_CW], eax
        fldcw   [esp+PATCH_CW]          ; mask exceptions & pc=80

;  The following lines check the numerator exponent before scaling.
;  This in order to prevent undeflow when scaling the numerator,
;  which will cause a denormal exception flag to be set when the
;  actual divide is preformed. This flag would not have been set
;  normally. If there is a risk of underflow, the scale factor is
;  17/16 instead of 15/16.
;
        mov     eax, [esp+MAIN_NUMER+8] ; test numerator exponent
        and     eax, 07fffh
        cmp     eax, 00001h
        je      small_numer

        fmul    fdiv_scale_1            ; scale denominator by 15/16
        fxch
        fmul    fdiv_scale_1            ; scale numerator by 15/16
        fxch

;
;  The next line restores the users control word.  If the incoming
;  control word had the underflow exception masked and precision
;  control set to 80 bits, this line can be omitted.
;

        fldcw   [esp+PREV_CW]           ; restore caller's control word
        fdivp   st(1), st               ; use of hardware is OK.
        ret

small_numer:
        fmul    fdiv_scale_2            ; scale denominator by 17/16
        fxch
        fmul    fdiv_scale_2            ; scale numerator by 17/16
        fxch

;
;  The next line restores the users control word.  If the incoming
;  control word had the underflow exception masked and precision
;  control set to 80 bits, this line can be omitted.
;

        fldcw   [esp+PREV_CW]           ; restore caller's control word
        fdivp   st(1), st               ; use of hardware is OK.
        ret

denormal:
        mov     eax, [esp+MAIN_DENOM]   ; test for whole mantissa == 0
        or      eax, [esp+MAIN_DENOM+4] ; test for whole mantissa == 0
        jnz     denormal_divide_scaled  ; denominator is not zero
invalid_denom:                          ; zero or invalid denominator
        fdivp   st(1), st               ; use of hardware is OK.
        ret

denormal_divide_scaled:
        mov     eax, [esp + MAIN_DENOM + 8]     ; get exponent
        and     eax, 07fffh             ; check for zero exponent
        jnz     invalid_denom           ;
;
;  The following six lines turn off exceptions and set the
;  precision control to 80 bits.  The former is necessary to
;  force any traps to be taken at the divide instead of the scaling
;  code.  The latter is necessary in order to get full precision for
;  codes with incoming 32 and 64 bit precision settings.  If
;  it can be guaranteed that before reaching this point, the underflow
;  exception is masked and the precision control is at 80 bits, these
;  five lines can be omitted.
;

        fnstcw  [esp+PREV_CW]           ; save caller's control word
        mov     eax, [esp+PREV_CW]
        or      eax, 033fh              ; mask exceptions, pc=80
        and     eax, 0f3ffh             ; set rounding mode to nearest
        mov     [esp+PATCH_CW], eax
        fldcw   [esp+PATCH_CW]          ; mask exceptions & pc=80

        mov     eax, [esp + MAIN_NUMER +8]      ; test numerator exponent
        and     eax, 07fffh             ; check for denormal numerator
        je      denormal_numer
        cmp     eax, 07fffh             ; NaN or infinity
        je      invalid_numer
        mov     eax, [esp + MAIN_NUMER + 4]     ; get bits 32..63 of mantissa
        add     eax, eax                ; shift the first bit into carry
        jnc     invalid_numer           ; if there is no carry, we have an
                                        ; invalid numer
        jmp     numer_ok

denormal_numer:
        mov     eax, [esp + MAIN_NUMER + 4]     ; get bits 32..63 of mantissa
        add     eax, eax                ; shift the first bit into carry
        jc      invalid_numer           ; if there is a carry, we have an
                                        ; invalid numer

numer_ok:
        fxch
        fstp    st                      ; pop numerator
        fld     st                      ; make copy of denominator
        fmul    dword ptr[one_shl_63]   ; make denominator not denormal
        fstp    tbyte ptr [esp+MAIN_DENOM]      ; save modified denominator
        fld     tbyte ptr [esp+MAIN_NUMER]      ; load numerator
        fxch                            ; restore proper order
        fwait

;  The next line restores the users control word.  If the incoming
;  control word had the underflow exception masked and precision
;  control set to 80 bits, this line can be omitted.
;

        fldcw   [esp+PREV_CW]           ; restore caller's control word
        jmp     retry                   ; start the whole thing over

invalid_numer:
;
;  The next line restores the users control word.  If the incoming
;  control word had the underflow exception masked and precision
;  control set to 80 bits, this line can be omitted.
;
        fldcw   [esp + PREV_CW]
        fdivp   st(1), st               ; use of hardware is OK.
        ret

fdiv_main_routine       ENDP

        public  __fdiv_fpr
        defpe   __fdiv_fpr

        sub     esp, STACK_SIZE
        jmp     dword ptr dispatch_table[eax*4]


label0:
        fdiv    st,st(0)                ; D8 F0         FDIV    ST,ST(0)
        add     esp, STACK_SIZE
        ret
label1:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label2:
        fdivr   st,st(0)                ; D8 F8         FDIVR   ST,ST(0)
        add     esp, STACK_SIZE
        ret
label3:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label4:
        fdiv    st(0),st                ; DC F8/D8 F0   FDIV    ST(0),ST
        add     esp, STACK_SIZE
        ret
label5:
        fdivp   st(0),st                ; DE F8         FDIVP   ST(0),ST
        add     esp, STACK_SIZE
        ret
label6:
        fdivr   st(0),st                ; DC F0/DE F0   FDIVR   ST(0),ST
        add     esp, STACK_SIZE
        ret
label7:
        fdivrp  st(0),st                ; DE F0         FDIVRP  ST(0),ST
        add     esp, STACK_SIZE
        ret
label8:
        fdiv_st 1, 0
        ret
label9:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label10:
        fdivr_st 1, 0
        ret
label11:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label12:
        fdiv_sti 1, 0
        ret
label13:
        fdivp_sti 1, 0
        ret
label14:
        fdivr_sti 1, 0
        ret
label15:
        fdivrp_sti 1, 0
        ret
label16:
        fdiv_st 2, 1
        ret
label17:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label18:
        fdivr_st 2, 1
        ret
label19:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label20:
        fdiv_sti 2, 1
        ret
label21:
        fdivp_sti 2, 1
        ret
label22:
        fdivr_sti 2, 1
        ret
label23:
        fdivrp_sti 2, 1
        ret
label24:
        fdiv_st 3, 2
        ret
label25:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label26:
        fdivr_st 3, 2
        ret
label27:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label28:
        fdiv_sti 3, 2
        ret
label29:
        fdivp_sti 3, 2
        ret
label30:
        fdivr_sti 3, 2
        ret
label31:
        fdivrp_sti 3, 2
        ret
label32:
        fdiv_st 4, 3
        ret
label33:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label34:
        fdivr_st 4, 3
        ret
label35:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label36:
        fdiv_sti 4, 3
        ret
label37:
        fdivp_sti 4, 3
        ret
label38:
        fdivr_sti 4, 3
        ret
label39:
        fdivrp_sti 4, 3
        ret
label40:
        fdiv_st 5, 4
        ret
label41:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label42:
        fdivr_st 5, 4
        ret
label43:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label44:
        fdiv_sti 5, 4
        ret
label45:
        fdivp_sti 5, 4
        ret
label46:
        fdivr_sti 5, 4
        ret
label47:
        fdivrp_sti 5, 4
        ret
label48:
        fdiv_st 6, 5
        ret
label49:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label50:
        fdivr_st 6, 5
        ret
label51:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label52:
        fdiv_sti 6, 5
        ret
label53:
        fdivp_sti 6, 5
        ret
label54:
        fdivr_sti 6, 5
        ret
label55:
        fdivrp_sti 6, 5
        ret
label56:
        fdiv_st 7, 6
        ret
label57:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label58:
        fdivr_st 7, 6
        ret
label59:
        add     esp, STACK_SIZE
        int     ILLEGAL_OPC
label60:
        fdiv_sti 7, 6
        ret
label61:
        fdivp_sti 7, 6
        ret
label62:
        fdivr_sti 7, 6
        ret
label63:
        fdivrp_sti 7, 6
        ret
__fdiv_fpr      ENDP


__fdivp_sti_st    PROC    NEAR
                                ; for calling from mem routines
        sub     esp, STACK_SIZE
        fdivp_sti 1, 0
        ret
__fdivp_sti_st    ENDP

__fdivrp_sti_st   PROC    NEAR
                                ; for calling from mem routines
        sub     esp, STACK_SIZE
        fdivrp_sti 1, 0
        ret
__fdivrp_sti_st   ENDP

        public  __fdiv_chk
        defpe __fdiv_chk
                                ; for calling from mem routines
        sub     esp, STACK_SIZE
        fdivrp_sti 1, 0
        ret
__fdiv_chk   ENDP

;
;  PRELIMINARY VERSIONS of the routines for register-memory
;  divide instructions
;

;;; FDIV_M32 - FDIV m32real FIX
;;
;;      Input : Value of the m32real in the top of STACK
;;
;;      Output: Result of FDIV in ST

        PUBLIC  __fdiv_m32
        defpe   __fdiv_m32

        push    eax                             ; save eax
        mov     eax, [esp + MEM_OPERAND]        ; check for
        and     eax, SINGLE_NAN                 ; NaN
        cmp     eax, SINGLE_NAN                 ;
        je      memory_divide_m32               ;

        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack                   ; is FP stack full?
        fld     dword ptr[esp + MEM_OPERAND]    ; load m32real in ST
        call    __fdivp_sti_st                    ; do actual divide
        pop     eax
        ret     4
spill_fpstack:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fld     dword ptr[esp + SPILL_MEM_OPERAND] ; load m32 real
        call    __fdivp_sti_st                    ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivrp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret     4
memory_divide_m32:
        fdiv    dword ptr[esp + MEM_OPERAND]    ; do actual divide
        pop     eax
        ret     4

__fdiv_m32        ENDP


;;; FDIV_M64 - FDIV m64real FIX
;;
;;      Input : Value of the m64real in the top of STACK
;;
;;      Output: Result of FDIV in ST

        PUBLIC  __fdiv_m64
        defpe   __fdiv_m64

        push    eax                             ; save eax
        mov     eax, [esp + MEM_OPERAND + 4]    ; check for
        and     eax, DOUBLE_NAN                 ; NaN
        cmp     eax, DOUBLE_NAN                 ;
        je      memory_divide_m64               ;

        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m64               ; is FP stack full?
        fld     qword ptr[esp + MEM_OPERAND]    ; load m64real in ST
        call    __fdivp_sti_st                    ; do actual divide
        pop     eax
        ret     8
spill_fpstack_m64:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp]                  ; save user's ST(1)
        fld     qword ptr[esp + SPILL_MEM_OPERAND] ; load m64real
        call    __fdivp_sti_st                    ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivrp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret     8

memory_divide_m64:
        fdiv    qword ptr[esp + MEM_OPERAND]    ; do actual divide
        pop     eax
        ret     8

__fdiv_m64        ENDP



;;; FDIVR_M32 - FDIVR m32real FIX
;;
;;      Input : Value of the m32real in the top of STACK
;;
;;      Output: Result of FDIVR in ST

        PUBLIC  __fdiv_m32r
        defpe   __fdiv_m32r
        push    eax                             ; save eax
        mov     eax, [esp + MEM_OPERAND]        ; check for
        and     eax, SINGLE_NAN                 ; NaN
        cmp     eax, SINGLE_NAN                 ;
        je      memory_divide_m32r              ;

        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m32r              ; is FP stack full?
        fld     dword ptr[esp + MEM_OPERAND]    ; load m32real in ST
        call    __fdivrp_sti_st                   ; do actual divide
        pop     eax
        ret     4
spill_fpstack_m32r:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fld     dword ptr[esp + SPILL_MEM_OPERAND] ; load m32 real
        call    __fdivrp_sti_st                   ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret     4
memory_divide_m32r:
        fdivr   dword ptr[esp + MEM_OPERAND]    ; do actual divide
        pop     eax
        ret     4

__fdiv_m32r     ENDP


;;; FDIVR_M64 - FDIVR m64real FIX
;;
;;      Input : Value of the m64real in the top of STACK
;;
;;      Output: Result of FDIVR in ST

        PUBLIC  __fdiv_m64r
        defpe   __fdiv_m64r
        push    eax                             ; save eax
        mov     eax, [esp + MEM_OPERAND + 4]    ; check for
        and     eax, DOUBLE_NAN                 ; NaN
        cmp     eax, DOUBLE_NAN                 ;
        je      memory_divide_m64r              ;

        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m64r              ; is FP stack full?
        fld     qword ptr[esp + MEM_OPERAND]    ; load m64real in ST
        call    __fdivrp_sti_st                   ; do actual divide
        pop     eax
        ret     8
spill_fpstack_m64r:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fld     qword ptr[esp + SPILL_MEM_OPERAND] ; load m64real
        call    __fdivrp_sti_st                   ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret     8
memory_divide_m64r:
        fdivr   qword ptr[esp + MEM_OPERAND]    ; do actual divide
        pop     eax
        ret     8


__fdiv_m64r       ENDP

comment ~******************************************************************
;;; FDIV_M16I - FDIV m16int FIX
;;
;;      Input : Value of the m16int in the top of STACK
;;
;;      Output: Result of FDIV in ST

        PUBLIC  FDIV_M16I
FDIV_M16I       PROC    NEAR
        push    eax                             ; save eax
        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m16i              ; is FP stack full?
        fild    word ptr[esp + MEM_OPERAND]     ; load m16int in ST
        call    __fdivp_sti_st                    ; do actual divide
        pop     eax
        ret
spill_fpstack_m16i:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fild    word ptr[esp + SPILL_MEM_OPERAND] ; load m16int
        call    __fdivp_sti_st                    ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivrp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret

FDIV_M16I       ENDP

;;; FDIV_M32I - FDIV m16int FIX
;;
;;      Input : Value of the m16int in the top of STACK
;;
;;      Output: Result of FDIV in ST

        PUBLIC  FDIV_M32I
FDIV_M32I       PROC    NEAR
        push    eax                             ; save eax
        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m32i              ; is FP stack full?
        fild    dword ptr[esp + MEM_OPERAND]    ; load m32int in ST
        call    __fdivp_sti_st                    ; do actual divide
        pop     eax
        ret
spill_fpstack_m32i:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fild    dword ptr[esp + SPILL_MEM_OPERAND] ; load m32int
        call    __fdivp_sti_st                    ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivrp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret


FDIV_M32I       ENDP


;;; FDIVR_M16I - FDIVR m16int FIX
;;
;;      Input : Value of the m16int in the top of STACK
;;
;;      Output: Result of FDIVR in ST

        PUBLIC  FDIVR_M16I
FDIVR_M16I      PROC    NEAR
        push    eax                             ; save eax
        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m16ir             ; is FP stack full?
        fild    word ptr[esp + MEM_OPERAND]     ; load m16int in ST
        call    __fdivrp_sti_st                   ; do actual divide
        pop     eax
        ret
spill_fpstack_m16ir:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fild    word ptr[esp + SPILL_MEM_OPERAND] ; load m16int
        call    __fdivrp_sti_st                   ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret


FDIVR_M16I      ENDP


;;; FDIVR_M32I - FDIVR m32int FIX
;;
;;      Input : Value of the m32int in the top of STACK
;;
;;      Output: Result of FDIVR in ST

        PUBLIC  FDIVR_M32I
FDIVR_M32I      PROC    NEAR
        push    eax                             ; save eax
        f_stsw  ax                              ; get status word
        and     eax, 3800h                      ; get top of stack
        je      spill_fpstack_m32ir             ; is FP stack full?
        fild    dword ptr[esp + MEM_OPERAND]    ; load m32int in ST
        call    __fdivrp_sti_st                   ; do actual divide
        pop     eax
        ret
spill_fpstack_m32ir:
        fxch
        sub     esp, SPILL_SIZE                 ; make temp space
        fstp    tbyte ptr[esp ]                 ; save user's ST(1)
        fild    dword ptr[esp + SPILL_MEM_OPERAND] ; load m32int
        call    __fdivrp_sti_st                   ; do actual divide
        fld     tbyte ptr[esp]                  ; restore user's ST(1)
                                                ;esp is adjusted by fdivp fn
        fxch
        add     esp, SPILL_SIZE
        pop     eax
        ret

FDIVR_M32I      ENDP
**********************************************************************~



_TEXT  ENDS

        end
