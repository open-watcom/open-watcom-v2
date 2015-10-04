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


        modstart  386atan

        xrefp   __FLDAC         ; add constant from stack
        xrefp   __FLDD          ; divide
        xrefp   ___LDA          ; add
        xrefp   ___LDD          ; divide
        xrefp   ___LDM          ; multiply
        xrefp   __OddPoly       ; evaluate polynomial

        xdefp   __fpatan        ; calc atan2(y,x)

;;/*
;;        atan - compute arctangent
;;
;;        Uses the following identities
;;        *****************************
;;
;;        arctan(x) = - arctan( - x )
;;
;;        arctan(x) = pi/2 - arctan( 1/x )
;;
;;        arctan(x) = arctan( (x*sqrt(3)-1) / (x+sqrt(3)) ) + pi/6
;;*/
;;
;;#include <math.h>
;;#include "pi.h"
;;
;;#define FALSE           0
;;#define TRUE            1
;;
;;tan15 dt      0.2679491924311227
;;sqrt3:
;;      dt      1.7320508075688772936
;;
;;extern  int             _sgn(double);   /* get sign of x */
;;extern  double          _OddPoly(double, double *, int);
;;
;;
;;static  double          AtanPoly[] = {
        align   2
AtanPoly dt      0.0443895157187
        dt      -0.06483193510303
        dt       0.0767936869066
        dt      -0.0909037114191074    ; -0.09090371144275426
        dt       0.11111097898051048
        dt      -0.14285714102825545
        dt       0.1999999999872945
        dt      -0.3333333333332993
        dt       1.0
;;};


;;double atan2( double y, double x )  - compute arctan(y/x)
;;
;;NOTE: x,y cannot both be 0
;;
;;case 1: y = 0    if x < 0 return pi, otherwise return 0
;;
;;case 2: x = 0    if y < 0 return -pi/2, otherwise return pi/2
;;
;;otherwise: compute z = atan( y/x )
;;               if y >= 0 and z < 0 return z + pi
;;               if y <  0 and z > 0 return z - pi
;;               else return z
;;
;;double atan2( double y, double x )
;;/********************************/
;;    {
;;        register int    sgnx;   /* sgn(x) */
;;        register int    sgny;   /* sgn(y) */
;;
;;        sgny = _sgn( y );
;;        sgnx = _sgn( x );
;;        if( sgny == 0 ) {               /* case 1 */
;;            if( sgnx == 0 ) {
;;                x = _matherr( DOMAIN, "atan2", &y, &x, 0.0 );
;;            } else if( sgnx < 0 ) {
;;                x = Pi;
;;            } else {
;;                x = 0.0;
;;            }
;;        } else if( sgnx == 0 ) {        /* case 2 */
;;            if( sgny < 0 ) {
;;                x = -Piby2;
;;            } else {
;;                x = Piby2;
;;            }
;;        } else {
;;            x = atan( y / x );
;;            sgnx = _sgn( x );
;;            if( sgny >= 0 ) {
;;                if( sgnx < 0 ) {
;;                    x += Pi;
;;                }
;;            } else {
;;                if( sgnx > 0 ) {
;;                    x -= Pi;
;;                }
;;            }
;;        }
;;        return( x );
;;    }
;;
;;  input:
;;      EAX - pointer to Y
;;      EDX - pointer to X
;;      EBX - pointer to result
;;

        defp    __fpatan

        push    EDI                     ; save registers
        push    ESI                     ; ...
        push    ECX                     ; ...
        mov     EDI,EAX                 ; save address of y
        mov     CX,8[EDI]               ; get sgn(y)
        _guess                          ; guess: y == 0
          shl   CX,1                    ; - get rid of sign bit
          _quif ne                      ; - quit if y != 0
          test  byte ptr 9[EDX],80h     ; - if x < 0.0
          _if   ne                      ; - then
            mov   word ptr 8[EBX],PI_EXP; - - return pi
            mov   dword ptr 4[EBX],PI_HW; - - ...
            mov   dword ptr  [EBX],PI_LW; - - ...
          _else                         ; - else
            mov   word ptr 8[EBX],0     ; - - return 0.0
            mov   dword ptr 4[EBX],0    ; - - ...
            mov   dword ptr  [EBX],0    ; - - ...
          _endif                        ; - endif
        _admit                          ; guess: x == 0
          mov   CX,8[EDX]               ; - get sgn(x)
          shl   CX,1                    ; - quit if x != 0
          _quif ne                      ; - ...
          mov   AX,8[EDI]               ; - get sign of y
          and   AX,8000h                ; - ...
          or    AX,PIBY2_EXP            ; - return (+/-) pi/2
          mov   word ptr 8[EBX],AX      ; - ...
          mov   dword ptr 4[EBX],PI_HW  ; - ...
          mov   dword ptr  [EBX],PI_LW  ; - ...
        _admit                          ; admit: calc. arctan( y / x )
          mov   CH,9[EDI]               ; - save sign of y
          call  __FLDD                  ; - calc. result = y/x
          mov   EAX,EBX                 ; - point to argument
          call  __atan                  ; - calc. atan(y/x)
          mov   DL,9[EBX]               ; - get sign of result
          or    CH,CH                   ; - quit if y < 0.0
          _quif s                       ; - ...
          or    DL,DL                   ; - if result < 0
          _if   s                       ; - then
            mov   EAX,EBX               ; - - calc. x += pi
            push  PI_EXP                ; - - ...
            push  PI_HW                 ; - - ...
            push  PI_LW                 ; - - ...
            call  __FLDAC               ; - - ...
          _endif                        ; - endif
        _admit                          ; admit: y < 0.0
          or    DL,DL                   ; - if result > 0
          _if   g                       ; - then
            mov   EAX,EBX               ; - - calc. x -= pi
            push  8000h+PI_EXP          ; - - ...
            push  PI_HW                 ; - - ...
            push  PI_LW                 ; - - ...
            call  __FLDAC               ; - - calc x = x + (-pi)
          _endif                        ; - endif
        _endguess                       ; endguess
        pop     ECX                     ; restore registers
        pop     ESI                     ; ...
        pop     EDI                     ; ...
        ret                             ; return

        endproc __fpatan

