
        modstart e86fprem

        xdefp   __fprem

;       int __fprem( LD *x, LD *modulus )
;
        defp    __fprem
        push    SI              ; save SI
        push    DI              ; save DI
        push    CX              ; save CX
        push    BX              ; save BX
        mov     SI,AX           ; op1
        mov     DI,DX           ; op2
        mov     AX,8[SI]        ; get exponent of op1
        mov     DX,8[DI]        ; get exponent of op2


        _guess                  ; guess: op1 is 0
          mov   CX,[SI]         ; - quit if op1 is not 0
          or    CX,2[SI]        ; - check the higher order words
          or    CX,4[SI]        ; - ...
          or    CX,6[SI]        ; - ...
          _quif ne              ; - ...
          test  AX,7FFFh        ; - quit if exponent not 0
          _quif ne              ; - ...
_return_0:
          sub   AX,AX           ; - quotient is 0
          pop   BX              ; - restore registers
          pop   CX              ; - ...
          pop   DI              ; - ...
          pop   SI              ; - ...
          ret                   ; - return
        _endguess               ; endguess

        _guess                  ; guess: op2 is 0
          mov   CX,[DI]         ; - quit if op2 is not 0
          or    CX,2[DI]        ; - check the higher order words
          or    CX,4[DI]        ; - ...
          or    CX,6[DI]        ; - ...
          _quif ne              ; - quit if op2 is not 0
          test  DX,7FFFh        ; - quit if exponent is not 0
          _quif ne              ; - ...
          mov   [SI],CX         ; - set result to 0
          mov   2[SI],CX        ; - ...
          mov   4[SI],CX        ; - ...
          mov   6[SI],CX        ; - ...
          mov   8[SI],CX        ; - ...
          jmp   _return_0       ; - return 0
        _endguess               ; endguess

        push    BP              ; save BP
        push    SI              ; save address for remainder
        push    6[DI]           ; push divisor
        push    4[DI]           ; ...
        push    2[DI]           ; ...
        push    [DI]            ; ...
        mov     BP,SP           ; point to divisor

        push    AX              ; save sign+exponent of op1
        push    DX              ; save sign+exponent of op2 (modulus)

        mov     DI,DX           ; get exponent of op2
        and     DI,7FFFh        ; isolate exponent of op2

        mov     BX,4[SI]        ; load dividend
        mov     CX,2[SI]        ; ...
        mov     DX,[SI]         ; ...
        mov     SI,6[SI]        ; ...
        xchg    AX,SI           ; get difference of exponents in SI
        and     SI,7FFFh        ; isolate exponent of op1
        sub     SI,DI           ; calculate difference in exponents
        _if     ge              ; if operand >= modulus
          mov   DI,0            ; - set quotient to 0
          _loop                 ; - loop
            _guess              ; - - guess
              cmp  6[BP],AX     ; - - - The purpose of this guess is to
              jb   try          ; - - - determine if the divisor will subtract
              ja   didnt_go     ; - - - from the dividend without a borrow, and to
              cmp  4[BP],BX     ; - - - branch to the appropriate routine
              jb   try          ; - - -
              ja   didnt_go     ; - - -
              cmp  2[BP],CX     ; - - -
              _quif ne          ; - - -
              cmp  0[BP],DX     ; - - -
              je   try          ; - - -
            _endguess           ; - - endguess
            _if   c             ; - - if the carry is set (ie the modulus will
                                ; - - - definitely subtract from the dividend
                                ; - - - without a borrow
try:
              sub  DX,0[BP]     ; - - - subtract divisor from dividend
              sbb  CX,2[BP]     ; - - - . . .
              sbb  BX,4[BP]     ; - - - . . .
              sbb  AX,6[BP]     ; - - - . . .
              stc               ; - - - set carry to indicate that modulus was
                                ; - - - successfully subtracted from dividend
            _endif              ; - - endif
didnt_go:   _rcl  DI,1          ; - - rotate 1 (if carry set) into quotient word
            dec   SI            ; - - adjust difference in exponents
            jl    _done         ; - - quit if done
            _shl  DX,1          ; - - shift dividend left
            _rcl  CX,1          ; - - . . .
            _rcl  BX,1          ; - - . . .
            _rcl  AX,1          ; - - . . .
            jc    try

;   If bit 5 of dividend is set here, we didnt subtract the modulus from the
;   dividend (recall that the divisor has a 1 in the msb -- if we subtracted
;   it from the dividend without a borrow, the dividend would not have a one
;   in its msb to be shifted into bit 5 tested for in the condition above. If
;   we are rotating a bit into bit 5, the dividend is now big enough that we
;   can be sure of subtracting out the divisor without a borrow, as we have
;   shifted it left one digit.

            or    AX,AX         ; - - check top bit of dividend
          _until  ns            ; - until
          jmp   short didnt_go  ; - continue
_done:    sub   SI,SI           ; - set SI to 0
;         normalize the remainder in AX:BX:CX:DX
          _guess                ; - guess: number is zero
            or    AX,AX         ; - - quit if not zero
            _quif ne            ; - - ...
            or    BX,BX         ; - - ...
            _quif ne            ; - - ...
            or    CX,CX         ; - - ...
            _quif ne            ; - - ...
            or    DX,DX         ; - - ...
            _quif ne            ; - - ...
          _admit                ; - admit: not zero
            _loop               ; - - loop
              or    AX,AX       ; - - - quit if number is normalized
              _quif s           ; - - - . . .
              _shl  DX,1        ; - - - shift result left
              _rcl  CX,1
              _rcl  BX,1
              _rcl  AX,1
              dec   SI          ; - - - decrement exponent
            _endloop            ; - - endloop
            pop   BP            ; - - restore exponent+sign of modulus
            push  BP            ; - - save it again
            and   BP,7FFFh      ; - - isolate exponent of modulus
            add   SI,BP         ; - - adjust exponent of result
          _endguess             ; - endguess
        _else                   ; else
          add   SI,DI           ; - restore exponent
          sub   DI,DI           ; - set quotient to 0
        _endif                  ; endif

        mov     BP,SP           ; get access to temps
        mov     BP,2[BP]        ; get sign+exponent of op1
        and     BP,8000h        ; isolate sign
        or      SI,BP           ; merge it with exponent
        pop     BP              ; restore sign+exponent of modulus
        xor     BP,SI           ; calculate sign of quotient
        _if     s               ; if quotient should be negative
          neg   DI              ; - negate quotient
        _endif                  ; endif
        or      AX,AX           ; test high word of remainder
        _if     e               ; if remainder is zero
          sub   SI,SI           ; - zero sign and remainder
        _endif                  ; endif
        add     SP,10           ; clean up the stack

_return:
        pop     BP              ; restore address for remainder
        xchg    BP,SI           ; ....
        mov     [SI],DX         ; store remainder
        mov     2[SI],CX        ; ...
        mov     4[SI],BX        ; ...
        mov     6[SI],AX        ; ...
        mov     8[SI],BP        ; ...
        mov     AX,DI           ; get quotient
        pop     BP              ; restore BP
        pop     BX              ; restore registers
        pop     CX              ; ...
        pop     DI              ; ...
        pop     SI              ; ...
        ret                     ; return
        endproc __fprem

