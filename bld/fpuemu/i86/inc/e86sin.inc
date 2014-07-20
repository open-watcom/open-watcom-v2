
        xrefp   __FLDA          ; add
        xrefp   __FLDS          ; subtract
        xrefp   __FLDD          ; divide
        xrefp   __FLDM          ; multiply
        xrefp   __FLDAC         ; add
        xrefp   __fprem         ; remainder
        xrefp   __OddPoly       ; evaluate odd degree polynomial
        xrefp   __Poly          ; evaluate polynomial

        xrefp   __PIby4         ; pi/4
        xrefp   __SinPoly       ; polynomial for sin
        xrefp   __CosPoly       ; polynomial for cos

        modstart  e86sin

        xdefp   __sin           ; calc sin(fac1)
        xdefp   __cos           ; calc cos(fac1)
        xdefp   __tan           ; calc tan(fac1)

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
;;__One:
;;   dt   1.00000000000000000000  ; 1./1!

Degree  dw  0, 1, 2, 3, 4, 4, 5, 5, 6
;;
cosy    equ     -10
siny    equ     cosy-10
exponent_ equ    siny-2
iflag   equ     exponent_-2
index   equ     iflag-2

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
        push    BP                      ; save EBP
        push    DI                      ; save registers
        push    DX                      ; ...
        push    CX                      ; ...
        push    BX                      ; ...
        mov     DI,AX                   ; save pointer to operand
        push    word ptr 8[DI]          ; duplicate argument on stack
        push    word ptr 6[DI]          ; ...
        push    word ptr 4[DI]          ; ...
        push    word ptr 2[DI]          ; ...
        push    word ptr [DI]           ; ...
        mov     DI,SP                   ; get pointer to argument
        push    AX                      ; push addr of old argument
        push    DS                      ; ...
        push    SS                      ; set DS=SS
        pop     DS                      ; ...
        mov     BP,SP                   ; point to stack
        sub     SP,(10*2)+6             ; allocate temporaries
;;
;;        frexp( x, &exponent );
;;        if( exponent >= 32 ) {
;;            return( _matherr( TLOSS, func_name[flag], &x, &x, 0.0 ) );
;;        }
        sub     CX,CX                   ; zero ECX
        mov     CX,8[DI]                ; get exponent
        and     CX,7FFFh                ; ...
        sub     CX,3FFEh                ; ...
        cmp     CX,32                   ; if exponent too large
        _if     ge                      ; then
;               error
        _endif                          ; endif
        mov     exponent_[BP],CX         ; save exponent

;;        y = __fprem( x, PIby4, &i );    /* 24-nov-88 */
        push    word ptr __PIby4+8      ; pi/4
        push    word ptr __PIby4+6      ; ...
        push    word ptr __PIby4+4      ; ...
        push    word ptr __PIby4+2      ; ...
        push    word ptr __PIby4+0      ; ...
        mov     AX,DI                   ; point to operand
        mov     DX,SP                   ; point to pi/4
        call    __fprem                 ; calculate remainder
        add     SP,10                   ; remove pi/4 from stack

;;        if( y < 0.0 ) {               ; 03-may-90
;;              y += PIby4;
;;              --i;
;;        }
        test    byte ptr 9[DI],80h      ; if y < 0.0
        _if     ne                      ; then
          push  AX                      ; - save i
          mov   AX,DI                   ; - y = y + PIby4
          mov   DX,offset __PIby4 ; - ...
          mov   BX,DI                   ; - ...
          call  __FLDAC                 ; - ...
          pop   AX                      ; - restore i
          dec   AX                      ; - i = i - 1
        _endif                          ; endif

;;        i = (i + (flag & 2)) & 7;
        mov     BX,SI                   ; get flag
        mov     AH,BL                   ; save it
        and     BL,2                    ; ...
        add     AL,BL                   ; + i
        and     AL,7                    ; & 7
        mov     iflag[BP],AX            ; save i and flag

;;        if( i & 1 ) {                   /* if odd octant */
;;            y = PIby4 - y;
;;        }
        test    AL,1                    ; if odd octant
        _if     ne                      ; then
          xor   byte ptr 9[DI],80h      ; - negate y
          mov   AX,DI                   ; - y
          mov   DX,offset __PIby4 ; - pi/4
          mov   BX,DI                   ; - y
          call  __FLDAC                 ; - calc. y = (-y) + pi/4
        _endif                          ; endif

;;        frexp( y, &index );
        mov     BX,8[DI]                ; get exponent
        sub     BX,3FFEh                ; ...
