;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<>     long double math library
;<>
;<>     inputs: AX - pointer to long double (op1)
;<>             DX - pointer to long double (op2)
;<>             BX - pointer to long double (result)
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __FLDM           ; 10-byte real multiply
        xdefp   ___LDM           ; 10-byte real multiply


;=====================================================================

        defp    __FLDM
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
        call    ___LDM          ; do the multiply
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
        endproc __FLDM

        defp    __FLDMC
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
        push    CS:8[DI]        ; push constant op2 onto stack
        push    CS:6[DI]        ; ...
        push    CS:4[DI]        ; ...
        push    CS:2[DI]        ; ...
        push    CS:[DI]         ; ...
        mov     DI,SP           ; point to op2
        call    ___LDM          ; do the multiply
        add     SP,10           ; remove constant from stack
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
        endproc __FLDMC

        defp    mul_oflow       ; overflow
        mov     SI,CX           ; get sign
        or      SI,7FFFh        ; set exponent for infinity
        mov     AX,8000h        ; set fraction
        sub     BX,BX           ; ...
        mov     CX,BX           ; ...
        mov     DX,BX           ; ...
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return
        endproc mul_oflow
;
; input:
;       DI - pointer to operand
;       AX - sign+exponent of op1
;       DX - sign+exponent of op2
;
        defp    mul_by_pow2
        mov     CX,AX           ; calc. sign of result
        xor     CX,DX           ; ...
        and     CX,8000h        ; ...
        and     AX,7FFFh        ; isolate exponent
        and     DX,7FFFh        ; ...
        _guess                  ; guess: overflow
          cmp   AX,7FFFh        ; - see if op1 was infinity     06-nov-92
          je    mul_oflow       ; - answer is infinity
          cmp   DX,7FFFh        ; - see if op2 was infinity
          je    mul_oflow       ; - answer is infinity
          add   AX,DX           ; - determine exponent of result
          sub   AX,3FFFh        ; - remove extra bias
          _quif c               ; - quit if exponent is negative
          cmp   AX,7FFFh        ; - check for overflow
          jae   mul_oflow       ; - . . .
        _endguess               ; endguess
        cmp     AX,-64          ; if exponent is too small
        _if     l               ; then underflow
          sub   AX,AX           ; - set result to 0
          mov   BX,AX           ; - ...
          mov   CX,AX           ; - ...
          mov   DX,AX           ; - ...
          mov   SI,AX           ; - ...
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endif                  ; endif
        push    CX              ; save sign
        mov     SI,AX           ; get exponent
        mov     AX,6[DI]        ; load mantissa
        mov     BX,4[DI]        ; ...
        mov     CX,2[DI]        ; ...
        mov     DX,[DI]         ; ...
        pop     DI              ; restore sign
        or      SI,SI           ; if denormal number
        _if     s               ; then
          _loop                 ; - loop
            shr   AX,1          ; - - shift number right
            rcr   BX,1          ; - - ...
            rcr   CX,1          ; - - ...
            rcr   DX,1          ; - - ...
            inc   SI            ; - - increment exponent
          _until  e             ; - until done
        _endif                  ; endif
        or      SI,DI           ; get sign
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return
        endproc mul_by_pow2


; input:
;       SI - pointer to op1
;       DI - pointer to op2
; output:
;       SI - exponent+sign of result
;       AX:BX:CX:DX - mantissa


        defp    ___LDM
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        mov     AX,8[SI]        ; get exponent+sign of op1
        mov     DX,8[DI]        ; get exponent+sign of op2
        _guess                  ; guess: op1 is 0
          sub   BX,BX           ; - counter for number of zeros
          mov   CX,[SI]         ; - quit if op1 is not 0
          or    CX,CX           ; - ...
          _quif ne              ; - ...
          inc   BX              ; - op1 has at least 1 zero
          or    CX,2[SI]        ; - check the higher order words
          _quif ne              ; - ...
          inc   BX              ; - op1 has at least 2 zeros
          or    CX,4[SI]        ; - ...
          _quif ne              ; - ...
          inc   BX              ; - op1 has at least 3 zeros
          or    CX,6[SI]        ; - ...

;         if CX = 8000h, then op1 is a power of 2, just return op2 with
;         appropriate sign and exponent
          cmp   CX,8000h        ; - if op1 is a power of 2
