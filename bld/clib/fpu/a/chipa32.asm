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


; static char sccs_id[] = "@(#)fpatan32.asm     1.7  12/21/94  08:33:45";
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


_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_TEXT  ENDS

CONST   SEGMENT DWORD PUBLIC USE32 'DATA'
CONST   ENDS

CONST2  SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2  ENDS

DATA    SEGMENT DWORD PUBLIC USE32 'DATA'


Y               EQU     0
X               EQU     12
PREV_CW         EQU     24
PATCH_CW        EQU     28
SPILL           EQU     32
STACK_SIZE      EQU     36


pos_1   DD   00000000H
        DD   3ff00000H

neg_1   DD   00000000H
        DD   0bff00000H


dispatch_table  DD      offset label0
                DD      offset label1
                DD      offset label2
                DD      offset label3
                DD      offset label4
                DD      offset label5
                DD      offset label6
                DD      offset label7
;end dispatch table

pi      DB      35H
        DB      0c2H
        DD      0daa22168H
        DD      4000c90fH

pi_by_2 DB      35H
        DB      0c2H
        DD      0daa22168H
        DD      3fffc90fH

flt_sixteen DD  41800000H

one_by_sixteen  DD 3d800000H


B1      DW      0AAA8H
        DD      0AAAAAAAAH
        DD      0BFFDAAAAH

B2      DW      2D6EH
        DD      0CCCCCCCCH
        DD      3FFCCCCCH

B3      DW      4892H
        DD      249241F9H
        DD      0BFFC9249H

B4      DW      0C592H
        DD      3897CDECH
        DD      3FFBE38EH

B5      DW      5DDDH
        DD      0C17BC162H
        DD      0BFFBBA2DH

B6      DW      4854H
        DD      77C7C78EH
        DD      3FFB9C80H


atan_k_by_16    dd 000000000H, 000000000H, 000000000H, 000000000H
                dd 067EF4E37H, 0FFAADDB9H, 000003FFAH, 000000000H
                dd 0617B6E33H, 0FEADD4D5H, 000003FFBH, 000000000H
                dd 072D81135H, 0BDCBDA5EH, 000003FFCH, 000000000H
                dd 06406EB15H, 0FADBAFC9H, 000003FFCH, 000000000H
                dd 03F5E5E6AH, 09B13B9B8H, 000003FFDH, 000000000H
                dd 026F78474H, 0B7B0CA0FH, 000003FFDH, 000000000H
                dd 0611FE5B6H, 0D327761EH, 000003FFDH, 000000000H
                dd 00DDA7B45H, 0ED63382BH, 000003FFDH, 000000000H
                dd 0D9867E2AH, 0832BF4A6H, 000003FFEH, 000000000H
                dd 0F7F59F9BH, 08F005D5EH, 000003FFEH, 000000000H
                dd 071BDDA20H, 09A2F80E6H, 000003FFEH, 000000000H
                dd 034F70924H, 0A4BC7D19H, 000003FFEH, 000000000H
                dd 0B4D8C080H, 0AEAC4C38H, 000003FFEH, 000000000H
                dd 0C2319E74H, 0B8053E2BH, 000003FFEH, 000000000H
                dd 0AC526641H, 0C0CE85B8H, 000003FFEH, 000000000H
                dd 02168C235H, 0C90FDAA2H, 000003FFEH, 000000000H

DATA   ENDS

BSS    SEGMENT DWORD PUBLIC USE32 'BSS'
BSS    ENDS


EXTRN   __fdiv_fpr:NEAR

DGROUP  GROUP CONST,CONST2,DATA,BSS


_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
        ASSUME CS:_TEXT,DS:DGROUP,ES:DGROUP, SS:nothing
        public __fpatan_chk

        defpe   __fpatan_chk
        push    eax
        push    ecx
        push    edx
        sub     esp, STACK_SIZE
        fstp    tbyte ptr [esp+X]       ; save X
        fstp    tbyte ptr [esp+Y]       ; save Y

        mov     ecx, [esp+Y+4]
        add     ecx, ecx
        jnc     hw_fpatan               ; unnormals (explicit 1 missing)
        mov     eax, [esp+X+4]
        add     eax, eax
        jnc     hw_fpatan               ; unnormals (explicit 1 missing)
        mov     ecx, [esp+Y+8]          ; save high part of Y
        mov     eax, [esp+X+8]          ; save high part of Y
        and     ecx, 7fffh              ; Ey = exponent Y
        jz      hw_fpatan               ; Ey = 0
        and     eax, 7fffh              ; Ex = exponent X
        jz      hw_fpatan               ; Ex = 0
        cmp     ecx, 7fffh              ; check if Ey = 0x7fffh
        je      hw_fpatan
        cmp     eax, 7fffh              ; check if Ex = 0x7fffh
        je      hw_fpatan

        fld     tbyte ptr [esp+X]       ; reload X
        fabs                            ; |X| = u
        fld     tbyte ptr [esp+Y]       ; reload Y
        fabs                            ; |Y| = v

