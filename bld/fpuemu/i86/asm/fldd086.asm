
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        xrefp   FPDivZero
        xrefp   FPInvalidOp

        modstart    fldd086, word


endif


        xdefp   __FLDD
        xdefp   ___LDD

; void __FLDD( long double *op1 , long double *op2, long double *result )
;       AX - pointer to op1
;       DX - pointer to op2
;       BX - pointer for result
;

        defp    __FLDD
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        push    SI              ; save SI
        push    DI              ; save DI
        push    CX              ; save CX
        push    BX              ; save pointer to result
        mov     SI,AX           ; point to op1
        mov     DI,DX           ; point to op2
        mov     AX,8[SI]        ; get exponent+sign of op1
        mov     DX,8[DI]        ; get exponent+sign of op2
        lcall   ___LDD          ; do the divide
        pop     DI              ; restore pointer to result
        mov     [DI],DX         ; store result
        mov     2[DI],CX        ; ...
        mov     4[DI],BX        ; ...
        mov     6[DI],AX        ; ...
        mov     8[DI],SI        ; ...
        pop     CX              ; restore CX
        pop     DI              ; restore DI
        pop     SI              ; restore SI
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return
        endproc __FLDD


; input:
;       SI - pointer to op1
;       DI - pointer to op2
;       AX - exponent+sign of op1
;       DX - exponent+sign of op2
; output:
;       SI - exponent+sign of result
;       AX:BX:CX:DX - mantissa

        defp    ___LDD
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        sub     BX,BX           ; indicate not a power of two
        _guess                  ; guess: divide by 0
          mov   CX,4[DI]        ; - check the lower order words
          or    CX,2[DI]        ; - ...
          or    CX,[DI]         ; - ...
          _quif ne              ; - quit if op2 is not 0
          mov   BX,6[DI]        ; - quit if op2 is not 0
          or    BX,BX           ; - ...
          _quif ne              ; - ...
          test  DX,7FFFh        ; - quit if exponent is not 0
          _quif ne              ; - ...
          _guess                ; - guess: 0/0 (invalid operation)
            mov   CX,6[SI]      ; - - quit if op1 is not 0
            or    CX,4[SI]      ; - - check the lower order words
            or    CX,2[SI]      ; - - ...
            or    CX,[SI]       ; - - ...
            _quif ne            ; - - quit if op1 is not 0
            mov   CX,AX         ; - - get sign+exponent
            _shl  CX,1          ; - - place sign in carry
            _quif ne            ; - - quit if not 0
            call  FPInvalidOp   ; - - process exception
            mov   SI,0FFFFh     ; - - return NaN
            mov   AX,0C000h     ; - - ...
          _admit                ; - admit: divide by 0
            mov   SI,AX         ; - - get exponent+sign of op1
            call  FPDivZero     ; - - process exception
            or    SI,7FFFh      ; - - return signed infinity
            mov   AX,8000h      ; - - ...
          _endguess             ; - endguess
          sub   BX,BX           ; - zero rest of fraction
          sub   CX,CX           ; - ...
          sub   DX,DX           ; - ...
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endguess               ; endguess

        _guess                  ; guess: op1 is 0
          mov   CX,6[SI]        ; - quit if op1 is not 0
          or    CX,CX           ; - ...
          _quif ne              ; - ...
          or    CX,4[SI]        ; - check the lower order words
          or    CX,2[SI]        ; - ...
          or    CX,[SI]         ; - ...
          _quif ne              ; - quit if op1 is not 0
          _shl  AX,1            ; - place sign in carry
          _if   e               ; - if operand one is 0
            mov   BX,AX         ; - - return zero
            mov   DX,AX         ; - - ...
            mov   SI,AX         ; - - ...
ifdef _BUILDING_MATHLIB
            pop   DS            ; - - restore DS
