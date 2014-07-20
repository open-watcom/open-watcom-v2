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

xrefp           macro   dsym
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
            _shl  CX,1          ; - - get rid of sign bit
          _shl  CX,1            ; - get rid of sign bit
          shr   CX,1            ; - restore sign
          _shl  EDX,1           ; - top bit should be on
        sar     CX,1            ; divide by 2
          shr   EDX,1           ; - divide argument by 2
          rcr   EBX,1           ; - ...
          rcr   EDI,1           ; - save guard bit
        rcr     ESI,1           ; ...
            rcr   ESI,1         ; - - ...
        rcr     ESI,1           ; ...
        rcr     EAX,1           ; ...
        ret                     ; return to caller
        endproc __sqrt

	xdefp   __FDLD

__FDLD  proc    near
        shld    EDX,EAX,11              ; shift fraction left 11 bits
        shl     EAX,11                  ; ...
          sar   ECX,32-12               ; - shift exponent to bottom
            _shl  EAX,1                 ; - - shift number left 1 bit
            _rcl  EDX,1                 ; - - ...
        _shl    ECX,1                   ; get sign
        rcr     CX,1                    ; place in top bit
        ret                             ; return
__FDLD  endp

        xdefp   __LDFD

__LDFD  proc    near
        shl     EBX,22                  ; get rounding bit
            shrd  EAX,EDX,12            ; - - drop 12 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shr   EDX,12                ; - - shift right 12
            shrd  EAX,EDX,11            ; - - drop 11 bits off the bottom
            _shl  EDX,1                 ; - - get rid of implied 1 bit
            shrd  EDX,ECX,11            ; - - copy exponent into high part
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
            shrd  ESI,EAX,CL            ; - - get the bits from the bottom
            shrd  EAX,EDX,CL            ; - - shift right
            shr   EDX,CL                ; - - ...
          shrd  EAX,EDX,11              ; - drop 11 bits off the bottom
          _shl  EDX,1                   ; - get rid of implied 1 bit
          shr   EDX,11                  ; - copy exponent into high part
          _shl  BX,1                    ; - get sign
          rcr   EDX,1                   ; - shift into result
        ret                             ; return
__LDFD  endp

        endmod
        end
