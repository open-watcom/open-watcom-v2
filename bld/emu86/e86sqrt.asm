include mdef.inc
ifdef __WASM__
 include struct.inc
else
 include struct3.inc
endif

        modstart  e86sqrt

        xdefp   __sqrtd

;
;      double __sqrtd( double AX BX CX DX );
;
        defp    __sqrtd
        push    SI                      ; save SI
        push    DS                      ; save DS
        sub     SP,10                   ; allocate space for long double
        mov     SI,SS                   ; set DS=SS
        mov     DS,SI                   ; ...
        mov     SI,SP                   ; point DS:SI to long double
        call    __EmuFDLD               ; convert double to long double
        mov     AX,SP                   ; point to long double
        call    __sqrt                  ; calculate square root
        mov     BX,SP                   ; point to long double
        call    __EmuLDFD               ; convert it to double
        add     SP,10                   ; remove long double from stack
        pop     DS                      ; restore DS
        pop     SI                      ; restore SI
        ret                             ; return
        endproc __sqrtd

include e86sqrt.inc
include xception.inc
include fstatus.inc

        xref    F8InvalidOp
        xref    F8OverFlow

include e86ldfd.inc
include e86fdld.inc

        endmod
        end