endif
            ret                 ; - - return
          _endif                ; - endif
          rcr   AX,1            ; - put back the sign
        _endguess               ; endguess

        push    BP              ; save BP
        mov     BP,SP           ; get pointer to saved BP
        mov     CX,3            ; set loop count
        push    CX              ; ...

        mov     CX,AX           ; get sign of op1
        xor     CX,DX           ; calc. sign for result
        and     CX,8000h        ; ...
        and     AH,7Fh          ; isolate exponent of op1
        and     DH,7Fh          ; isolate exponent of op2

        _guess                  ; guess: divide by power of 2
          cmp   BX,8000h        ; - quit if divisor is not a power of two
          _quif ne              ; - ...
          or    AX,AX           ; - quit if op1 is denormal
          _quif e               ; - ...
          or    DX,DX           ; - quit if op2 is denormal
          _quif e               ; - ...
          mov   BX,AX           ; - get exponent of op1
          sub   BX,DX           ; - determine exponent of result
          add   BX,3FFFh        ; - add in removed bias
          _quif le              ; - quit if underflow
          cmp   BX,7FFFh        ; - quit if overflow
          je    div_oflow       ; - ...
          mov   DI,BX           ; - get exponent of result
          or    DI,CX           ; - merge in the sign
          mov   AX,6[SI]        ; - load op1
          mov   BX,4[SI]        ; - ...
          mov   CX,2[SI]        ; - ...
          mov   DX,[SI]         ; - ...
          mov   SI,DI           ; - get exponent+sign
          mov   SP,BP           ; - clean up stack
          pop   BP              ; - restore BP
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endguess               ; endguess

count   equ     -2              ; loop counter
den     equ     -10             ; denominator
num     equ     -18             ; numerator
sign    equ     -20             ; sign of result
exponent equ    -22             ; exponent of result
quot    equ     -30             ; quotient

        mov     BX,6[DI]        ; get high order word of op2
        push    BX              ; duplicate op2 on the stack
        push    4[DI]           ; ...
        push    2[DI]           ; ...
        push    [DI]            ; ...
        mov     DI,SP           ; point to pushed value
        or      DX,DX           ; if op2 is a denormal
        _if     e               ; then
          call  normalize_opnd  ; - normalize op2
        _endif                  ; endif

        mov     BX,6[SI]        ; get high order word of op1
        push    BX              ; duplicate op1 on the stack
        push    4[SI]           ; ...
        push    2[SI]           ; ...
        push    [SI]            ; ...
        mov     DI,SP           ; point to pushed value
        or      AX,AX           ; if op1 is a denormal
        _if     e               ; then
          xchg  AX,DX           ; - exchange exponents
          call  normalize_opnd  ; - normalize op1
          xchg  AX,DX           ; - exchange exponents
          or    DX,DX           ; - if other operand is a normal
          _if   g               ; - then
            inc   AX            ; - - adjust exponent
          _endif                ; - endif
        _endif                  ; endif

        _guess                  ; guess: overflow
          sub   AX,DX           ; - determine exponent of result
          add   AX,3FFFh        ; - add in removed bias
          _quif s               ; - quit if exponent is negative
          cmp   AX,7FFFh        ; - quit if not overflow
          _quif b               ; - . . .
div_oflow:mov   SI,CX           ; - get sign of result
          or    SI,7FFFh        ; - return signed infinity
          mov   AX,8000h        ; - ...
          sub   BX,BX           ; - ...
          sub   CX,CX           ; - ...
          sub   DX,DX           ; - ...
          mov   SP,BP           ; - clean up stack
          pop   BP              ; - restore BP
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return signed infinity
        _endguess               ; endguess
        cmp     AX,-64          ; if exponent is too small
        _if     l               ; then underflow
          sub   AX,AX           ; - set result to 0
          mov   BX,AX           ; - ...
          mov   CX,AX           ; - ...
          mov   DX,AX           ; - ...
          mov   SI,AX           ; - ...
          mov   SP,BP           ; - clean up stack
          pop   BP              ; - restore BP
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endif                  ; endif

        push    CX              ; save sign of result
        push    AX              ; save exponent of result

        mov     DX,num+6[BP]    ; get high part of dividend
        mov     AX,num+4[BP]    ; ...
        mov     SI,den+6[BP]    ; get high word of divisor
        sub     BX,BX           ; assume high word of quotient = 0
        cmp     SI,DX           ; if divisor <= high word of dividend
        _if     be              ; then
          sub   DX,SI           ; - subtract divisor from dividend
          inc   BX              ; - set high word of quotient to 1
        _endif                  ; endif
        push    BX              ; save high word of quotient    (quot+6[BP])
        div     SI              ; estimate next word of quotient
        push    AX              ; save estimate of quotient     (quot+4[BP])
        mov     SI,AX           ; get estimate as multiplier
