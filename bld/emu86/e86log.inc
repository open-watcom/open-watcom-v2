
        modstart  e86log

        xref    __FLDA          ; add
        xref    __FLDS          ; subtract
        xref    __FLDD          ; divide
        xref    __FLDM          ; multiply
        xref    __FLDAC         ; add
        xref    __FLDSC         ; subtract
        xref    __FLDMC         ; multiply
        xref    __I4LD          ; convert long to long double
        xref    __EvalPoly      ; evaluate polynomial
        xref    __Poly          ; poly

        xdefp   __log           ; calc log(fac1)
        xdefp   __fyl2x         ; calc. log2([ax]) * [dx]
        xdefp   __fyl2xp1       ; calc. log2([ax]+1.0) * [dx]

one_over_loge2  dt      1.4426950408889634075
OneHalf         dt      0.5
logconst1       dw      0,0,0,0b180h,3ffeh      ;  355./512.
logconst2       dw      435ch,0865h,82e3h,0de80h,0bff2h

APoly   dt  -0.78956112887491257267
        dt   0.16383943563021534222e+2
        dt  -0.64124943423745581147e+2

BPoly   dt   1.0
        dt  -0.35667977739034646171e+2
        dt   0.31203222091924532844e+3
        dt  -0.76949932108494879777e+3


        defp    __fyl2xp1
        push    DX                      ; save registers
        push    BX                      ; ...
        push    DI                      ; ...
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
        mov     AX,DI                   ; point to pushed operand
        mov     DX,offset _TEXT:__One   ; point to 1.0
        mov     BX,DI                   ; put answer back in operand
        call    __FLDAC                 ; add 1.0 to x
        pop     DS                      ; restore address of result
        pop     DI                      ; ...
        pop     word ptr [DI]           ; copy value into result
        pop     word ptr 2[DI]          ; ...
        pop     word ptr 4[DI]          ; ...
        pop     word ptr 6[DI]          ; ...
        pop     word ptr 8[DI]          ; ...
        mov     AX,DI                   ; restore address of operand
        pop     DI                      ; restore DI
        pop     BX                      ; restore BX
        pop     DX                      ; restore DX
;
;       fall into __fyl2x routine
;
__fyl2xp1 endp


        defp    __fyl2x
        push    DX                      ; save registers
        push    BX                      ; ...
        push    DI                      ; ...
        mov     DI,DX                   ; save pointer to operand
        push    word ptr 8[DI]          ; duplicate argument on stack
        push    word ptr 6[DI]          ; ...
        push    word ptr 4[DI]          ; ...
        push    word ptr 2[DI]          ; ...
        push    word ptr [DI]           ; ...
        mov     DI,SP                   ; get pointer to argument
        push    DX                      ; push addr of old argument
        push    DS                      ; ...
        mov     DX,DI                   ; point to pushed operand
        mov     DI,AX                   ; save pointer to operand
        push    word ptr 8[DI]          ; duplicate argument on stack
        push    word ptr 6[DI]          ; ...
        push    word ptr 4[DI]          ; ...
        push    word ptr 2[DI]          ; ...
        push    word ptr [DI]           ; ...
        push    SS                      ; set DS=SS
        pop     DS                      ; ...
        mov     AX,SP                   ; point to pushed operand
        call    __log                   ; calc. ln([ax])
        mov     AX,SP                   ; get address of ln([ax])
        mov     BX,DX                   ; point to place for result
        push    DX                      ; save address of result
        call    __FLDM                  ; multiply
        pop     AX                      ; restore address of result
        mov     DX,offset _TEXT:one_over_loge2  ; point to 1/loge(2)
        mov     BX,AX                   ; address for result
        call    __FLDMC                 ; do the multiply
        add     SP,10                   ; remove temp from stack
        pop     DS                      ; restore address of result
        pop     DI                      ; ...
        pop     word ptr [DI]           ; copy value into result
        pop     word ptr 2[DI]          ; ...
        pop     word ptr 4[DI]          ; ...
        pop     word ptr 6[DI]          ; ...
        pop     word ptr 8[DI]          ; ...
        pop     DI                      ; restore DI
        pop     BX                      ; restore BX
        pop     DX                      ; restore DX
        ret                             ; return
__fyl2x endp