;  The following five lines turn off exceptions and set the
;  precision control to 80 bits.  The former is necessary to
;  force any traps to be taken at the divide instead of the scaling
;  code.  The latter is necessary in order to get full precision for
;  codes with incoming 32 and 64 bit precision settings.  If
;  it can be guaranteed that before reaching this point, the underflow
;  exception is masked and the precision control is at 80 bits, these
;  five lines can be omitted.
;
        fnstcw  [PREV_CW+esp]           ; save caller's control word
        mov     edx, [PREV_CW+esp]
        or      edx, 033fh              ; mask exceptions, pc=80
        and     edx, 0f3ffh
        mov     [PATCH_CW+esp], edx
        fldcw   [PATCH_CW+esp]          ; mask exceptions & pc=80


        xor     edx, edx                ; initialize sflag = 0
        fcom                            ; |Y| > |x|
        push    eax
        fstsw  ax
        sahf
        pop     eax
        jb      order_X_Y_ok
        fxch
        inc     edx                     ; sflag = 1
order_X_Y_ok:
        push    eax
        mov     eax, 0fh
        call    __fdiv_fpr                  ; v/u = z
        pop     eax
        fld     dword ptr flt_sixteen   ; 16.0
        fmul    st, st(1)               ; z*16.0
; Top of stack looks like k, z
        fistp   dword ptr [SPILL+esp]   ; store k as int
        mov     ecx, [SPILL+esp]
        shl     ecx, 4
        fild    dword ptr[SPILL+esp]
        fmul    dword ptr one_by_sixteen; 1.0/16.0
; Top of stack looks like g, z
        fld     st(1)                   ; duplicate g
        fsub    st, st(1)               ; z-g = r
        fxch
; Top of stack looks like g, r, z
        fmulp   st(2), st               ; g*z
; Top of stack looks like r, g*z
        fld     qword ptr pos_1         ; load 1.0
        faddp   st(2), st               ; 1+g*z
; Top of stack looks like r, 1+g*z
        push    eax
        mov     eax, 0fh
        call    __fdiv_fpr                  ; v/u = z
        pop     eax
        fld     st(0)                   ; duplicate s
        fmul    st,st(1)                ; t = s*s
; Top of stack looks like t, s

        fld     st(0)
        fmul    st, st(1)
; Top of stack looks like t2, t, s
        fld     st(0)
        fmul    st, st(1)
        fld     tbyte ptr B6
        fld     tbyte ptr B5
; Top of stack looks like B5, B6, t4, t2, t, s
        fxch
        fmul    st, st(2)
        fld     tbyte ptr B4
        fxch    st(2)
        fmul    st, st(3)
; Top of stack looks like B5t4, B6t4, B4, t4, t2, t, s
        fld     tbyte ptr B3
        fxch    st(2)
        fmul    st, st(5)
; Top of stack looks like B6t6, B5t4, B3, B4, t4, t2, t, s
        fxch    st(3)
        fmulp   st(4), st
        fld     tbyte ptr B2
; Top of stack looks like B2, B5t4, B3, B6t6, B4t4, t2, t, s
        fxch    st(3)
        faddp   st(4), st
        mov     eax, [esp+X+8]
        fld     tbyte ptr B1
        fxch
        shl     eax, 16
; Top of stack looks like B5t4, B1, B3, B2, even, t2, t, s
        fmul    st, st(6)
        fxch    st(2)
        add     eax, eax
        fmul    st, st(5)
; Top of stack looks like B3t2, B1, B5t5, B2, even, t2, t, s
        fxch    st(3)
        adc     edx, edx                ; |sflag|Sx|
        fmulp   st(5), st
        fxch    st(2)
        mov     eax, [Y+8+esp]          ; save high part of Y
        fmul    st, st(5)
; Top of stack looks like B3t3, B5t5, B1, even, B2t2, t, s
        fxch    st(2)
        shl     eax, 16
        fmulp   st(5), st
; Top of stack looks like  B5t5, B3t3, even, B2t2, B1t, s
        fxch    st(2)
        faddp   st(3), st
        add     eax, eax
        faddp   st(1), st
        adc     edx, edx                ; |sflag|Sx|Sy|
; Top of stack looks like  odd, even, B1t, s
        faddp   st(2), st
        faddp   st(1), st
        fmul    st,st(1)                ; s*(odd+even)
        faddp   st(1), st               ; poly

        fld     tbyte ptr atan_k_by_16[ecx]     ; arctan[k;16]
        faddp   st(1), st               ; w = poly + arctan(g)

        jmp     dword ptr dispatch_table[edx*4]

label0:
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label1:
        fchs
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label2:
        fld     tbyte ptr pi
        fsubrp  st(1), st               ; pi - w
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label3:
        fld     tbyte ptr pi
        fsubrp  st(1), st               ; pi - w
        fchs                            ; - (pi - w)
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label4:
        fld     tbyte ptr pi_by_2
        fsubrp  st(1), st               ; pi/2 - w
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label5:
        fld     tbyte ptr pi_by_2
        fsubrp  st(1), st               ; pi/2 - w
        fchs                            ; - (pi/2 - w)
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label6:
        fld     tbyte ptr pi_by_2
        faddp   st(1), st               ; pi/2 + w
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
label7:
        fld     tbyte ptr pi_by_2
        faddp   st(1), st               ; pi/2 + w
        fchs                            ; -(pi/2+w)
        fldcw   [esp+PREV_CW]
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret


hw_fpatan:
        fld     tbyte ptr [esp+Y]       ; reload Y
        fld     tbyte ptr [esp+X]       ; reload X
        fpatan
        add     esp, STACK_SIZE
        pop     edx
        pop     ecx
        pop     eax
        ret
__fpatan_chk       ENDP

_TEXT  ENDS
        END
