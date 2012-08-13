.386
__386__=1
STACK=0
_OS             macro
                _PLDT
                endm
;include mdef386.inc
;grb
modstart        macro   modname
                name    modname
_TEXT           segment use32 dword public 'CODE'
                assume  cs:_TEXT
                endm

xdefp           macro   xsym
                public  xsym
                endm

defp            macro   dsym
         dsym   proc    near
                endm

prolog          macro
                push    EBP
                mov     EBP,ESP
                endm

epilog          macro
                pop     EBP
                endm

xref            macro   dsym
                extrn   dsym : near
                endm

endproc         macro   dsym
         dsym   endp
                endm

endmod          macro
_TEXT           ends
                endm

datasegment     macro
_DATA           segment use32 dword public 'DATA'
DGROUP          group _DATA
                assume  ds:DGROUP
                endm

enddata         macro
_DATA           ends
                endm
;grb
;include struct3.inc
; Structured macros for MASM.

; Changes:
; 86/10/30      Peter Bumbulis  Original implementation.
; 87/01/06      Peter Bumbulis  Fixed bug in quif.
; 87/01/28      Peter Bumbulis  Added calls stuff.
; 87/09/17      Peter Bumbulis  Added guess/admit.
; 87/09/24      Peter Bumbulis  Fixed guess/admit.
; 87/09/29      Peter Bumbulis  Fixed more bugs.
; 87/10/02      Peter Bumbulis  Fixed more bugs in quif, added block labels.
; 91/06/19      D.J.Gaudet      Added _loopif
; 91/07/22      D.J.Gaudet      Made _loopif work on innermost loop
; 93/06/11      Mark Patchett   adjusted to work with wasm

        __label = 0
        __depth = 0
        __inner_loop = 0

_guess  macro   name
        __depth = __depth + 1
        __label = __label + 1
        _set    __astk,%__depth,__label,<>
        __label = __label + 1
        _set    __estk,%__depth,__label,<>
        ifnb    <name>
          name = __label
        endif
        endm

_quif   macro   cc,name
        ifnb    <name>
          _set  __elbl,,name,<>
          _j    cc,_l,%__elbl
        else
          _set  __albl,,__astk,%__depth
          _j    cc,_l,%__albl
        endif
        endm

_quit   macro   name
        _quif   ,name
        endm

_admit  macro
        _set    __albl,,__astk,%__depth
        __label = __label + 1
        _set    __astk,%__depth,__label,<>
        _set    __elbl,,__estk,%__depth
        _j      ,_l,%__elbl
        _label  _l,%__albl
        endm

_endguess macro
        _set    __albl,,__astk,%__depth
        _label  _l,%__albl
        _set    __elbl,,__estk,%__depth
        _label  _l,%__elbl
        __depth = __depth - 1
        endm

_loop   macro   name
        _guess  name
        _set    __albl,,__astk,%__depth
        _label  _m,%__albl
        _set    __elbl,,__estk,%__depth
        _label  _m,%__elbl
        _set    __llbl,%__depth,%__inner_loop,<>
        _set    __inner_loop,,%__depth,<>
        endm

_loopif macro   cc,name
        ifnb    <name>
          _set  __elbl,,name,<>
          _j    cc,_m,%__elbl
        else
          _set  __albl,,__astk,%__inner_loop
          _j    cc,_m,%__albl
        endif
        endm

_until  macro   cc
        _set    __albl,,__astk,%__depth
        _jn     cc,_m,%__albl
        _set    __inner_loop,,__llbl,%__depth
        _endguess
        endm

_endloop macro
        _set    __albl,,__astk,%__depth
        _j      ,_m,%__albl
        _set    __inner_loop,,__llbl,%__depth
        _endguess
        endm

_if     macro   cc
        _guess
        _set    __albl,,__astk,%__depth
        _jn     cc,_l,%__albl
        endm

_else   macro
        _admit
        endm

_endif  macro
        _endguess
        endm

_set    macro   base1,ext1,base2,ext2
        base1&ext1 = base2&ext2
        endm

_label  macro   base,ext
base&ext:
        endm

_j      macro   cc,base,ext
        j&cc        base&ext
        endm

_jn     macro   cc,base,ext
        jn&cc       base&ext
        endm

jnna    macro   label
        ja      label
        endm

jnnae   macro   label
        jae     label
        endm

jnnb    macro   label
        jb      label
        endm

jnnbe   macro   label
        jbe     label
        endm

jnnc    macro   label
        jc      label
        endm

jnne    macro   label
        je      label
        endm

jnng    macro   label
        jg      label
        endm

jnnge   macro   label
        jge     label
        endm

