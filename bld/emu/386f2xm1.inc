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


        modstart  386f2xm1

        xref    __FLDAC         ; add constant from stack
        xref    ___LDA          ; add
        xref    __FLDD          ; divide
        xref    ___LDD          ; divide
        xref    ___LDM          ; multiply

        xdefp   __f2xm1         ; calc 2**x - 1


;;const0:
;;        dt      20.813771196523035
;;const1:
;;        dt      0.057761135831801924
;;const2:
;;        dt      7.213503410844819


        align   2
ExpConsts dt    1.0442737824274138403   ;/*   sqrt(sqrt(sqrt(sqrt(2))))  */
        dt      1.0905077326652576592   ;/*   sqrt(sqrt(sqrt(2)))        */
        dt      1.1892071150027210667   ;/*   sqrt(sqrt(2))              */
        dt      1.4142135623730950488   ;/*   sqrt(2)                    */

;;void __f2xm1( long double *x )
;;/****************************/
;;      -.5 <= x <= .5
;;{
;;    register int        sgnx;
;;    register int        exp;
;;    register int        exp2;
;;    register double     *poly;
;;    double              ipart;
;;    double              a;
;;    double              b;
;;    double              ee;
;;
        defp    __f2xm1
        push    EBP                     ; save EBP
        push    EDI                     ; save registers
        push    ESI                     ; ...
        push    EDX                     ; ...
        push    ECX                     ; ...
        push    EBX                     ; ...
        mov     EDI,EAX                 ; save pointer to operand

;;    if( fabs( x ) < 4.445e-16 ) {       /* if argument is too small */
;;        x = 1.0;
;;    } else {
;;        exp = 0;
        sub     ESI,ESI                 ; exp = 0
;;        exp2 = 0;
;;        sgnx = _sgn( x );
;;        x = fabs( x );
        mov     CX,8[EDI]               ; get exponent and sign
        and     byte ptr 9[EDI],7Fh     ; force number to be positive

;;        if( x != 0.0 ) {
        cmp     word ptr 8[EDI],0       ; if x == 0.0
        _if     e                       ; then
          cmp   dword ptr 4[EDI],0      ; ...
          _if   e                       ; ...
            cmp  dword ptr [EDI],0      ; ...
          _endif                        ; ...
        _endif                          ; ...
        _if     ne                      ; if x != 0.0
;;            if( sgnx < 0 ) {
;;                --exp;
;;                x = 1.0 - x;
;;            }
          or    CX,CX                   ; - if number was negative
          _if   s                       ; - then calc. x = 1.0 - x
            dec   ESI                   ; - - exp = -1
            push  8000h+ONE_EXP         ; - - push -1.0
            push  ONE_HW                ; - - ...
            push  ONE_LW                ; - - ...
            mov   EAX,EDI               ; - - ...
            mov   EBX,EDI               ; - - ...
            call  __FLDAC               ; - - calc. x = x + -1.0
            xor   byte ptr 9[EBX],80h   ; - - flip sign bit
          _endif                        ; - endif

;;            x = modf( ldexp( x, 4 ), &ipart );
;;            if( x != 0.0 ) {
;;                x = ldexp( x, -4 );
;;            }
;;            exp2 = ipart;
          mov   BX,8[EDI]               ; - get exponent
          add   BX,4                    ; - multiply by 16
          sub   ECX,ECX                 ; - set ipart to 0
          mov   EDX,4[EDI]              ; - get fraction
          mov   EAX,[EDI]               ; - ...
          _loop                         ; - loop
            cmp   BX,3FFFh              ; - - quit if done
            _quif b                     ; - - ...
            _shl  EAX,1                 ; - - shift left one
            _rcl  EDX,1                 ; - - ...
            _rcl  ECX,1                 ; - - into integer part
            dec   BX                    ; - - decrement exponent
          _endloop                      ; - endloop
          or    EDX,EDX                 ; - check fraction
          _if   e                       ; - if zero
            or  EAX,EAX                 ; - - check other part
          _endif                        ; - endif
          _if   e                       ; - if fraction is zero
            sub   EBX,EBX               ; - - zero exponent
          _else                         ; - else
            _loop                       ; - - loop (normalize fraction)
              or  EDX,EDX               ; - - - check top bit
              _quif s                   ; - - - quit if normalized
              _shl EAX,1                ; - - - shift left
              _rcl EDX,1                ; - - - ...
              dec BX                    ; - - - decrement exponent
            _endloop                    ; - - endloop
            sub   BX,4                  ; - - divide by 16
          _endif                        ; - endif
          mov   [EDI],EAX               ; - store fraction
          mov   4[EDI],EDX              ; - ...
          mov   8[EDI],BX               ; - ...
        _else                           ; else
          sub   ECX,ECX                 ; - exp2 = 0
        _endif                          ; endif
;;        }
        push    ESI                     ; save exp
        push    ECX                     ; save exp2

;;        ee = x * x;
        mov     EAX,[EDI]               ; load x
        mov     EDX,4[EDI]              ; ...
        mov     SI,8[EDI]               ; ...
        shl     ESI,16                  ; load x again
        mov     SI,8[EDI]               ; ...
        mov     EBX,EAX                 ; ...
        mov     ECX,EDX                 ; ...
        call    ___LDM                  ; calc. ee = x * x
        push    EAX                     ; save ee
        push    EDX                     ; ...
        push    ESI                     ; ...
;;        a = ee + const0;
        mov     ECX,0A6829A79h          ; load const0
        mov     EBX,01BF21F8Ch          ; ...
        and     ESI,0000FFFFh           ; ...
        or      ESI,40030000h           ; ...
        call    ___LDA                  ; calc. a = ee + const0
        mov     EBP,ESP                 ; get access to stack
        xchg    EAX,8[EBP]              ; save a, load ee
        xchg    EDX,4[EBP]              ; ...
        xchg    ESI,0[EBP]              ; ...

;;        b = (ee * const1 + const2) * x;
        mov     ECX,0EC96F0D6h          ; load const1
        mov     EBX,009D7BFDBh          ; ...
        and     ESI,0000FFFFh           ; ...
        or      ESI,3FFA0000h           ; ...
        call    ___LDM                  ; calc. ee * const1
        mov     ECX,0E6D5051Ah          ; load const2
        mov     EBX,0E536E187h          ; ...
        and     ESI,0000FFFFh           ; ...
        or      ESI,40010000h           ; ...
        call    ___LDA                  ; calc. (ee * const1 + const2)
        mov     EBX,[EDI]               ; load x
        mov     ECX,4[EDI]              ; ...
        rol     ESI,16                  ; ...
        mov     SI,8[EDI]               ; ...
        rol     ESI,16                  ; ...
        call    ___LDM                  ; calc. b = (ee * const1 + const2) * x;
        push    EAX                     ; save b
        push    EDX                     ; ...
        push    ESI                     ; ...
;;        x = (a + b) / (a - b);
        xor     ESI,00008000h           ; negate b
        rol     ESI,16                  ; load a
        mov     SI,0[EBP]               ; ...
        rol     ESI,16                  ; ...
        mov     EBX,8[EBP]              ; ...
        mov     ECX,4[EBP]              ; ...
        call    ___LDA                  ; calc (a - b)
        xchg    EAX,-4[EBP]             ; save (a-b) and load b
        xchg    EDX,-8[EBP]             ; ...
        xchg    ESI,-12[EBP]            ; ...
        rol     ESI,16                  ; load a
        mov     SI,0[EBP]               ; ...
        rol     ESI,16                  ; ...
        mov     EBX,8[EBP]              ; ...
        mov     ECX,4[EBP]              ; ...
        call    ___LDA                  ; calc (a + b)
        rol     ESI,16                  ; load (a-b)
        mov     SI,-12[EBP]             ; ...
        rol     ESI,16                  ; ...
        pop     ECX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
        call    ___LDD                  ; calc. (a+b) / (a-b)
        mov     [EDI],EAX               ; save it in x
        mov     4[EDI],EDX              ; ...
        mov     8[EDI],SI               ; ...
        add     ESP,12                  ; clean up stack
        pop     ECX                     ; restore exp2

;;        poly = ExpConsts;
        _mova   ESI,ExpConsts
;;        while( exp2 != 0 ) {
        _loop                           ; loop
          or    ECX,ECX                 ; - quif exp2 == 0
          _quif e                       ; - ...
;;            if( exp2 & 1 ) {
          shr   ECX,1                   ; - get next bit
          _if   c                       ; - if on
;;                x *= *poly;
            push  ECX                   ; - - save ECX
            push  ESI                   ; - - save addr of poly
            mov   AX,8[EDI]             ; - - load x
            mov   ECX,4[EDI]            ; - - ...
            mov   EBX,[EDI]             ; - - ...
            shl   EAX,16                ; - - shift exponent to top
            mov   AX,CS:8[ESI]          ; - - load *poly
            mov   EDX,CS:4[ESI]         ; - - ...
            mov   ESI,CS:[ESI]          ; - - ...
            xchg  EAX,ESI               ; - - ...
            call  ___LDM                ; - - calc x * *poly
            mov   [EDI],EAX             ; - - store result
            mov   4[EDI],EDX            ; - - ...
            mov   8[EDI],SI             ; - - ...
            pop   ESI                   ; - - restore addr of poly
            pop   ECX                   ; - - restore ECX
;;            }
          _endif                        ; - endif
;;            ++poly;
          lea   ESI,10[ESI]             ; - ++poly
;;            exp2 = exp2 >> 1;
;;        }
        _endloop                        ; endloop
;;        x = ldexp( x, exp );
        pop     ESI                     ; restore exp
        add     word ptr 8[EDI],SI
;;        x = x - 1.0;
        mov     EAX,EDI                 ; point to x
        push    8000h+ONE_EXP           ; push -1.0
        push    ONE_HW                  ; ...
        push    ONE_LW                  ; ...
        mov     EBX,EDI                 ; result in x
        call    __FLDAC                 ; calc. x = x + -1.0
;;    }
;;    return( x );
;;}

        pop     EBX                     ; restore registers
        pop     ECX                     ; ...
        pop     EDX                     ; ...
        pop     ESI                     ; ...
        pop     EDI                     ; ...
        pop     EBP                     ; ...
        ret                             ; return
__f2xm1 endp