pow2_hop: je    mul_by_pow2     ; - then do special calc.

          or    CX,CX           ; - test high order word
          _quif ne              ; - quit if op1 is not 0
          _shl  AX,1            ; - place sign in carry
          _if   e               ; - if operand one is 0
            mov   BX,AX         ; - - set result to 0
            mov   DX,AX         ; - - ...
            mov   SI,AX         ; - - ...
ifdef _BUILDING_MATHLIB
            pop   DS            ; - - restore DS
endif
            ret                 ; - - return
          _endif                ; - endif
          rcr   AX,1            ; - put back the sign
        _endguess               ; endguess

        _guess                  ; guess: op2 is 0
          mov   CX,[DI]         ; - quit if op2 is not 0
          or    CX,CX           ; - ...
          _quif ne              ; - ...
          dec   BX              ; - decrement counter
          or    CX,2[DI]        ; - check the higher order words
          _quif ne              ; - ...
          dec   BX              ; - decrement counter
          or    CX,4[DI]        ; - ...
          _quif ne              ; - ...
          dec   BX              ; - decrement counter
          or    CX,6[DI]        ; - ...

;         if CX = 8000h, then op2 is a power of 2, just return op1 with
;         appropriate sign and exponent
          xchg  SI,DI           ; - flip pointers
          cmp   CX,8000h        ; - if op2 is a power of 2
          je    pow2_hop        ; - then do special calc.
          xchg  SI,DI           ; - flip pointers back

          or    CX,CX           ; - test high order word
          _quif ne              ; - quit if op2 is not 0
          test  DX,7FFFh        ; - quit if exponent is not 0
          _quif ne              ; - ...
          mov   AX,CX           ; - set result to 0
          mov   BX,CX           ; - ...
          mov   DX,CX           ; - ...
          mov   SI,CX           ; - ...
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return 0
        _endguess               ; endguess
        or      BX,BX           ; if op2 has more lower word zeros than op1
        _if     s               ; then
          xchg  SI,DI           ; - flip the arguments to reduce # of multiplies
        _endif                  ; endif

        mov     CX,AX           ; calc. sign of result
        xor     CX,DX           ; ...
        and     CX,8000h        ; ...
        and     AH,7Fh          ; isolate exponent
        and     DH,7Fh          ; ...

        _guess                  ; guess: overflow
          add   AX,DX           ; - determine exponent of result
          sub   AX,3FFEh        ; - remove extra bias
          _quif s               ; - quit if exponent is negative
          cmp   AX,7FFFh        ; - quit if not overflow
          _quif b               ; - . . .
          jmp   mul_oflow       ; - handle overflow
        _endguess               ; endguess
        cmp     AX,-64          ; if exponent is too small
        _if     l               ; then underflow
          sub   AX,AX           ; - set result to 0
          mov   BX,AX           ; - ...
          mov   CX,AX           ; - ...
          mov   DX,AX           ; - ...
          mov   SI,AX           ; - ...
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return
        _endif                  ; endif

        push    BP              ; save BP
        push    CX              ; save sign of result
        push    AX              ; save exponent

        sub     BX,BX           ; zero BX
        push    BX              ; allocate and zero accumulator
        push    BX              ; ...
        push    BX              ; ...
        push    BX              ; ...
        push    BX              ; ...
        push    BX              ; ...
        push    BX              ; ...
        push    BX              ; ... (sticky bits)
        push    6[DI]           ; push multiplicand onto stack
        push    4[DI]           ; ...
        push    2[DI]           ; ...
        push    [DI]            ; ...
        mov     BP,SP           ; point to multiplicand and accumulator
        push    6[SI]           ; push multiplier onto stack
        push    4[SI]           ; ...
        push    2[SI]           ; ...
        push    [SI]            ; ...

        _loop                   ; loop (until SP=BP)
          pop   SI              ; - get next word of multiplier from stack
          mov   BX,SP           ; - get access to stack
          mov   DI,SS:14+0[BX]  ; - get low order word for sticky bits
          or    SI,SI           ; - if multiplier not zero
          _if   ne              ; - then
            sub   CX,CX         ; - - zero CX
            mov   AX,[BP]       ; - - get low order word of multiplicand
            mul   SI            ; - - do multiply
            mov   DI,AX         ; - - save result
            mov   BX,DX         ; - - ...
            mov   AX,2[BP]      ; - - get next word of multiplicand
            mul   SI            ; - - do multiply
            add   BX,AX         ; - - accumulate result
            adc   CX,DX         ; - - ...
            mov   AX,4[BP]      ; - - get next word of multiplicand
            mul   SI            ; - - do multiply
            add   CX,AX         ; - - accumulate result
            adc   DX,0          ; - - ...
            xchg  DX,SI         ; - - answer in SI, multiplier in DX
            mov   AX,6[BP]      ; - - get last word of multiplicand
            mul   DX            ; - - do multiply
            add   AX,SI         ; - - accumulate result
            adc   DX,0          ; - - ...
            mov   SI,BP         ; - - save BP
            mov   BP,SP         ; - - get access to stack
            add   DI,14+0[BP]   ; - - accumulate result
            adc   14+2[BP],BX   ; - - ...
            adc   14+4[BP],CX   ; - - ...
            adc   14+6[BP],AX   ; - - ...
            adc   14+8[BP],DX   ; - - ...
            mov   BP,SI         ; - - restore BP
          _endif                ; - endif
          or    8[BP],DI        ; - update the sticky bits
          cmp   SP,BP           ; - check to see if we are done
        _until  e               ; until done