jnnl    macro   label
        jl      label
        endm

jnnle   macro   label
        jle     label
        endm

jnno    macro   label
        jo      label
        endm

jnnp    macro   label
        jp      label
        endm

jnns    macro   label
        js      label
        endm

jnnz    macro   label
        jz      label
        endm

jnpe    macro   label
        jpo     label
        endm

jnpo    macro   label
        jpe     label
        endm

j       macro   label
        jmp     short label
        endm

jn      macro   label
        nop
        endm

_shl    macro   reg,count
        add     reg,reg
        endm

_rcl    macro   reg,count
        adc     reg,reg
        endm

        modstart  _386sqrt

;
;
;
        xdefp   __sqrtd

;
;      double __sqrtd( double EDX EAX );
;

        defp    __sqrtd
        push    EBX                     ; save EBX
        sub     ESP,12                  ; allocate space for long double
        mov     EBX,ESP                 ; point EBX to long double
        call    __FDLD                  ; convert double to long double
        mov     EAX,ESP                 ; point to long double
        call    __sqrt                  ; calculate square root
        mov     EAX,ESP                 ; point to long double
        call    __LDFD                  ; convert it to double
        add     ESP,12                  ; remove long double from stack
        pop     EBX                     ; restore EBX
        ret                             ; return
        endproc __sqrtd

        defp    __Exception_HNDLR
        ret                             ; return
        endproc __Exception_HNDLR

;include xception.inc
; status word fields
; exception bits (sticky) (nyi)
SW_IE           =       0001H
SW_DE           =       0002H
SW_ZE           =       0004H
SW_OE           =       0008H
SW_UE           =       0010H
SW_PE           =       0020H
;include fstatus.inc
FPE_OK                  equ     0
FPE_INVALID             equ     81h
FPE_DENORMAL            equ     82h
FPE_ZERODIVIDE          equ     83h
FPE_OVERFLOW            equ     84h
FPE_UNDERFLOW           equ     85h
FPE_INEXACT             equ     86h
FPE_UNEMULATED          equ     87h
FPE_SQRTNEG             equ     88h
FPE_STACKOVERFLOW       equ     8ah
FPE_STACKUNDERFLOW      equ     8bh
FPE_EXPLICITGEN         equ     8ch
FPE_IOVERFLOW           equ     8dh
;include 386sqrt.inc
;
;
        xdefp   __sqrt
;
;       __sqrt( long double *EAX );
;

        defp    __sqrt
        push    EDI             ; save EDI
        push    ESI             ; save ESI
        push    EDX             ; save EDX
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        mov     CX,8[EAX]       ; get exponent
        mov     EDX,4[EAX]      ; get operand
        mov     EBX,[EAX]       ; ...
        _guess                  ; guess: special number
          or    EBX,EBX         ; - quit if not zero
          _quif ne              ; - ...
          or    EDX,EDX         ; - if fraction all zero
          _if   e               ; - then
            _shl  CX,1          ; - - get rid of sign bit
            je    sqrt9         ; - - answer is 0 if exponent is 0
indefinite: mov   word ptr 8[EAX],0FFFFh ; - - set result to indefinite
            mov   dword ptr 4[EAX],0    ; - - ...
            mov   dword ptr [EAX],0     ; - - ...
sq_NaN:     or    byte ptr 7[EAX],0C0h  ; - - set result to NaN
            jmp   sqrt9         ; - - return
          _endif                ; - endif
          cmp   CX,7FFFh        ; - if +ve infinity
          je    sqrt9           ; - answer is +ve infinity if arg is +vf inf.
        _endguess               ; endguess
        _guess                  ; guess: NaN
          mov   EDX,ECX         ; - get exponent
          and   DH,07Fh         ; - get rid of sign bit
          cmp   DX,07FFFh       ; - check for NaN
          je    sq_NaN          ; - result is a NaN
          _shl  CX,1            ; - get rid of sign bit
          jc    sq_NaN          ; - sqrt(-ve) = -ve
          shr   CX,1            ; - restore sign
          _quif e               ; - quit if number is denormal
          mov   EDX,4[EAX]      ; - get top part of fraction
          _shl  EDX,1           ; - top bit should be on
          jnc   indefinite      ; - number is an unnormal
        _endguess               ; endguess
        mov     EDX,4[EAX]      ; get operand
        sub     EDI,EDI         ; zero guard bits
        push    EAX             ; save address of operand
        sub     CX,3FFFh        ; remove bias
        sar     CX,1            ; divide by 2
        _if     nc              ; if exponent is even
          shr   EDX,1           ; - divide argument by 2
          rcr   EBX,1           ; - ...
          rcr   EDI,1           ; - save guard bit
        _endif                  ; endif
        add     CX,3FFFh        ; add bias back in
        mov     8[EAX],CX       ; store exponent
        mov     ECX,EDX         ; save operand
        mov     EAX,EBX         ; ...
        mov     ESI,EDX         ; get high order word
        stc                     ; calculate initial estimate
        rcr     ESI,1           ; ...
        inc     EDX             ; check for EDX=FFFFFFFFh
        _if     ne              ; if not -1, then
          dec   EDX             ; - restore EDX
          _loop                 ; - loop
            div   ESI           ; - - calculate newer estimate
            dec   ESI           ; - - want estimate to be within one
            cmp   ESI,EAX       ; - -
            _quif na            ; - - quit if estimate good enough
            inc   ESI           ; - -
            add   ESI,EAX       ; - - calculate new estimate as (old+new)/2
            rcr   ESI,1         ; - - ...
            mov   EDX,ECX       ; - - restore operand
            mov   EAX,EBX       ; - - ...
          _endloop              ; - endloop
          inc   ESI             ; - restore divisor
          mov   ECX,EAX         ; - save word of quotient
          mov   EAX,EDI         ; - get guard bit
          div   ESI             ; - calculate next word of quotient
