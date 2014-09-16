
ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    flda086, word

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<>     long double math library
;<>
;<>     inputs: AX - pointer to long double (op1)
;<>             DX - pointer to long double (op2)
;<>             BX - pointer to long double (result)
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __FLDA          ; add real*10 to real*10
        xdefp   __FLDS          ; subtract real*10 from real*10
        xdefp   __FLDAC         ; add real*10 to real*10
        xdefp   __FLDSC         ; subtract real*10 from real*10
        xdefp   ___LDA          ; long double add routine

        defp    __FLDS
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
        xor     DX,8000h        ; flip the sign
        lcall   ___LDA          ; do the add
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
        endproc __FLDS

        defp    __FLDSC
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
        mov     DX,CS:8[DI]     ; get exponent+sign of op2
        xor     DX,8000h        ; flip the sign of op2
        jmp     short _addc     ; add constant
        endproc __FLDSC

        defp    __FLDA
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
        lcall   ___LDA          ; do the add
_add9:  pop     DI              ; restore pointer to result
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
        endproc __FLDA


        defp    __FLDAC
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
        mov     DX,CS:8[DI]     ; get exponent+sign of op2
_addc:  push    CS:8[DI]        ; push constant op2 onto stack
        push    CS:6[DI]        ; ...
        push    CS:4[DI]        ; ...
        push    CS:2[DI]        ; ...
        push    CS:[DI]         ; ...
        mov     DI,SP           ; point to op2
        mov     AX,8[SI]        ; get exponent+sign of op1
        lcall   ___LDA          ; do the add
        add     SP,10           ; remove constant from stack
        jmp     _add9           ; store result
        endproc __FLDAC


; input:
;       SI - pointer to op1
;       DI - pointer to op2
;       AX - exponent+sign of op1
;       DX - exponent+sign of op2
; output:
;       SI - exponent+sign of result
;       AX:BX:CX:DX - mantissa

        defp    ___LDA
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
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
            mov   SI,DX         ; - - get exponent+sign of op2
            mov   AX,6[DI]      ; - - load op2
            mov   BX,4[DI]      ; - - ...
            mov   CX,2[DI]      ; - - ...
            mov   DX,[DI]       ; - - ...
            mov   DI,DX         ; - - see if op2 is 0
            or    DI,CX         ; - - ...
            or    DI,BX         ; - - ...
            or    DI,AX         ; - - ...
            _if   e             ; - - if mantissa zero
              _shl  SI,1        ; - - - get rid of sign bit
              _if   ne          ; - - - if exponent not zero
                rcr   SI,1      ; - - - - restore sign bit
              _endif            ; - - - endif
            _endif              ; - - endif
ifdef _BUILDING_MATHLIB
            pop   DS            ; - - restore DS
endif
            ret                 ; - - return
          _endif                ; - endif
          rcr   AX,1            ; - put back the sign
        _endguess               ; endguess

        _guess                  ; guess: op2 is 0
          mov   CX,6[DI]        ; - quit if op2 is not 0
          or    CX,CX           ; - ...
          _quif ne              ; - ...
          or    CX,4[DI]        ; - check the lower order words
          or    CX,2[DI]        ; - ...
          or    CX,[DI]         ; - ...
          _quif ne              ; - quit if op2 is not 0
          test  DX,7FFFh        ; - quit if exponent is not 0
          _quif ne              ; - ...
          mov   DI,SI           ; - get pointer to op1
          mov   SI,AX           ; - get exponent+sign of op1
          mov   AX,6[DI]        ; - load op1
          mov   BX,4[DI]        ; - ...
          mov   CX,2[DI]        ; - ...
          mov   DX,[DI]         ; - ...
ifdef _BUILDING_MATHLIB
          pop   DS              ; - restore DS