donemul:
        mov     DI,14+0[BP]     ; get result
        mov     DX,14+2[BP]     ; ...
        mov     CX,14+4[BP]     ; ...
        mov     BX,14+6[BP]     ; ...
        mov     AX,14+8[BP]     ; ...
;
;       now have AX:BX:CX:DX:DI containing the fraction
;
        mov     BP,8[BP]        ; get sticky bits
        add     SP,8+(8*2)      ; clean up the stack
        pop     SI              ; get exponent
        _guess                  ; guess: exponent >= 0
          or    SI,SI           ; - quit if exponent < 0
          _quif s               ; - ...
          _loop                 ; - loop (normalize)
            _shl  DI,1          ; - - shift number left 1
            _rcl  DX,1          ; - - ...
            _rcl  CX,1          ; - - ...
            _rcl  BX,1          ; - - ...
            _rcl  AX,1          ; - - ...
            dec   SI            ; - - decrement exponent (doesn't change carry)
            _quif s             ; - - exit if denormal operand
          _until  c             ; - until carry
          inc   SI              ; - bump exponent back up one
          _if   e               ; - if exponent is 0
            _if   nc            ; - - if no carry from fraction
;             cmp   AX,7FFFh    ; - - - and fraction < 8000h
;             _if   le          ; - - - then
;               cmp   SP,SI     ; - - - - cmp things that aren't equal
;             _endif            ; - - - endif
            _endif              ; - - endif
          _endif                ; - endif
          _quif e               ; - exit if denormal operand
        _admit                  ; admit: exponent <= 0
          _loop                 ; - loop (number is a denormal)
            shr   AX,1          ; - - shift number right
            rcr   BX,1          ; - - ...
            rcr   CX,1          ; - - ...
            rcr   DX,1          ; - - ...
            adc   BP,0          ; - - add carry to sticky bits
            inc   SI            ; - - increment exponent
          _until  ns            ; - until back to 0
          clc                   ; - clear carry
        _endguess               ; endguess
        rcr     AX,1            ; shift fraction right once
        rcr     BX,1            ; ...
        rcr     CX,1            ; ...
        rcr     DX,1            ; ...
        _if     c               ; if guard bit is on
          or    BP,BP           ; - if no sticky bits
          _if   e               ; - then
            or    SI,SI         ; - - if exponent not 0
            _if   ne            ; - - then
              mov   BP,DX       ; - - - get low order word
              shr   BP,1        ; - - - get bottom bit
              cmc               ; - - - complement it
            _endif              ; - - endif
          _endif                ; - endif
          cmc                   ; - complement carry
          adc   DX,0            ; - round up
          adc   CX,0            ; - ...
          adc   BX,0            ; - ...
          adc   AX,0            ; - ...
          _if   c               ; - if carry            05-jul-90
            inc   SI            ; - - increment exponent
            mov   AH,80h        ; - - set fraction
          _endif                ; - endif
        _endif                  ; endif
        pop     BP              ; restore sign
        or      SI,BP           ; merge sign in with exponent
        pop     BP              ; restore BP
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return

mul_denormal:
        pop     SI              ; restore sign
        pop     BP              ; restore BP
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return

        endproc ___LDM