;
;       ESI:0   estimate is too small
;       ECX:EAX estimate is too large
;       calculate new estimate as (ESI:0+ECX:EAX)/2
;
          add   ESI,ECX         ; - ...
        _else                   ; else (high word was -1)
          cmp   EAX,ESI         ; - if low word not -1
          je    short sqrt8     ; - then
          xchg  EAX,EDX         ; - flip around
          mov   EAX,EDI         ; - get guard bit
          div   ESI             ; - calculate value for last 32 bits
        _endif                  ; endif
        sub     EDX,EDX         ; zero EDX
        stc                     ; divide by 2
        rcr     ESI,1           ; ...
        rcr     EAX,1           ; ...
        adc     EAX,EDX         ; round up (EDX=0)
sqrt8:  adc     EDX,ESI         ; ...

        pop     ESI             ; restore address of operand
        mov     [ESI],EAX       ; store result
        mov     4[ESI],EDX      ; ...
sqrt9:  pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        pop     EDX             ; restore EDX
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return to caller
        endproc __sqrt
;include 386fdld.inc
; ====          ==              ======
        xdefp   __FDLD

;       convert double to long double
; input:
;       EDX:EAX double
;       EBX     pointer to long double to be filled in

__FDLD  proc    near
        push    ECX                     ; save ECX
        mov     ECX,EDX                 ; get exponent and sign
        shld    EDX,EAX,11              ; shift fraction left 11 bits
        shl     EAX,11                  ; ...
        _guess                          ; guess: a normal number
          sar   ECX,32-12               ; - shift exponent to bottom
          and   CX,07FFh                ; - isolate exponent
          _quif e                       ; - quit if denormal number
          _guess                        ; - guess: normal number
            cmp   CX,07FFh              ; - - quit if infinity or NaN
            _quif e                     ; - - ...
            add   CX,3FFFh-03FFh        ; - - change bias to temp real format
          _admit                        ; - guess: NaN
            mov   CX,7FFFh              ; - - set exponent for infinity or NaN
            test  EDX,7FFFFFFFh         ; - - check for infinity
            _if   e                     ; - - if top part is 0
              or    EAX,EAX             ; - - - check low word
            _endif                      ; - - endif
            _quif e                     ; - - quit if infinity
            push  EAX                   ; - - save EAX
            mov   AL,SW_IE              ; - - indicate "Invalid" exception
            mov   AH,FPE_INVALID        ; - - ...
            call  __Exception_HNDLR     ; - - ...
            pop   EAX                   ; - - restore EAX
            or    EDX,40000000h         ; - - indicate NaN
          _endguess                     ; - endguess
          or    EDX,80000000h           ; - turn on implied 1 bit
        _admit                          ; guess: zero
          or    EDX,EDX                 ; - quit if non-zero
          _quif ne                      ; - ...
          or    EAX,EAX                 ; - ...
          _quif ne                      ; - ...
          sub   ECX,ECX                 ; - set exponent and sign to 0
        _admit                          ; admit: denormal number
          mov   CX,3C01h                ; - set exponent
          or    EDX,EDX                 ; - if high word is zero
          _if   e                       ; - then
            xchg  EAX,EDX               ; - - shift number left 32 bits
            sub   CX,32                 ; - - adjust exponent
          _endif                        ; - endif
          _loop                         ; - loop (normalize number)
            or    EDX,EDX               ; - - quit if top bit is on
            _quif s                     ; - - ...
            _shl  EAX,1                 ; - - shift number left 1 bit
            _rcl  EDX,1                 ; - - ...
            dec   CX                    ; - - decrement exponent
          _endloop                      ; - endloop
        _endguess                       ; endguess
        mov     [EBX],EAX               ; store number
        mov     4[EBX],EDX              ; ...
        _shl    ECX,1                   ; get sign
        rcr     CX,1                    ; place in top bit
        mov     8[EBX],CX               ; ...
        pop     ECX                     ; restore ECX
        ret                             ; return
