
        xref    _COne            ; 1.0

        modstart  e86f2xm1

        xref    __FLDA          ; add
        xref    __FLDS          ; subtract
        xref    __FLDD          ; divide
        xref    __FLDM          ; multiply

        xref    __FLDAC         ; add
        xref    __FLDSC         ; subtract
        xref    __FLDMC         ; multiply

        xdefp   __f2xm1         ; calc 2**x - 1


ExpConsts   dt  1.0442737824274138403   ;/*   sqrt(sqrt(sqrt(sqrt(2))))  */
            dt  1.0905077326652576592   ;/*   sqrt(sqrt(sqrt(2)))        */
            dt  1.1892071150027210667   ;/*   sqrt(sqrt(2))              */
            dt  1.4142135623730950488   ;/*   sqrt(2)                    */

const0  dt      20.813771196523035
const1  dt      0.057761135831801924
const2  dt      7.213503410844819


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
        push    BP                      ; save EBP
        push    DI                      ; save registers
        push    SI                      ; ...
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

;;    if( fabs( x ) < 4.445e-16 ) {       /* if argument is too small */
;;        x = 1.0;
;;    } else {
;;        exp = 0;
        sub     SI,SI                   ; exp = 0
;;        exp2 = 0;
;;        sgnx = _sgn( x );
;;        x = fabs( x );
        mov     CX,8[DI]                ; get exponent and sign
        and     byte ptr 9[DI],7Fh      ; force number to be positive

;;        if( x != 0.0 ) {
        mov     DX,8[DI]                ; check to see of x == 0.0
        or      DX,6[DI]                ; ...
        or      DX,4[DI]                ; ...
        or      DX,2[DI]                ; ...
        or      DX,[DI]                 ; ...
        _if     ne                      ; if x != 0.0
;;            if( sgnx < 0 ) {
;;                --exp;
;;                x = 1.0 - x;
;;            }
          or    CX,CX                   ; - if number was negative
          _if   s                       ; - then
            dec   SI                    ; - - decrement exponent
            xor   byte ptr 9[DI],80h    ; - - negate x
            mov   AX,DI                 ; - - calc. x = (-x) + 1.0
            mov   DX,offset _TEXT:_COne ; - - ...
            mov   BX,DI                 ; - - ...
            call  __FLDAC               ; - - ...
          _endif                        ; - endif

;;            x = modf( ldexp( x, 4 ), &ipart );
;;            if( x != 0.0 ) {
;;                x = ldexp( x, -4 );
;;            }
;;            exp2 = ipart;
          mov   BX,8[DI]                ; - get exponent
          add   BX,4                    ; - multiply by 16

          sub   CX,CX                   ; - set ipart to 0
          _loop                         ; - loop
            cmp   BX,3FFFh              ; - - quit if done
            _quif b                     ; - - ...
            shl   word ptr [DI],1       ; - - shift left one
            rcl   word ptr 2[DI],1      ; - - ...
            rcl   word ptr 4[DI],1      ; - - ...
            rcl   word ptr 6[DI],1      ; - - ...
            _rcl  CX,1                  ; - - into integer part
            dec   BX                    ; - - decrement exponent
          _endloop                      ; - endloop
          mov   DX,[DI]                 ; - get fraction
          or    DX,2[DI]                ; - ...
          or    DX,4[DI]                ; - ...
          or    DX,6[DI]                ; - ...
          _if   e                       ; - if fraction is zero
            sub   BX,BX                 ; - - zero exponent
          _else                         ; - else
            mov   DX,6[DI]              ; - - get high order word of fraction
            _loop                       ; - - loop (normalize fraction)
              or  DX,DX                 ; - - - check top bit
              _quif s                   ; - - - quit if normalized
              shl word ptr [DI],1       ; - - - shift left
              rcl word ptr 2[DI],1      ; - - - ...
              rcl word ptr 4[DI],1      ; - - - ...
              _rcl DX,1                 ; - - - ...
              dec BX                    ; - - - decrement exponent
            _endloop                    ; - - endloop
            sub   BX,4                  ; - - divide by 16
          _endif                        ; - endif
          mov   6[DI],DX                ; - update fraction
          mov   8[DI],BX                ; - ...
        _else                           ; else
          sub   CX,CX                   ; - exp2 = 0
        _endif                          ; endif
;;        }
        push    SI                      ; save exp
        push    CX                      ; save exp2

        sub     SP,10*2                 ; allocate two temporaries
        mov     CX,SP                   ; calc. address of temp2
        add     CX,10                   ; ...

;;        ee = x * x;

        mov     AX,DI                   ; calc. ee = x * x
        mov     DX,DI                   ; ...
        mov     BX,SP                   ; ...
        call    __FLDM                  ; ...

;;        a = ee + const0;

        mov     AX,SP                   ; calc. a = ee + const0
        mov     DX,offset _TEXT:const0  ; ...
        mov     BX,CX                   ; ...
        call    __FLDAC                 ; ...

;;        b = (ee * const1 + const2) * x;

        mov     AX,SP                   ; calc. b = ee * const1
        mov     DX,offset _TEXT:const1  ; ...
        mov     BX,SP                   ; ...
        call    __FLDMC                 ; ...
        mov     AX,SP                   ; calc. b = (ee * const1 + const2)
        mov     DX,offset _TEXT:const2  ; ...
        mov     BX,SP                   ; ...
        call    __FLDAC                 ; ...
        mov     AX,SP                   ; calc. b = (ee * const1 + const2) * x
        mov     DX,DI                   ; ...
        mov     BX,SP                   ; ...
        call    __FLDM                  ; ...

;;        x = (a + b) / (a - b);

        mov     AX,CX                   ; calc. x = (a + b)
        mov     DX,SP                   ; ...
        mov     BX,DI                   ; ...
        call    __FLDA                  ; ...
        mov     AX,CX                   ; calc. temp = (a - b)
        mov     DX,SP                   ; ...
        mov     BX,SP                   ; ...
        call    __FLDS                  ; ...
        mov     AX,DI                   ; calc. x = (a + b) / (a - b)
        mov     DX,SP                   ; ...
        mov     BX,DI                   ; ...
        call    __FLDD                  ; ...

        add     SP,10*2                 ; clean up stack
        pop     CX                      ; restore exp2

;;        poly = ExpConsts;
        mov     SI,offset _TEXT:ExpConsts
;;        while( exp2 != 0 ) {
        _loop                           ; loop
          or    CX,CX                   ; - quif exp2 == 0
          _quif e                       ; - ...
;;            if( exp2 & 1 ) {
          shr   CX,1                    ; - get next bit
          _if   c                       ; - if on
;;                x *= *poly;
            mov     AX,DI
            mov     DX,SI
            mov     BX,DI
            call    __FLDMC
;;            }
          _endif                        ; - endif
;;            ++poly;
          add   SI,10                   ; - ++poly
;;            exp2 = exp2 >> 1;
;;        }
        _endloop                        ; endloop
;;        x = ldexp( x, exp );
        pop     SI                      ; restore exp
        add     8[DI],SI
;;        x = x - 1.0;
        mov     AX,DI
        mov     DX,offset _TEXT:_COne
        mov     BX,DI
        call    __FLDSC
;;    }
;;    return( x );
;;}

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
        pop     SI                      ; ...
        pop     DI                      ; ...
        pop     BP                      ; ...
        ret                             ; return
__f2xm1 endp