;;/*      if( z < ldexp( 1.0, -26 ) ) { */
;;//      if( z < 1.49011611938476580e-008 ) {
;;        if( index < -26 ) {
;;            siny = y;
;;            cosy = 1.0;
;;        } else {
        cmp     BX,-32                  ; if exponent too small
        _if     l                       ; then
          mov   AX,3FFFh                ; - set cosy = 1.0
          mov   cosy+8[BP],AX           ; - ...
          mov   AX,8000h                ; - ...
          mov   cosy+6[BP],AX           ; - ...
          sub   AX,AX                   ; - ...
          mov   cosy+4[BP],AX           ; - ...
          mov   cosy+2[BP],AX           ; - ...
          mov   cosy+0[BP],AX           ; - ...
          jmp   __endif1
        _endif                          ; else
;;            if( index > 0 ) index = 0;
;;            index = - index;
;;            if( index > 8 ) index = 8;
;;            index = Degree[ index ];

          or    BX,BX                   ; - if index > 0
          _if   ns                      ; - then
            mov   BX,0                  ; - - set to 0
          _endif                        ; - endif
          neg   BX                      ; - make positive
          cmp   BX,8                    ; - if index > 8
          _if   g                       ; - then
            mov   BX,8                  ; - - set to 8
          _endif                        ; - endif
          shl   BX,1                    ; - times 2 for index
          mov   BX,Degree[BX]           ; - get degree
          mov   index[BP],BX            ; - save degree

;;            /* only calculate the necessary polynomials */
;;            if( ((i+1) & 2) || flag == 1 ) {
          _guess                        ; - guess
            mov   AX,iflag[BP]          ; - - get i and flag
            inc   AL                    ; - - + 1
            and   AL,2                  ; - - & 2
            _quif ne                    ; - - quit if ON
            cmp   AH,1                  ; - - or flag == 1
            _quif e                     ; - - ...
          _admit                        ; - admit
;;                cosy = _EvalPoly( y*y, &_cospoly[index], 8 - index );
            push  AX                    ; - - save (i+1) & 2
            mov   AX,DI                 ; - - calc. temp = y * y
            mov   DX,DI                 ; - - ...
            lea   BX,cosy[BP]           ; - - ...
            call  __FLDM                ; - - ...
            mov   BX,index[BP]          ; - - get index
            mov   CX,8                  ; - - calc. 8 - index
            sub   CX,BX                 ; - - ...
            shl   BX,1                  ; - - calc. index * 10
            shl   BX,1                  ; - - ...
            add   BX,index[BP]          ; - - ...
            shl   BX,1                  ; - - ...
            lea   DX,__CosPoly[BX]; - - point to start of polynomial
            mov   BX,CX                 ; - - get the degree
            lea   AX,cosy[BP]           ; - - point to y*y
            mov   CX,AX                 ; - - point to cosy for result
            call  __Poly                ; - - evaluate polynomial
;;            }
            pop   AX                    ; - - restore (i+1) & 2
          _endguess                     ; - endguess

;;            if( (((i+1) & 2) == 0) || flag == 1 ) {
          _guess                        ; - guess
            cmp   AL,0                  ; - - quit if (1+1) & 2 == 0
            _quif e                     ; - - ...
            cmp   AH,1                  ; - - or flag == 1
            _quif e                     ; - - ...
          _admit                        ; - admit
;;                siny = _OddPoly( y, &_sinpoly[index], 8 - index );
            mov   AX,DI                 ; - - point to y
            mov   BX,index[BP]          ; - - get index
            mov   CX,8                  ; - - calc. 8 - index
            sub   CX,BX                 ; - - ...
            shl   BX,1                  ; - - calc. index * 10
            shl   BX,1                  ; - - ...
            add   BX,index[BP]          ; - - ...
            shl   BX,1                  ; - - ...
            lea   DX,__SinPoly[BX]; - - point to start of polynomial
            mov   BX,CX                 ; - - get the degree
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
        lea     SI,cosy[BP]             ; assume octants 0,3,4,7
        mov     BX,DI                   ; ...
        mov     AX,iflag[BP]            ; get i and flag
        inc     AL                      ; + 1
        and     AL,2                    ; & 2
        _if     ne                      ; if octants 1,2,5,6
          xchg  BX,SI                   ; - xchg siny and cosy
;         lea   BX,cosy[BP]             ; - sinx = cosy
;       _else                           ; else octants 0,3,4,7
;         mov   BX,DI                   ; - sinx = siny
        _endif                          ; endif

;;        if( i & 4 )     sinx = - sinx;  /* octants 4,5,6,7 */
        mov     CX,iflag[BP]            ; get i and flag
        test    CL,4                    ; if octants 4,5,6,7
        _if     ne                      ; then
          xor   byte ptr 9[BX],80h      ; - flip the sign bit
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
          add   CL,2                    ; - i += 2;
;         mov   BL,CL                   ; - calc. (i+1) & 2
;         inc   BL                      ; - ...
;         and   BL,2                    ; - ...
;         _if   ne                      ; - if octants 1,2,5,6
;           mov   EBX,cosy[EBP]         ; - - load cosy
;           mov   ECX,cosy+4[EBP]       ; - - ...
;           mov   SI,cosy+8[EBP]        ; - - ...
;         _else                         ; - else
;           mov   EBX,[EDI]             ; - - load siny
;           mov   ECX,4[EDI]            ; - - ...
;           mov   SI,8[EDI]             ; - - ...
;         _endif                        ; - endif
;;            if( i & 4 )     cosx = - cosx;  /* octants 4,5,6,7 */
          and   CL,4                    ; - if octants 4,5,6,7
          _if   ne                      ; - then
            xor   byte ptr 9[SI],80h    ; - - flip the sign
          _endif                        ; - endif
;;            if( cosx == 0.0 ) {
;;                errno = ERANGE;
;;                if( sinx > 0.0 )  return( HUGE_VAL );
;;                return( - HUGE_VAL );
;;            }
          _guess                        ; - guess
            mov   AX,[SI]               ; - - quit if cosx != 0
            or    AX,2[SI]              ; - - ...
            or    AX,4[SI]              ; - - ...
            or    AX,6[SI]              ; - - ...
            _quif ne                    ; - - ...
            mov    AX,8[SI]             ; - - ...
            shl    AX,1                 ; - - ...
            _quif ne                    ; - - ...
            mov   [DI],AX               ; - - set result to infinity
            mov   2[DI],AX              ; - - ...
            mov   4[DI],AX              ; - - ...
            mov   AX,8000h              ; - - ...
            mov   6[DI],AX              ; - - ...
            mov   AX,8[BX]              ; - - ...
            or    AX,7FFFh              ; - - ...
            mov   8[DI],AX              ; - - ...
          _admit                        ; - admit
;;            sinx = sinx/cosx;         /* calculate value of tan function */
            mov   AX,BX                 ; - - ...
            mov   DX,SI                 ; - - ...
            mov   BX,DI                 ; - - ...
            call  __FLDD                ; - - tan(x) = sin(x) / cos(x)
            mov   BX,DI                 ; - - ...
          _endguess                     ; - endguess
;;        }
        _endif                          ; endif
;;        if( exponent >= 28 ) {
;;            return( _matherr( PLOSS, func_name[flag], &x, &x, sinx ) );
;;        }
;;        return( sinx );
        cmp     BX,DI                   ; if result not in x
        _if     ne                      ; then
          mov   SI,BX                   ; - copy result to x
          lodsw                         ; - ...
          mov   [DI],AX                 ; - ...
          lodsw                         ; - ...
          mov   2[DI],AX                ; - ...
          lodsw                         ; - ...
          mov   4[DI],AX                ; - ...
          lodsw                         ; - ...
          mov   6[DI],AX                ; - ...
          lodsw                         ; - ...
          mov   8[DI],AX                ; - ...
        _endif                          ; endif

        mov     SP,BP                   ; clean up stack
        pop     DS                      ; restore address of result
        pop     DI                      ; ...
        pop     word ptr [DI]           ; copy value into result
        pop     word ptr 2[DI]          ; ...
        pop     word ptr 4[DI]          ; ...
        pop     word ptr 6[DI]          ; ...
        pop     word ptr 8[DI]          ; ...
        pop     BX                      ; restore registers
        pop     CX                      ; ...
        pop     DX                      ; ...
        pop     DI                      ; ...
        pop     BP                      ; restore EBP
        ret                             ; return
__sincos endp
;;    }
;;
;;
;;void sin( long double *x )
;;    {
;;        return _sincos( x, 0 );
        defp    __sin
        push    SI                      ; save registers
        sub     SI,SI                   ; indicate sin
        call    __sincos                ; evaluate sin
        pop     SI                      ; restore ESI
        ret                             ; return
__sin   endp
;;    }
;;
;;
;;void cos( long double *x )
;;    {
;;        return _sincos( x, 2 );
        defp    __cos
        push    SI                      ; save registers
        mov     SI,2                    ; indicate cos
        call    __sincos                ; evaluate cos
        pop     SI                      ; restore ESI
        ret                             ; return
__cos   endp
;;    }
;;
;;
;;void tan( long double *x )
;;    {
;;        return _sincos( x, 1 );
        defp    __tan
        push    SI                      ; save registers
        mov     SI,1                    ; indicate tan
        call    __sincos                ; evaluate tan
        pop     SI                      ; restore ESI
        ret                             ; return
__tan   endp
;;    }