__FDLD  endp
;include 386ldfd.inc
;
; =========     ==              ======
;                               call exception_hndlr on overflow
        xdefp   __LDFD

;       convert long double to double
; input:
;       EAX - pointer to long double
; output:
;       EDX:EAX - double
;
__LDFD  proc    near
        push    ECX                     ; save ECX
        push    EBX                     ; save EBX
        push    ESI                     ; save ESI
        mov     CX,8[EAX]               ; get exponent and sign
        mov     EDX,4[EAX]              ; get fraction
        mov     EAX,[EAX]               ; ...
        mov     ESI,0FFFFF800h          ; get mask of bits to keep
        mov     EBX,EAX                 ; get bottom part
        shl     EBX,22                  ; get rounding bit
        _if     c                       ; if have to round
          _if   e                       ; - if half way between
            _shl  ESI,1                 ; - - adjust mask
          _endif                        ; - endif
          add   EAX,0800h               ; - round up
          adc   EDX,0                   ; - ...
          _if   c                       ; - if exponent needs adjusting
            mov   EDX,80000000h         ; - - set fraction
            inc   CX                    ; - - increment exponent
            ;  check for overflow
          _endif                        ; - endif
        _endif                          ; endif
        and     EAX,ESI                 ; mask off bottom bits
        mov     EBX,ECX                 ; save exponent and sign
        and     CX,7FFFh                ; if number not 0
        add     CX,03FFh-3FFFh          ; change bias to double format
        _guess                          ; guess: number not too large or -ve
          cmp   CX,07FFh                ; - quit if too large or -ve
          _quif ae                      ; - ...
          _guess                        ; - guess: DENORMAL
            or    CX,CX                 ; - - quit if normal
            _quif ne                    ; - - ...
            shrd  EAX,EDX,12            ; - - drop 12 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shr   EDX,12                ; - - shift right 12
          _admit                        ; - admit: NORMAL
            shrd  EAX,EDX,11            ; - - drop 11 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shrd  EDX,ECX,11            ; - - copy exponent into high part
          _endguess                     ; - endguess
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
        _admit                          ; guess: underflow or denormal
          cmp   CX,0C400h               ; - quit if overflow
          _quif b                       ; - ...
          cmp   CX,-52                  ; - if in the denormal range
          _if   ge                      ; - then
            sub   CX,12                 ; - - adjust exponent for 12 more shifts
            neg   CX                    ; - - negate shift count
            cmp   CL,32                 ; - - if at least 32 bits
            _if   ae                    ; - - then
              sub   CL,32               ; - - - adjust shift count
              mov   ESI,EAX             ; - - - shift right 32
              mov   EAX,EDX             ; - - - ...
              sub   EDX,EDX             ; - - - ...
            _endif                      ; - - endif
            shrd  ESI,EAX,CL            ; - - get the bits from the bottom
            shrd  EAX,EDX,CL            ; - - shift right
            shr   EDX,CL                ; - - ...
            add   ESI,ESI               ; - - round up
            adc   EAX,0                 ; - - ...
            adc   EDX,0                 ; - - ...
          _else                         ; - else
            sub   EAX,EAX               ; - - set result to 0
            sub   EDX,EDX               ; - - ...
          _endif                        ; - endif
        _admit                          ; admit: OVERFLOW
          shrd  EAX,EDX,11              ; - drop 11 bits off the bottom
          _shl  EDX,1                   ; - get rid of implied 1 bit
          shr   EDX,11                  ; - copy exponent into high part
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
          or    EDX,7FF00000h           ; - set exponent
          cmp   CX,43FFh                ; - quit if infinity or NaN
          _quif e                       ; - ...
          push  EAX                     ; - save EAX
          mov   AL,SW_OE                ; - get OVERFLOW mask
          mov   AH,FPE_OVERFLOW         ; - set OVERFLOW exception code
          call  __Exception_HNDLR       ; - set OVERFLOW exception
          pop   EAX                     ; - restore EAX
        _endguess                       ; endguess
        pop     ESI                     ; restore ESI
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        ret                             ; return
__LDFD  endp

        endmod
        end
