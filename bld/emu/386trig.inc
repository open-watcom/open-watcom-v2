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


;;#include <stdio.h>
;;#include <math.h>
;;#include <limits.h>
;;#include <errno.h>
;;
;;PIby4:
;;        dt      0.785398163397448309615
;;
;;extern  double  _EvalPoly( double, double *, int );
;;extern  double  _OddPoly( double, double *, int );
;;
;;
;;static double _sinpoly[] = {
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15),
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11),
;;         1.0 / (2.*3*4*5*6*7*8*9),
;;        -1.0 / (2.*3*4*5*6*7),
;;         1.0 / (2.*3*4*5),
;;        -1.0 / (2.*3),
;;         1.0
;;};
;;__sinpoly:
;;   dt   0.0000000000000028114572543455207631  ; 1./17!
;;   dt  -0.00000000000076471637318198164759  ; 1./15!
;;   dt   0.00000000016059043836821614599  ; 1./13!
;;   dt  -0.000000025052108385441718775  ; 1./11!
;;   dt   0.0000027557319223985890652  ; 1./9!
;;   dt  -0.00019841269841269841269  ; 1./7!
;;   dt   0.0083333333333333333333  ; 1./5!
;;   dt  -0.16666666666666666666  ; 1./3!
;;   dt   1.00000000000000000000  ; 1./1!
;;
;;static double _cospoly[] = {
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14),
;;         1.0 / (2.*3*4*5*6*7*8*9*10*11*12),
;;        -1.0 / (2.*3*4*5*6*7*8*9*10),
;;         1.0 / (2.*3*4*5*6*7*8),
;;        -1.0 / (2.*3*4*5*6),
;;         1.0 / (2.*3*4),
;;        -1.0 / (2.),
;;         1.0
;;};
;;__cospoly:
;;   dt   0.000000000000047794773323873852974  ; 1./16!
;;   dt  -0.000000000011470745597729724713  ; 1./14!
;;   dt   0.0000000020876756987868098979  ; 1./12!
;;   dt  -0.00000027557319223985890652  ; 1./10!
;;   dt   0.000024801587301587301587  ; 1./8!
;;   dt  -0.0013888888888888888888  ; 1./6!
;;   dt   0.041666666666666666666  ; 1./4!
;;   dt  -0.50000000000000000000  ; 1./2!
;;_One:
;;   dt   1.00000000000000000000  ; 1./1!

Degree  label   word
        dw  0, 1, 2, 3, 4, 4, 5, 5, 6
;;
cosy    equ     -12
exponent equ    cosy-4
iflag   equ     exponent-4
index   equ     iflag-4