;
;       calculate divisor * estimated quotient
;
        sub     CX,CX           ; zero CX
        mov     AX,den+0[BP]    ; get low order word of divisor
        mul     SI              ; do multiply
        mov     DI,AX           ; save result
        mov     BX,DX           ; ...
        mov     AX,den+2[BP]    ; get next word of divisor
        mul     SI              ; do multiply
        add     BX,AX           ; accumulate result
        adc     CX,DX           ; ...
        mov     AX,den+4[BP]    ; get next word of divisor
        mul     SI              ; do multiply
        add     CX,AX           ; accumulate result
        adc     DX,0            ; ...
        xchg    DX,SI           ; answer in SI, multiplier in DX
        mov     AX,den+6[BP]    ; get last word of divisor
        mul     DX              ; do multiply
        add     AX,SI           ; accumulate result
        adc     DX,0            ; ...

;       result is in DX:AX:CX:BX:DI

        cmp     word ptr quot+6[BP],0 ; if high word of quotient is 1
        _if     ne              ; then
          add   BX,den+0[BP]    ; - add in divisor
          adc   CX,den+2[BP]    ; - ...
          adc   AX,den+4[BP]    ; - ...
          adc   DX,den+6[BP]    ; - ...
        _endif                  ; endif
;
;       subtract estimate * divisor from dividend
;
        not     DX              ; negate estimate * divisor
        not     AX              ; ...
        not     CX              ; ...
        not     BX              ; ...
        neg     DI              ; ...
        sbb     BX,-1           ; ...
        sbb     CX,-1           ; ...
        sbb     AX,-1           ; ...
        sbb     DX,-1           ; ...
;
;       add dividend to this
;
        add     BX,num+0[BP]    ; ...
        adc     CX,num+2[BP]    ; ...
        adc     AX,num+4[BP]    ; ...
        adc     DX,num+6[BP]    ; ...
        _if     ne              ; if high word not 0 (quotient is too big)
          _loop                 ; - loop (find correct quotient)
            sub   word ptr quot+4[BP],1 ; - - decrement quotient
            sbb   word ptr quot+6[BP],0 ; - - ...
            add   DI,den+0[BP]  ; - - add divisor back to dividend
            adc   BX,den+2[BP]  ; - - ...
            adc   CX,den+4[BP]  ; - - ...
            adc   AX,den+6[BP]  ; - - ...
            adc   DX,0          ; - - ...
          _until  e             ; - until done
        _endif                  ; endif
        mov     num+0[BP],DI    ; save new dividend
        mov     num+2[BP],BX    ; ...
        mov     num+4[BP],CX    ; ...
        mov     num+6[BP],AX    ; ...

divloop:;_loop                  ; loop (for rest of estimates)


        xchg    DX,AX           ; get high part of dividend into DX:AX
        xchg    AX,CX           ; ...
        mov     SI,den+6[BP]    ; get high word of divisor
        cmp     SI,DX           ; if divisor <= high word of dividend
        _if     be              ; then
;         sub   DX,SI           ; - subtract divisor from dividend
;         pop   BX              ; - restore last word of quotient
;         add   BX,1            ; - increment it
;         _if   c               ; - if carry
;           pop   CX            ; - - get next word
;           add   CX,1          ; - - increment it
;         what about carry?
;           push  CX            ; - - save updated quotient word
;         _endif                ; - endif
;         push  BX              ; - restore last word of quotient
          mov   AX,0FFFFh       ; - set estimate to 0xffff
        _else                   ; else
          div   SI              ; - estimate next word of quotient
        _endif                  ; endif
        push    AX              ; save estimate of quotient     (quot+2[BP])
        sub     CX,CX           ; assume quotient is 0
        mov     BX,CX           ; ...
        mov     DX,AX           ; ...
        or      AX,AX           ; if quotient not zero
        _if     ne              ; then
          xchg  SI,AX           ; - get estimate as multiplier
;
;         calculate divisor * estimated quotient
;
          mov   AX,den+0[BP]    ; - get low order word of divisor
          mul   SI              ; - do multiply
          mov   DI,AX           ; - save result
          mov   BX,DX           ; - ...
          mov   AX,den+2[BP]    ; - get next word of divisor
          mul   SI              ; - do multiply
          add   BX,AX           ; - accumulate result
          adc   CX,DX           ; - ...
          mov   AX,den+4[BP]    ; - get next word of divisor
          mul   SI              ; - do multiply
          add   CX,AX           ; - accumulate result
          adc   DX,0            ; - ...
          xchg  DX,SI           ; - answer in SI, multiplier in DX
          mov   AX,den+6[BP]    ; - get last word of divisor
          mul   DX              ; - do multiply
          add   AX,SI           ; - accumulate result
          adc   DX,0            ; - ...