endif
          ret                   ; - return op1
        _endguess               ; endguess

        push    BP              ; save BP
        mov     BP,DX           ; assume op1 < op2 (save op2's exponent+sign)
        sub     BX,BX           ; zero BX for sign of op1
        _shl    AX,1            ; get sign of op1
        rcr     BX,1            ; save it in BX
        shr     AX,1            ; shift exponent back into place

        sub     CX,CX           ; zero CX for sign of op2
        _shl    DX,1            ; get sign of op2
        rcr     CX,1            ; save it in CX
        shr     DX,1            ; shift exponent back into place
        add     CX,BX           ; calculate sign of result

        sub     DX,AX           ; calculate difference in exponents
        _if     ne              ; if different
          _if   b               ; - if op2 < op1
            mov   BP,AX         ; - - use exponent+sign of op1
            or    BP,BX         ; - - ... (get the sign of op1)
            neg   DX            ; - - negate the shift count
            xchg  SI,DI         ; - - flip pointers to arguments
          _endif                ; - endif

;         BP has exponent+sign of largest argument
;         DI points to the largest operand

          _guess                ; - guess: can return operand 1
            cmp   DX,64         ; - - quit if shift count not too big
            _quif be            ; - - ...
            mov   DL,64         ; - - set shift count to 64
            or    CX,CX         ; - - quit if signs of operands are different
            _quif s             ; - - ...
            mov   AX,6[DI]      ; - - load largest argument
            mov   BX,4[DI]      ; - - ...
            mov   CX,2[DI]      ; - - ...
            mov   DX,[DI]       ; - - ...
            mov   SI,BP         ; - - get exponent+sign
            pop   BP            ; - - restore BP
ifdef _BUILDING_MATHLIB
            pop   DS            ; - - restore DS
endif
            ret                 ; - - return
          _endguess             ; - endguess
        _endif                  ; endif

;       load the smaller argument into registers so we can shift it
;       right to align the fractions.
;       SI points to the smaller argument.
;       DI points to the larger argument.
;       DX has amount to shift by to align the fractions.
;       BP has exponent + sign of result
;       high bit of CX = 1 iff operands have different signs

        push    CX              ; save sign indicator
        push    DX              ; save shift count
        push    DI              ; save DI (pointer to larger operand)
        push    CX              ; save sign indicator

        mov     AX,6[SI]        ; load smallest argument
        mov     BX,4[SI]        ; ...
        mov     CX,2[SI]        ; ...
        mov     SI,[SI]         ; ...
        xchg    SI,DX           ; get shift count into SI
        sub     DI,DI           ; set sticky bits to 0
        xchg    AX,DI           ; AX gets sticky bits
        _loop                   ; loop (for groups of 16 bits)
          cmp   SI,16           ; - quit if < 16 bits to shift
          _quif b               ; - ...
          or    AL,AH           ; - move sticky bits to bottom
          xor   AH,AH           ; - zero top sticky bits
          or    AX,DX           ; - set sticky bits
          mov   DX,CX           ; - shift fraction right 16 bits
          mov   CX,BX           ; - ...
          mov   BX,DI           ; - ...
          sub   DI,DI           ; - ...
          sub   SI,16           ; - adjust shift count
        _endloop                ; endloop
        cmp     SI,8            ; if >= 8 bits to move
        _if     ae              ; then
          or    AL,AH           ; - move sticky bits to bottom
          mov   AH,DL           ; - make these the top sticky bits
          mov   DL,DH           ; - shift right 8 bits
          mov   DH,CL           ; - ...
          mov   CL,CH           ; - ...
          mov   CH,BL           ; - ...
          mov   BL,BH           ; - ...
          xchg  AX,DI           ; - put sticky bits into DI
          mov   BH,AL           ; - ...
          mov   AL,AH           ; - ...
          xor   AH,AH           ; - ...
          xchg  AX,DI           ; - get sticky bits back into AX
          sub   SI,8            ; - adjust shift count
        _endif                  ; endif
        _loop                   ; loop (bit shifting)
          or    SI,SI           ; - quit if shift count = 0
          _quif e               ; - ...
          or    AL,AH           ; - move sticky bits to bottom
          shr   DI,1            ; - shift fraction right 1 bit
          rcr   BX,1            ; - ...
          rcr   CX,1            ; - ...
          rcr   DX,1            ; - ...
          rcr   AH,1            ; - save carry in AH (sticky bits)
          dec   SI              ; - decrement shift count
        _endloop                ; endloop
        xchg    AX,DI           ; place sticky bits in DI

        pop     SI              ; restore sign
        _shl    SI,1            ; set carry if operands have different signs
        pop     SI              ; restore pointer to larger operand
        _if     c               ; if operands have different signs
          sub   DX,[SI]         ; - subtract larger operand from smaller one
          sbb   CX,2[SI]        ; - ...
          sbb   BX,4[SI]        ; - ...
          sbb   AX,6[SI]        ; - ...
          _if   c               ; - if borrow
            pop   SI            ; - - restore shift count
            push  SI            ; - - save shift count
            not   AX            ; - - negate the fraction
            not   BX            ; - - ...
            not   CX            ; - - ...
            not   DX            ; - - ...
            rol   DI,1          ; - - get carry from guard bit
            ror   DI,1          ; - - ...
;           neg   DI            ; - - negate the sticky bits as well
;           _guess              ; - - guess: have to round up before negate
;             cmp   SI,64       ; - - - quit if shift count < 64
;             _quif b           ; - - - ...
;             stc               ; - - - increment
;           _endguess           ; - - endguess
;           cmc                 ; - - complement carry
            sbb   DX,-1         ; - - add 1 (or zero)
            sbb   CX,-1         ; - - ...
            sbb   BX,-1         ; - - ...
            sbb   AX,-1         ; - - ...
            xor   BP,8000h      ; - - change the sign
          _endif                ; - endif
          xor   BP,8000h        ; - change the sign
          mov   SI,AX           ; - check for 0
          or    SI,BX           ; - ...
          or    SI,CX           ; - ...
          or    SI,DX           ; - ...
          _if   e               ; - if result is 0
            pop   BP            ; - - clean up stack
            pop   BP            ; - - clean up stack
            pop   BP            ; - - restore BP
ifdef _BUILDING_MATHLIB
            pop   DS            ; - - restore DS
endif
            ret                 ; - - return
          _endif                ; - endif
        _else                   ; else operands have same signs
          add   DX,[SI]         ; - add larger operand to smaller one
          adc   CX,2[SI]        ; - ...
          adc   BX,4[SI]        ; - ...
          adc   AX,6[SI]        ; - ...
        _endif                  ; endif
        pop     SI              ; restore shift count

        mov     SI,BP           ; get exponent
        pop     BP              ; restore sign indicate

        _rcl    SI,1            ; get rid of sign bit (and save carry)
        rcr     BP,1            ; save sign bit
        shr     SI,1            ; isolate exponent (and reload carry)
        _if     nc              ; if no carry out of the top
          je    denormal        ; - denormal number when exponent hits 0
          rol   DI,1            ; - set carry from last sticky bit
          ror   DI,1            ; - ...
          _loop                 ; - loop (normalize)
            dec   SI            ; - - decrement exponent (doesn't change carry)
            je    denormal      ; - - exit if denormal operand
            _rcl  DX,1          ; - - shift number left 1
            _rcl  CX,1          ; - - ...
            _rcl  BX,1          ; - - ...
            _rcl  AX,1          ; - - ...
          _until  c             ; - until carry
        _endif                  ; endif
        inc     SI              ; bump exponent back up one
        stc                     ; set carry
        rcr     AX,1            ; shift fraction right once
        rcr     BX,1            ; ...
        rcr     CX,1            ; ...
        rcr     DX,1            ; ...
        _if     c               ; if guard bit is on
          test  BP,4000h        ; - if operands have different signs
          _if   ne              ; - then
;           shl   DI,1          ; - - get top sticky bit
;           _if   e             ; - - if no more sticky bits
;             ror   DX,1        ; - - - set carry with bottom bit of DX
;             rol   DX,1        ; - - - ...
;           _endif              ; - - endif
;           sbb   DX,0          ; - - subtract carry from answer
;           sbb   CX,0          ; - - ...
;           sbb   BX,0          ; - - ...
;           sbb   AX,0          ; - - ...
;           _if   ns            ; - - if went from 0x8000 to 0x7fff
;             mov   AX,BX       ; - - - set AX to 0xffff (BX,CX,DX == 0xffff)
;             dec   SI          ; - - - decrement exponent
;           _endif              ; - - endif
          _else                 ; - else
            _shl  DI,1          ; - - get top sticky bit
            _if   e             ; - - if no more sticky bits
              ror   DX,1        ; - - - set carry with bottom bit of DX
              rol   DX,1        ; - - - ...
            _endif              ; - - endif
            adc   DX,0          ; - - add carry into result
            adc   CX,0          ; - - ...
            adc   BX,0          ; - - ...
            adc   AX,0          ; - - ...
            _if   c             ; - - if we got a carry
              mov   AH,80h      ; - - - set fraction to 8000 ...
              inc   SI          ; - - - increment exponent
            _endif              ; - - endif
          _endif                ; - endif
        _endif                  ; endif

denormal:
        and     BP,8000h        ; isolate sign
        or      SI,BP           ; merge sign in with exponent
        pop     BP              ; restore BP
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        ret                     ; return

        endproc ___LDA


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