;;
;;
;;void atan( long double *x )
;;/*************************/
;;    {
;;        register char   add_piby2;
;;        register char   add_piby6;
;;        register int    sgnx;
;;
        defp    __atan

        push    EDI                     ; save registers
        push    ESI                     ; ...
        push    ECX                     ; ...
        mov     EDI,EAX                 ; save address of x

;;        add_piby2 = FALSE;
;;        add_piby6 = FALSE;
        mov     CH,0                    ; add_piby2 and add_piby6 = 0
;;        sgnx = _sgn( x );
        mov     CL,9[EDI]               ; get sign bit
;;        x = fabs( x );
        and     byte ptr 9[EDI],7fh     ; turn off sign bit

;;        if( x == 1.0 ) {        /* 06-dec-88 */
;;            x = Pi / 4;
;;        } else {
        _guess                          ; guess: x == 1.0
          cmp   word ptr 8[EDI],ONE_EXP ; - quit if not 1.0
          _quif ne                      ; - ...
          cmp   dword ptr 4[EDI],ONE_HW ; - ...
          _quif ne                      ; - ...
          cmp   dword ptr [EDI],ONE_LW  ; - ...
          _quif ne                      ; - ...
          mov   dword ptr [EDI],PI_LW   ; - x = pi/4
          mov   dword ptr 4[EDI],PI_HW  ; - ...
          mov   AX,PIBY4_EXP            ; - ...
          and   CL,80H                  ; - isolate sign bit    26-feb-91
          or    AH,CL                   ; - set sign
          mov   8[EDI],AX               ; - ...
          pop   ECX                     ; - restore ECX
          pop   ESI                     ; - restore ESI
          pop   EDI                     ; - restore EDI
          ret                           ; - return
        _endguess                       ; admit: not 1.0
        _guess                          ; ...
        push    EDX                     ; ...
        push    EBX                     ; ...

;;            if( x > 1.0 ) {
;;                x = 1.0 / x;
;;                add_piby2 = TRUE;
;;            }
          cmp   word ptr 8[EDI],3FFFh   ; - if > 1.0
          _if   ge                      ; - then
            push  ECX                   ; - - save flags
            mov   SI,8[EDI]             ; - - load x
            mov   ECX,4[EDI]            ; - - ...
            mov   EBX,[EDI]             ; - - ...
            shl   ESI,16                ; - - shift exponent to top
            mov   SI,ONE_EXP            ; - - load 1.0
            mov   EDX,ONE_HW            ; - - ...
            mov   EAX,ONE_LW            ; - - ...
            call  ___LDD                ; - - calc 1.0 / x
            mov   [EDI],EAX             ; - - store result
            mov   4[EDI],EDX            ; - - ...
            mov   8[EDI],SI             ; - - ...
            pop   ECX                   ; - - restore flags
            mov   CH,2                  ; - - add_piby2 = TRUE
          _endif                        ; - endif

;;            if( x > tan15 ) {
;;                x = (x * sqrt3 - 1.0) / (x + sqrt3);
;;                add_piby6 = TRUE;
;;            }
          _guess                        ; - guess: x > tan15
            mov   AX,8[EDI]             ; - - quit if x <= tan15
            cmp   AX,TAN15_EXP          ; - - ...
            _if   e                     ; - - if exponent is same
              mov   EAX,4[EDI]          ; - - - compare fractions
              cmp   EAX,TAN15_HW        ; - - - ...
              _if   e                   ; - - - if equal
                mov   EAX,[EDI]         ; - - - - compare bottom part
                cmp   EAX,TAN15_LW      ; - - - - ...
              _endif                    ; - - - endif
            _endif                      ; - - endif
            _quif be                    ; - - quit if x <= tan15
            push  ECX                   ; - - save flags
            mov   EBX,SQRT3_LW          ; - - load sqrt(3)
            mov   ECX,SQRT3_HW          ; - - ...
            mov   ESI,SQRT3_EXP shl 16  ; - - ...
            mov   EAX,[EDI]             ; - - load x
            mov   EDX,4[EDI]            ; - - ...
            mov   SI,8[EDI]             ; - - ...
            call  ___LDA                ; - - calc. x + sqrt(3)
            push  EAX                   ; - - save result
            push  EDX                   ; - - ...
            push  ESI                   ; - - ...

            mov   EBX,SQRT3_LW          ; - - load sqrt(3)
            mov   ECX,SQRT3_HW          ; - - ...
            mov   ESI,SQRT3_EXP shl 16  ; - - ...
            mov   EAX,[EDI]             ; - - load x
            mov   EDX,4[EDI]            ; - - ...
            mov   SI,8[EDI]             ; - - ...
            call  ___LDM                ; - - calc. x * sqrt(3)
            sub   EBX,EBX               ; - - load -1.0
            mov   ECX,80000000h         ; - - ...
            rol   ESI,16                ; - - ...
            mov   SI,0BFFFh             ; - - ...
            rol   ESI,16                ; - - ...
            call  ___LDA                ; - - calc.  x * sqrt(3) - 1.0
            pop   EBX                   ; - - restore x + sqrt(3)
            shl   EBX,16                ; - - ...
            mov   BX,SI                 ; - - ...
            mov   ESI,EBX               ; - - ...
            pop   ECX                   ; - - ...
            pop   EBX                   ; - - ...
            call  ___LDD                ; - - calc. (x*sqrt(3)-1.0 / (x+sqrt(3))
            mov   [EDI],EAX             ; - - store result in x
            mov   4[EDI],EDX            ; - - ...
            mov   8[EDI],SI             ; - - ...
            pop   ECX                   ; - - restore flags
            or    CH,1                  ; - - add_piby6 = TRUE
          _endguess                     ; - endguess


;;            x = _OddPoly( x, AtanPoly, 8 );
          mov   EAX,EDI                 ; - x
          lea   EDX,AtanPoly            ; - polynomial
          mov   EBX,8                   ; - degree
          call  __OddPoly               ; - evaluate polynomial

;;            if( add_piby6 ) {
;;                x += Piby6;
;;            }
          shr   CH,1                    ; - if add_piby6
          _if   c                       ; - then
            mov   EAX,EDI               ; - - x
            push  PIBY6_EXP             ; - - push pi/6
            push  PIBY6_HW              ; - - ...
            push  PIBY6_LW              ; - - ...
            mov   EBX,EDI               ; - - result goes into x
            call  __FLDAC               ; - - calc. x = x + pi/6
          _endif                        ; - endif

;;            if( add_piby2 ) {
;;                x = Piby2 - x;
;;            }
          shr   CH,1                    ; - if add_piby2
          _if   c                       ; - then calc. x = pi/2 - x
            push  8000h+PIBY2_EXP       ; - - (-pi/2)
            push  PI_HW                 ; - - ...
            push  PI_LW                 ; - - ...
            mov   EAX,EDI               ; - - x
            mov   EBX,EDI               ; - - result goes into x
            call  __FLDAC               ; - - calc. x = x + (-pi/2)
            xor   byte ptr 9[EBX],80h   ; - - flip the sign
          _endif                        ; - endif

;;        }
;;        if( sgnx < 0 ) {
;;            x = -x;
;;        }
          cmp   CL,0                    ; - if sgnx < 0
          _if   s                       ; - then
            xor   byte ptr 9[EDI],80h   ; - - flip sign bit
          _endif                        ; - endif
        _endguess                       ; endguess

;;        return( x );
;;    }
;;
        pop     EBX                     ; restore registers
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     ESI                     ; ...
        pop     EDI                     ; ...
        ret                             ; return

        endproc __atan