;;
;;void _sincos( long double *x, int flag )
;;    {
;;        int i;
;;        long j;
;;        auto int exponent;
;;        auto int index;
;;        double y;
;;        double siny, cosy;
;;        double sinx, cosx;
;;        static char *func_name[] = { "sin", "tan", "cos" };


        defp    __sincos
        push    EBP                     ; save EBP
        push    EDI                     ; save registers
        push    EDX                     ; ...
        push    ECX                     ; ...
        push    EBX                     ; ...
        mov     EBP,ESP                 ; point to stack
        sub     ESP,12                  ; allocate temporaries
;;
;;        frexp( x, &exponent );
;;        if( exponent >= 32 ) {
;;            return( _matherr( TLOSS, func_name[flag], &x, &x, 0.0 ) );
;;        }
        mov     EDI,EAX                 ; save address of argument
        sub     ECX,ECX                 ; zero ECX
        mov     CX,8[EAX]               ; get exponent
        and     CX,7FFFh                ; ...
        sub     CX,3FFEh                ; ...
        cmp     CX,32                   ; if exponent too large
        _if     ge                      ; then
;               error
        _endif                          ; endif
        push    ECX                     ; save exponent

;;        y = __fprem( x, PIby4, &i );    /* 24-nov-88 */

        push    ESI                     ; save quadrant
        push    EDI                     ; save address of operand
        mov     SI,8[EAX]               ; get exponent of op1
        mov     DI,PIBY4_EXP            ; get pi/4
        mov     ECX,PI_HW               ; ...
        mov     EBX,PI_LW               ; ...
        mov     EDX,4[EAX]              ; get operand
        mov     EAX,[EAX]               ; ...
        call    ___fprem                ; calc remainder
        pop     ECX                     ; restore address of operand
        mov     [ECX],EAX               ; store the remainder
        mov     4[ECX],EDX              ; ...
        mov     8[ECX],SI               ; ...
        mov     EAX,EDI                 ; store quotient
        mov     EDI,ECX                 ; get address of operand
        pop     ESI                     ; restore quadrant

;;        if( y < 0.0 ) {               ; 03-may-90
;;              y += PIby4;
;;              --i;
;;        }
        test    byte ptr 9[EDI],80h     ; if y < 0.0
        _if     ne                      ; then
          push  EAX                     ; - save i
          mov   EBX,EDI                 ; - y = y + PIby4
          mov   EAX,EDI                 ; - ...
          push  PIBY4_EXP               ; - push pi/4
          push  PI_HW                   ; - ...
          push  PI_LW                   ; - ...
          call  __FLDAC                 ; - y = y + pi/4
          pop   EAX                     ; - restore i
          dec   EAX                     ; - i = i - 1
        _endif                          ; endif

;;        i = (i + (flag & 2)) & 7;
        mov     EBX,ESI                 ; get flag
        mov     AH,BL                   ; save it
        and     BL,2                    ; ...
        add     AL,BL                   ; + i
        and     AL,7                    ; & 7
        push    EAX                     ; save i and flag

;;        if( i & 1 ) {                   /* if odd octant */
;;            y = PIby4 - y;
;;        }
        test    AL,1                    ; if odd octant
        _if     ne                      ; then calc. y = pi/4 - y
          push  8000h+PIBY4_EXP         ; - (-pi/4)
          push  PI_HW                   ; - ...
          push  PI_LW                   ; - ...
          mov   EAX,EDI                 ; - y
          mov   EBX,EDI                 ; - y
          call  __FLDAC                 ; - calc. y = y + -pi/4
          xor   byte ptr 9[EBX],80h     ; - flip the sign bit
        _endif                          ; endif

;;        frexp( y, &index );
        sub     ECX,ECX                 ; zero whole register
        mov     CX,8[EDI]               ; get exponent
        sub     CX,3FFEh                ; ...
;;/*      if( z < ldexp( 1.0, -26 ) ) { */
;;//      if( z < 1.49011611938476580e-008 ) {
;;        if( index < -26 ) {
;;            siny = y;
;;            cosy = 1.0;
;;        } else {
        cmp     CX,-32                  ; if exponent too small
        _if     l                       ; then
          mov   dword ptr cosy[EBP],ONE_LW ; - set cosy = 1.0
          mov   dword ptr cosy+4[EBP],ONE_HW
          mov   word ptr cosy+8[EBP],ONE_EXP
          jmp   __endif1
        _endif                          ; else
;;            if( index > 0 ) index = 0;
;;            index = - index;
;;            if( index > 8 ) index = 8;
;;            index = Degree[ index ];

          or    CX,CX                   ; - if index > 0
          _if   ns                      ; - then
            mov   CX,0                  ; - - set to 0
          _endif                        ; - endif
          neg   CX                      ; - make positive
          cmp   CX,8                    ; - if index > 8
          _if   g                       ; - then
            mov   CX,8                  ; - - set to 8
          _endif                        ; - endif
          mov   CX,Degree[ECX*2]        ; - get degree
          push  ECX                     ; - save degree

;;            /* only calculate the necessary polynomials */
;;            if( ((i+1) & 2) || flag == 1 ) {
          _guess                        ; - guess
            mov   EAX,iflag[EBP]        ; - - get i and flag
            inc   AL                    ; - - + 1
            and   AL,2                  ; - - & 2
            _quif ne                    ; - - quit if ON
            cmp   AH,1                  ; - - or flag == 1
            _quif e                     ; - - ...
          _admit                        ; - admit
;;                cosy = _EvalPoly( y*y, &_cospoly[index], 8 - index );
            push  EAX                   ; - - save (i+1) & 2
            mov   EAX,[EDI]             ; - - load y
            mov   EDX,4[EDI]            ; - - ...
            mov   SI,8[EDI]             ; - - ...
            shl   ESI,16                ; - - again
            mov   SI,8[EDI]             ; - - ...
            mov   EBX,EAX               ; - - ...
            mov   ECX,EDX               ; - - ...
            call  ___LDM                ; - - calc. y*y

            mov   ECX,index[EBP]        ; - - get index
            mov   EBX,8                 ; - - calc. 8 - index
            sub   EBX,ECX               ; - - ...
            lea   ECX,[ECX+ECX*4]       ; - - index * 5
ifdef __WIN387__
            add   ecx,ecx               ; - - index *= 2
            add   ECX,OFFSET32 __cospoly
else
            lea   ECX,__cospoly[ECX*2]  ; - - point to start of polynomial
endif
            call  __poly                ; - - evaluate polynomial
            mov   cosy[EBP],EAX         ; - - save cosy
            mov   cosy+4[EBP],EDX       ; - - ...
            mov   cosy+8[EBP],SI        ; - - ...
;;            }
            pop   EAX                   ; - - restore (i+1) & 2
          _endguess                     ; - endguess

;;            if( (((i+1) & 2) == 0) || flag == 1 ) {
          _guess                        ; - guess
            cmp   AL,0                  ; - - quit if (1+1) & 2 == 0
            _quif e                     ; - - ...
            cmp   AH,1                  ; - - or flag == 1
            _quif e                     ; - - ...
          _admit                        ; - admit
;;                siny = _OddPoly( y, &_sinpoly[index], 8 - index );
            mov   EAX,EDI               ; - - point to y
            mov   ECX,index[EBP]        ; - - get index
            mov   EBX,8                 ; - - calc. 8 - index
            sub   EBX,ECX               ; - - ...
            lea   ECX,[ECX+ECX*4]       ; - - index * 5
            add   ECX,ECX               ; - - index * 2
            _mova EDX,__sinpoly
            add   EDX,ECX               ; - - point to start of poly.
            call  __OddPoly             ; - - evaluate polynomial
;;            }
          _endguess                     ; - endguess

;;        }
__endif1:; _endif                       ; endif
;;#if 0
;;        switch( i ) {
;;        case 0: sinx =   siny;  cosx =   cosy;  break;
;;        case 1: sinx =   cosy;  cosx =   siny;  break;
;;        case 2: sinx =   cosy;  cosx = - siny;  break;
;;        case 3: sinx =   siny;  cosx = - cosy;  break;
;;        case 4: sinx = - siny;  cosx = - cosy;  break;
;;        case 5: sinx = - cosy;  cosx = - siny;  break;
;;        case 6: sinx = - cosy;  cosx =   siny;  break;
;;        case 7: sinx = - siny;  cosx =   cosy;  break;
;;        }
;;#endif
;;        if( (i+1) & 2 ) {       /* if octants 1,2,5,6 */
;;            sinx = cosy;
;;        } else {                /* octants 0,3,4,7 */
;;            sinx = siny;
;;        }
        mov     EAX,iflag[EBP]          ; get i and flag
        inc     AL                      ; + 1
        and     AL,2                    ; & 2
        _if     ne                      ; if octants 1,2,5,6
          mov   EAX,cosy[EBP]           ; - load cosy
          mov   EDX,cosy+4[EBP]         ; - ...
          mov   SI,cosy+8[EBP]          ; - ...
        _else                           ; else octants 0,3,4,7
          mov   EAX,[EDI]               ; - load siny
          mov   EDX,4[EDI]              ; - ...
          mov   SI,8[EDI]               ; - ...
        _endif                          ; endif

;;        if( i & 4 )     sinx = - sinx;  /* octants 4,5,6,7 */
        mov     ECX,iflag[EBP]          ; get i and flag
        test    CL,4                    ; if octants 4,5,6,7
        _if     ne                      ; then
          xor   SI,8000h                ; - flip the sign
        _endif                          ; endif
;;        if( flag == 1 ) {               /* if "tan" */
        cmp     CH,1                    ; if "tan"
        _if     e                       ; then
;;
;;            /* cos is out of phase with sin by 2 octants */
;;
;;            i += 2;
;;            if( (i+1) & 2 ) {       /* if octants 1,2,5,6 */
;;                cosx = cosy;
;;            } else {                /* octants 0,3,4,7 */
;;                cosx = siny;
;;            }
          rol   ESI,16                  ; - shift exponent to top
          add   CL,2                    ; - i += 2;
          mov   BL,CL                   ; - calc. (i+1) & 2
          inc   BL                      ; - ...
          and   BL,2                    ; - ...
          _if   ne                      ; - if octants 1,2,5,6
            and   CL,4                  ; - - if octants 4,5,6,7
            mov   EBX,cosy[EBP]         ; - - load cosy
            mov   ECX,cosy+4[EBP]       ; - - ...
            mov   SI,cosy+8[EBP]        ; - - ...
          _else                         ; - else
            and   CL,4                  ; - - if octants 4,5,6,7
            mov   EBX,[EDI]             ; - - load siny
            mov   ECX,4[EDI]            ; - - ...
            mov   SI,8[EDI]             ; - - ...
          _endif                        ; - endif
;;            if( i & 4 )     cosx = - cosx;  /* octants 4,5,6,7 */
          _if   ne                      ; - if octants 4,5,6,7
            xor   ESI,00008000h         ; - - flip the sign
          _endif                        ; - endif
          rol   ESI,16                  ; - shift exponent back to bottom
;;            if( cosx == 0.0 ) {
;;                errno = ERANGE;
;;                if( sinx > 0.0 )  return( HUGE_VAL );
;;                return( - HUGE_VAL );
;;            }
          _guess                        ; - guess
            or    EBX,EBX               ; - - quit if cosx != 0
            _quif ne                    ; - - ...
            or    ECX,ECX               ; - - ...
            _quif ne                    ; - - ...
            test  ESI,7FFF0000h         ; - - ...
            _quif ne                    ; - - ...
            sub   EAX,EAX               ; - - set result to infinity
            mov   EDX,80000000h         ; - - ...
            or    SI,7FFFh              ; - - ...
          _admit                        ; - admit
;;            sinx = sinx/cosx;         /* calculate value of tan function */
            call  ___LDD                ; - - tan(x) = sin(x) / cos(x)
          _endguess                     ; - endguess
;;        }
        _endif                          ; endif
;;        if( exponent >= 28 ) {
;;            return( _matherr( PLOSS, func_name[flag], &x, &x, sinx ) );
;;        }
;;        return( sinx );
        mov     [EDI],EAX               ; store result
        mov     4[EDI],EDX              ; ...
        mov     8[EDI],SI               ; ...
        mov     ESP,EBP                 ; clean up stack
        pop     EBX                     ; restore registers
        pop     ECX                     ; ...
        pop     EDX                     ; ...
        pop     EDI                     ; ...
        pop     EBP                     ; restore EBP
        ret                             ; return
__sincos endp
;;    }
;;
;;
;;void sin( long double *x )
;;    {
;;        return _sincos( x, 0 );
        defp    __sin
        push    ESI                     ; save registers
        sub     ESI,ESI                 ; indicate sin
        call    __sincos                ; evaluate sin
        pop     ESI                     ; restore ESI
        ret                             ; return
__sin   endp
;;    }
;;
;;
;;void cos( long double *x )
;;    {
;;        return _sincos( x, 2 );
        defp    __cos
        push    ESI                     ; save registers
        mov     ESI,2                   ; indicate cos
        call    __sincos                ; evaluate cos
        pop     ESI                     ; restore ESI
        ret                             ; return
__cos   endp
;;    }
;;
;;
;;void tan( long double *x )
;;    {
;;        return _sincos( x, 1 );
        defp    __tan
        push    ESI                     ; save registers
        mov     ESI,1                   ; indicate tan
        call    __sincos                ; evaluate tan
        pop     ESI                     ; restore ESI
        ret                             ; return
__tan   endp
;;    }
