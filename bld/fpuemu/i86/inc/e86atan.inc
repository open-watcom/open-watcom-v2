
        modstart  e86atan

        xref    __FLDA          ; add
        xref    __FLDC          ; compare
        xref    __FLDD          ; divide
        xref    __FLDM          ; multiply
        xref    __FLDS          ; subtract
        xref    __FLDAC         ; add
        xref    __FLDSC         ; subtract
        xref    __FLDMC         ; multiply
        xref    __OddPoly       ; evaluate polynomial

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
;;
;;extern  int             _sgn(double);   /* get sign of x */
;;extern  double          _OddPoly(double, double *, int);
;;
;;
;;static  double          AtanPoly[] = {
AtanPoly dt      0.0443895157187
        dt      -0.06483193510303
        dt       0.0767936869066
        dt      -0.0909037114191074    ; -0.09090371144275426
        dt       0.11111097898051048
        dt      -0.14285714102825545
        dt       0.1999999999872945
        dt      -0.3333333333332993
_COne   dt       1.0

_Pi     dt      3.141592653589793238
__PIby2 dt      1.570796326794896619230
__PIby6 dt      0.523598775598298873078
sqrt3   dt      1.7320508075688772936
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
;;      AX - pointer to Y
;;      DX - pointer to X
;;      BX - pointer to result
;;
        defp    __fpatan
        push    DI                      ; save registers
        push    SI                      ; ...
        push    CX                      ; ...
        mov     DI,AX                   ; save address of y
        mov     SI,DX                   ; save address of x
        mov     CX,8[DI]                ; get sgn(y)
        _guess                          ; guess: y == 0
          shl   CX,1                    ; - get rid of sign bit
          _quif ne                      ; - quit if y != 0
          test  byte ptr 9[SI],80h      ; - if x < 0.0
          _if   ne                      ; - then
            mov   word ptr 8[BX],4000h  ; - - return pi
            mov   word ptr 6[BX],0C90Fh ; - - ...
            mov   word ptr 4[BX],0DAA2h ; - - ...
            mov   word ptr 2[BX],02168h ; - - ...
            mov   word ptr  [BX],0C235h ; - - ...
          _else                         ; - else
            sub   AX,AX                 ; - - get 0
            mov   word ptr 8[BX],AX     ; - - return 0.0
            mov   word ptr 6[BX],AX     ; - - ...
            mov   word ptr 4[BX],AX     ; - - ...
            mov   word ptr 2[BX],AX     ; - - ...
            mov   word ptr  [BX],AX     ; - - ...
          _endif                        ; - endif
        _admit                          ; guess: x == 0
          mov   CX,8[SI]                ; - get sgn(x)
          shl   CX,1                    ; - quit if x != 0
          _quif ne                      ; - ...
          mov   AX,8[DI]                ; - get sign of y
          and   AX,8000h                ; - ...
          or    AX,3FFFh                ; - return (+/-) pi/2
          mov   word ptr 8[BX],AX       ; - ...
          mov   word ptr 6[BX],0C90Fh   ; - ...
          mov   word ptr 4[BX],0DAA2h   ; - ...
          mov   word ptr 2[BX],02168h   ; - ...
          mov   word ptr  [BX],0C235h   ; - ...
        _admit                          ; admit: calc. arctan( y / x )
          mov   CX,8[DI]                ; - save sign+exponent of y
          mov   DI,BX                   ; - save address for result
          call  __FLDD                  ; - calc. result = y/x
          mov   AX,DI                   ; - point to argument
          push  word ptr 8[DI]          ; - duplicate argument on stack
          push  word ptr 6[DI]          ; - ...
          push  word ptr 4[DI]          ; - ...
          push  word ptr 2[DI]          ; - ...
          push  word ptr [DI]           ; - ...
          mov   DI,SP                   ; - get pointer to argument
          push  AX                      ; - push addr of old argument
          push  DS                      ; - ...
          push  SS                      ; - set DS=SS
          pop   DS                      ; - ...
          mov   AX,DI                   ; - point to new operand
          call  __atan                  ; - calc. atan(y/x)
          mov   AX,8[DI]                ; - get sign+exponent of result
          lea   DX,_Pi                  ; - point to pi
          _guess                        ; - guess: y >= 0.0
            or    CX,CX                 ; - - quit if y < 0.0
            _quif s                     ; - - ...
            or    AX,AX                 ; - - if result < 0
            _if   s                     ; - - then
              mov   AX,DI               ; - - - calc. x += pi
              mov   BX,DI               ; - - - ...
              call  __FLDAC             ; - - - ...
            _endif                      ; - - endif
          _admit                        ; - admit: y < 0.0
            or    AX,AX                 ; - - if result > 0
            _if   g                     ; - - then
              mov   AX,DI               ; - - - calc. x -= pi
              mov   BX,DI               ; - - - ...
              call  __FLDSC             ; - - - ...
            _endif                      ; - - endif
          _endguess                     ; - endguess
          pop   DS                      ; - restore address of result
          pop   DI                      ; - ...
          pop   word ptr [DI]           ; - copy value into result
          pop   word ptr 2[DI]          ; - ...
          pop   word ptr 4[DI]          ; - ...
          pop   word ptr 6[DI]          ; - ...
          pop   word ptr 8[DI]          ; - ...
        _endguess                       ; endguess
        pop     CX                      ; restore registers
        pop     SI                      ; ...
        pop     DI                      ; ...
        ret                             ; return
__fpatan endp
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
        push    DI                      ; save registers
        push    SI                      ; ...
        push    DX                      ; ...
        push    CX                      ; ...
        push    BX                      ; ...
        mov     DI,AX                   ; save address of x

;;        add_piby2 = FALSE;
;;        add_piby6 = FALSE;
        mov     CH,0                    ; add_piby2 and add_piby6 = 0
;;        sgnx = _sgn( x );
        mov     CL,9[DI]                ; get sign bit
;;        x = fabs( x );
        and     byte ptr 9[DI],7fh      ; turn off sign bit

;;        if( x == 1.0 ) {        /* 06-dec-88 */
;;            x = Pi / 4;
;;        } else {
        mov     AX,8[DI]                ; get exponent
        mov     BX,6[DI]                ; get high part of fraction
        _guess                          ; guess: x == 1.0
          cmp   AX,3FFFh                ; - quit if not 1.0
          _quif ne                      ; - ...
          cmp   BX,8000h                ; - ...
          _quif ne                      ; - ...
          mov   DX,4[DI]                ; - check rest for 0
          or    DX,2[DI]                ; - ...
          or    DX,[DI]                 ; - ...
          _quif ne                      ; - ...
          mov   word ptr 8[DI],3FFEh    ; - return pi/4
          mov   word ptr 6[DI],0C90Fh   ; - ...
          mov   word ptr 4[DI],0DAA2h   ; - ...
          mov   word ptr 2[DI],02168h   ; - ...
          mov   word ptr  [DI],0C235h   ; - ...
          jmp   atan_exit               ; - exit and set sign   26-feb-91
        _endguess                       ; admit: not 1.0
        _guess                          ; ...

;;            if( x > 1.0 ) {
;;                x = 1.0 / x;
;;                add_piby2 = TRUE;
;;            }
          cmp   AX,3FFFh                ; - if > 1.0
          _if   ge                      ; - then
            mov   AX,3FFFh              ; - - push 1.0 onto stack
            push  AX                    ; - - ...
            mov   AX,8000h              ; - - ...
            push  AX                    ; - - ...
            sub   AX,AX                 ; - - ...
            push  AX                    ; - - ...
            push  AX                    ; - - ...
            push  AX                    ; - - ...
            mov   AX,SP                 ; - - point to 1.0
            mov   DX,DI                 ; - - x
            mov   BX,DI                 ; - - result goes into x
            call  __FLDD                ; - - calc. x = 1.0 / x
            add   SP,10                 ; - - remove 1.0 from stack
            mov   CH,2                  ; - - add_piby2 = TRUE
          _endif                        ; - endif

;;            if( x > tan15 ) {
;;                x = (x * sqrt3 - 1.0) / (x + sqrt3);
;;                add_piby6 = TRUE;
;;            }
          _guess                        ; - guess: x > tan15
            cmpconst DI,03FFDh,08930h,0A2F4h,0F66Ah,0B09Bh
            _quif be                    ; - - quit if x <= tan15
            sub   SP,10                 ; - - allocate temporary
            mov   AX,DI                 ; - - x
            mov   DX,offset sqrt3 ; - - sqrt(3)
            mov   BX,SP                 ; - - temp
            call  __FLDMC               ; - - calc. temp = x * sqrt(3)
            mov   AX,SP                 ; - - calc. temp = temp - 1.0
            mov   DX,offset _COne ; - - ...
            mov   BX,SP                 ; - - ...
            call  __FLDSC               ; - - ...
            mov   AX,DI                 ; - - calc. x = x + sqrt(3)
            mov   DX,offset sqrt3 ; - - ...
            mov   BX,DI                 ; - - ...
            call  __FLDAC               ; - - ...
            mov   AX,SP                 ; - - calc. x = temp / x
            mov   DX,DI                 ; - - ...
            mov   BX,DI                 ; - - ...
            call  __FLDD                ; - - ...
            add   SP,10                 ; - - remove temporary
            or    CH,1                  ; - - add_piby6 = TRUE
          _endguess                     ; - endguess


;;            x = _OddPoly( x, AtanPoly, 8 );
          mov   AX,DI                   ; - x
          mov   DX,offset AtanPoly ; - polynomial
          mov   BX,8                    ; - degree
          call  __OddPoly               ; - evaluate polynomial

;;            if( add_piby6 ) {
;;                x += _Piby6;
;;            }
          shr   CH,1                    ; - if add_piby6
          _if   c                       ; - then
            mov   AX,DI                 ; - - x
            mov   DX,offset __PIby6 ; - - pi/6
            mov   BX,DI                 ; - - result goes into x
            call  __FLDAC               ; - - calc. x = x + pi/6
          _endif                        ; - endif

;;            if( add_piby2 ) {
;;                x = _Piby2 - x;
;;            }
          shr   CH,1                    ; - if add_piby2
          _if   c                       ; - then
            xor   byte ptr 9[DI],80h    ; - - negate x
            mov   AX,DI                 ; - - x
            mov   DX,offset __PIby2 ; - - pi/2
            mov   BX,DI                 ; - - result goes into x
            call  __FLDAC               ; - - calc. x = (-x) + pi/2
          _endif                        ; - endif

;;        }
atan_exit:
;;        if( sgnx < 0 ) {
;;            x = -x;
;;        }
          cmp   CL,0                    ; - if sgnx < 0
          _if   s                       ; - then
            xor   byte ptr 9[DI],80h    ; - - flip sign bit
          _endif                        ; - endif
        _endguess                       ; endguess

;;        return( x );
;;    }
;;
        pop     BX                      ; restore registers
        pop     CX                      ; ...
        pop     DX                      ; ...
        pop     SI                      ; ...
        pop     DI                      ; ...
        ret                             ; return
__atan  endp