;;void __log( long double *x )
;;/**************************/
;;{
;;        int     exp;
;;        double  z;

;;    if( x <= 0.0 ) {
;;        x = _matherr( x == 0.0 ? SING : DOMAIN, "log", &x, &x, -HUGE_VAL );
;;    } else {

        defp    __log
        push    BP                      ; save registers
        push    DI                      ; ...
        push    SI                      ; ...
        push    DX                      ; ...
        push    CX                      ; ...
        push    BX                      ; ...
        mov     DI,AX                   ; save address of argument
        sub     CX,CX                   ; zero ECX
        mov     CX,8[DI]                ; get exponent
        _guess                          ; guess: x > 0.0
          or    CX,CX                   ; - quit if negative
          _quif s                       ; - ...
          ; check for 0
        _admit                          ; admit: x <= 0.0
          ;                             ; - error
        _endguess                       ; endguess

;;        x = frexp( x, &exp );

        mov     DX,3FFEh                ; set exponent
        sub     CX,3FFEh                ; ...
        push    CX                      ; save exponent
        mov     8[DI],DX                ; set exponent in the fraction
        sub     SP,10*3                 ; allocate temporaries
        mov     BP,SP                   ; get access to stack
        add     BP,10                   ; point to z

;;        z = x - .5;
        mov     AX,DI                   ; calc. z = x - .5
        mov     DX,offset _TEXT:OneHalf ; .5
        mov     BX,BP                   ; z
        call    __FLDSC                 ; ...

;;        if( x > sqrt_of_half ) {
        cmpconst DI,3FFEh,0B504h,0F333h,0F9DEh,06484h
        _if     a                               ; if x > sqrt(.5)
;;            z -= .5;
;;        } else {
;;            x = z;
;;            --exp;
;;        }
          mov   AX,BP                   ; - calc. z = z - .5
          mov   DX,offset _TEXT:OneHalf ; - ...
          mov   BX,BP                   ; - ...
          call  __FLDSC                 ; - ...
          mov   AX,DI                   ; - point to x
        _else                           ; else
          mov   AX,BP                   ; - point to z (set x = z)
          dec   word ptr 20[BP]         ; - decrement exponent
        _endif                          ; endif
;;        z = z / (x * .5 + .5);
                                        ; calc. temp = x * .5
        mov     DX,offset _TEXT:OneHalf ; ...
        mov     BX,SP                   ; ...
        call    __FLDMC                 ; ...
        mov     AX,SP                   ; calc. temp = temp + .5
        mov     DX,offset _TEXT:OneHalf ; ...
        mov     BX,SP                   ; ...
        call    __FLDAC                 ; ...
        mov     AX,BP                   ; calc. z = z / temp
        mov     DX,SP                   ; ...
        mov     BX,BP                   ; ...
        call    __FLDD                  ; ...

;;        x = z * z;
        mov     AX,BP                   ; calc. x = z * z
        mov     DX,BP                   ; ...
        mov     BX,DI                   ; ...
        call    __FLDM                  ; ...

;;        x = z + z * x * _EvalPoly( x, APoly, 2 ) / _EvalPoly( x, BPoly, 3 );

        mov     AX,DI                   ; calc. _EvalPoly(x,BPoly,3)
        mov     DX,offset _TEXT:BPoly   ; ...
        mov     BX,3                    ;
        mov     CX,SP                   ; temp1
        call    __Poly                  ; evaluate polynomial
        mov     AX,DI                   ; calc. _EvalPoly(x,APoly,2)
        mov     DX,offset _TEXT:APoly   ; ...
        mov     BX,2                    ;
        lea     CX,10[BP]               ; temp2
        call    __Poly                  ; evaluate polynomial
        lea     AX,10[BP]               ; temp2
        mov     DX,SP                   ; temp1
        mov     BX,SP                   ; calc. temp1 = APoly / BPoly
        call    __FLDD                  ; ...
        mov     AX,DI                   ; calc. temp1 = x * APoly/BPoly
        mov     DX,SP                   ; ...
        mov     BX,SP                   ; ...
        call    __FLDM                  ; calc. x * Apoly / BPoly
        mov     AX,BP                   ; calc. temp1= z * x*APoly/BPoly
        mov     DX,SP                   ; ...
        mov     BX,SP                   ; ...
        call    __FLDM                  ; calc. z * x * Apoly / BPoly
        mov     AX,BP                   ; calc. x = z + z*x*APoly/BPoly
        mov     DX,SP                   ; ...
        mov     BX,DI                   ; ...
        call    __FLDA                  ; calc. x = z + z*x*Apoly/BPoly

;;        if( exp != 0 ) {
;;            z = exp;
;;            x = (z * C2 + x) + z * C1;
;;        }
        mov     AX,20[BP]               ; get exponent
        or      AX,AX                   ; if not zero
        _if     ne                      ; then
          cwd                           ; - convert to long
          mov   BX,BP                   ; - calc. z = exp
          call  __I4LD                  ; - ...
          mov   AX,BP                   ; - calc. temp = z * const2
          mov   DX,offset _TEXT:logconst2 ; - ...
          mov   BX,SP                   ; - ...
          call  __FLDMC                 ; - ...
          mov   AX,SP                   ; - calc. x = temp + x
          mov   DX,DI                   ; - ...
          mov   BX,DI                   ; - ...
          call  __FLDA                  ; - ...
          mov   AX,BP                   ; - calc. temp = z * const1
          mov   DX,offset _TEXT:logconst1 ; - ...
          mov   BX,SP                   ; - ...
          call  __FLDMC                 ; - ...
          mov   AX,SP                   ; - calc. x = temp + x
          mov   DX,DI                   ; - ...
          mov   BX,DI                   ; - ...
          call  __FLDA                  ; - ...
        _endif                          ; endif

;;    }
;;    return( x );
;;}
        add     SP,(10*3)+2             ; remove temporaries

        pop     BX                      ; restore registers
        pop     CX                      ; ...
        pop     DX                      ; ...
        pop     SI                      ; ...
        pop     DI                      ; ...
        pop     BP                      ; ...
        ret                             ; return
__log   endp

