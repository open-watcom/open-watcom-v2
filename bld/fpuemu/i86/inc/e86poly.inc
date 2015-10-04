
        modstart  e86poly

        xrefp   __FLDA          ; add
        xrefp   __FLDM          ; multiply
        xrefp   __FLDMC         ; multiply by constant from polynomial

        xdefp   __EvalPoly      ; evaluate polynomial
        xdefp   __Poly          ; evaluate polynomial
        xdefp   __OddPoly       ; evaluate odd degree polynomial


;void __EvalPoly( LD *x, LD *poly, int degree )
;/*****************AX*******DX*********BX*****/
;    {
;        __Poly( x, poly, degree, x );
;    }

        defp    __EvalPoly

        push    CX                      ; save CX
        mov     CX,AX                   ; result goes back into x
        call    __Poly                  ; evaluate the polynomial
        pop     CX                      ; restore CX
        ret                             ; return

        endproc __EvalPoly


;void __Poly( LD *x, LD *poly, int degree, LD *result )
;/**************AX******DX*********BX**********CX*****/
;    {
;        LD  z;
;        LD *p;
;
;    }
;

        defp    __Poly

        push    BP                      ; save BP
        push    SI                      ; save registers
        push    DI                      ; ...
        push    DX                      ; ...
        push    CX                      ; ...
        push    BX                      ; ...
        push    AX                      ; save address of x
        mov     DI,DX                   ; get address of poly
        mov     CX,BX                   ; get degree
        mov     BP,SP                   ; point to stack
        push    CS:8[DI]                ; push first coefficient onto stack
        push    CS:6[DI]                ; ...
        push    CS:4[DI]                ; ...
        push    CS:2[DI]                ; ...
        push    CS:[DI]                 ; ...

;        z = *poly;
;        do {
;            ++poly;
;            __FLDM( &z, x, &z );
;            p = &z;
;            if( degree == 1 )  p = result;      /* last add goes into result */
;            __FLDA( &z, poly, p );
;        } while( --degree != 0 );

        _loop                           ; loop
          add   DI,10                   ; - ++poly
          mov   AX,SP                   ; - p
          mov   DX,0[BP]                ; - x
          mov   BX,SP                   ; - &z
          call  __FLDM                  ; - do the multiply
          mov   SI,SP                   ; - p = &z
          cmp   CX,1                    ; - if degree == 1
          _if   e                       ; - then
            mov   SI,4[BP]              ; - - p = result
          _endif                        ; - endif
          mov   AX,SP                   ; - &z
          mov   DX,DI                   ; - poly
          mov   BX,SI                   ; - p
          call  __FLDAC                 ; - __FLDAC( &z, poly, p );
          dec   CX                      ; - decrement degree
        _until  e                       ; until done
        mov     SP,BP                   ; deallocate temporary
        pop     AX                      ; restore registers
        pop     BX                      ; ...
        pop     CX                      ; ...
        pop     DX                      ; ...
        pop     DI                      ; ...
        pop     SI                      ; ...
        pop     BP                      ; ...
        ret                             ; return

        endproc __Poly

;void __OddPoly( LD *x, LD *poly, int degree )
;/*****************AX******DX*********BX*****/
;    {
;//        return( _EvalPoly( x * x, poly, degree ) * x );
;
;        LD temp;
;
;        __FLDM( x, x, &temp );
;        __EvalPoly( &temp, poly, degree );
;        __FLDM( &temp, x, x );
;    }

        defp    __OddPoly

        sub     SP,10                   ; allocate temporary
        push    AX                      ; save AX
        push    DX                      ; save DX
        push    BX                      ; save BX
        mov     DX,AX                   ; point to x
        mov     BX,SP                   ; point to temp
        add     BX,6                    ; ...
        call    __FLDM                  ; calculate x*x
        pop     BX                      ; restore degree
        pop     DX                      ; restore address of polynomial
        mov     AX,SP                   ; point to temp
        add     AX,2                    ; ...
        call    __EvalPoly              ; evaluate polynomial
        pop     DX                      ; restore address of x
        mov     BX,DX                   ; store result in x
        mov     AX,SP                   ; point to temp
        call    __FLDM                  ; calculate result
        add     SP,10                   ; remove temp
        ret                             ; return

        endproc __OddPoly