;       result is in DX:AX:CX:BX:DI
;
;       subtract estimate * divisor from dividend
;
          not   DX              ; - negate estimate * divisor
          not   AX              ; - ...
          not   CX              ; - ...
          not   BX              ; - ...
          neg   DI              ; - ...
          sbb   BX,-1           ; - ...
          sbb   CX,-1           ; - ...
          sbb   AX,-1           ; - ...
          sbb   DX,-1           ; - ...
;
;       add dividend to this
;
        _endif                  ; endif
        add     BX,num+0[BP]    ; ...
        adc     CX,num+2[BP]    ; ...
        adc     AX,num+4[BP]    ; ...
        adc     DX,num+6[BP]    ; ...
        _if     ne              ; if high word not 0 (quotient is too big)
          sub   SI,SI           ; - zero correction factor
          _loop                 ; - loop (find correct quotient)
            inc   SI            ; - - increment adjustment
            add   DI,den+0[BP]  ; - - add divisor back to dividend
            adc   BX,den+2[BP]  ; - - ...
            adc   CX,den+4[BP]  ; - - ...
            adc   AX,den+6[BP]  ; - - ...
            adc   DX,0          ; - - ...
          _until  e             ; - until done
          mov   DX,BP           ; - save BP
          mov   BP,SP           ; - get access to quotient
          sub   [BP],SI         ; - adjust quotient
;         _loop                 ; - loop
;           _quif  nc           ; - - quit if no borrow
;           add    BP,2         ; - - point to next word of quotient
;           sub word ptr [BP],1 ; - - adjust quotient
;         _endloop              ; - endloop
          mov   BP,DX           ; - restore DX
          sub   DX,DX           ; - zero DX
        _endif                  ; endif
        mov     num+0[BP],DI    ; save new dividend
        mov     num+2[BP],BX    ; ...
        mov     num+4[BP],CX    ; ...
        mov     num+6[BP],AX    ; ...

        dec   word ptr count[BP]; - decrement loop count
        _if     ne              ; if not done
          jmp   divloop         ; - continue
        _endif                  ; endif
;       _until  e               ; until done

        pop     DX              ; restore quotient from stack
        pop     CX              ; ...
        pop     BX              ; ...
        pop     AX              ; ...
        pop     DI              ; load high word
        shr     DI,1            ; get top bit of quotient
        pop     SI              ; restore exponent of result
        pop     DI              ; restore sign
        mov     SP,BP           ; clean up the stack
        _if     c               ; if top bit of quotient set
          rcr   AX,1            ; - shift fraction right 1 bit
          rcr   BX,1            ; - ...
          rcr   CX,1            ; - ...
          rcr   DX,1            ; - ...
          adc   DX,0            ; - round up
          adc   CX,0            ; - ...
          adc   BX,0            ; - ...
          adc   AX,0            ; - ...
          inc   SI              ; - increment exponent
        _endif                  ; endif
        dec     SI              ; decrement exponent
        _if     le              ; if exponent is negative (denormal number)
          _loop                 ; - loop
            shr   AX,1          ; - - shift number right
            rcr   BX,1          ; - - ...
            rcr   CX,1          ; - - ...
            rcr   DX,1          ; - - ...
            inc   SI            ; - - increment exponent
          _until  ge            ; - until back to 0
          sub   SI,SI           ; - set exponent to 0
        _endif                  ; endif
        or      SI,DI           ; merge sign in with exponent
        pop     BP              ; restore BP
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return

        endproc ___LDD

normalize_opnd proc     near
;       operand is on the stack pointed to by DI

        xchg    DI,BP           ; place address of operand in BP
        _loop                   ; loop (normalize it)
          or    BX,BX           ; - check high word for 0
          _quif ne              ; - quit if not zero
          xchg  [BP],BX         ; - shift operand left 16 bits
          xchg  2[BP],BX        ; - ...
          xchg  4[BP],BX        ; - ...
          sub   DX,16           ; - decrement exponent by 16
        _endloop                ; endloop
        _loop                   ; loop (normalize it)
          or    BX,BX           ; - check for implied 1 bit
          _quif s               ; - quit if its on
          shl   word ptr [BP],1 ; - shift fraction left
          rcl   word ptr 2[BP],1; - ...
          rcl   word ptr 4[BP],1; - ...
          _rcl  BX,1            ; - ...
          dec   DX              ; - decrement exponent
        _endloop                ; endloop
        mov     6[BP],BX        ; save high order word
        xchg    DI,BP           ; restore address of operand
        ret                     ; return
normalize_opnd endp


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
