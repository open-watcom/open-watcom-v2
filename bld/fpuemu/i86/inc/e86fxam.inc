
        modstart  e86fxam

        xdefp   __fxam          ; examine stack top

;
;       these equates match the condition code bits in the status word
;
XAM_UNNORMAL    equ     00h
XAM_NAN         equ     01h
XAM_NORMAL      equ     04h
XAM_INFINITY    equ     05h
XAM_ZERO        equ     40h
XAM_EMPTY       equ     41h
XAM_DENORMAL    equ     44h

;
;       input:  AX - pointer to long double operand
;
        defp    __fxam
        push    BX                      ; save BX
        push    DX                      ; save DX
        mov     BX,AX                   ; get pointer to top of stack
        mov     AX,CS:tagtab[DI].t_empty; get tag word mask for top of stack
        mov     DX,AX                   ; ...
        and     AX,DS:[BP].tags         ; ...
        _guess                          ; guess: EMPTY
          cmp   AX,DX                   ; - quit if not empty
          _quif ne                      ; - ...
          mov   AL,XAM_EMPTY            ; - indicate empty
          mov   DH,0                    ; - set sign to +ve
        _admit                          ; guess: DENORMAL or ZERO
          not   DX                      ; - set tag entry to VALID
          and   DS:[BP].tags,DX         ; - ...
          mov   AX,8[BX]                ; - get exponent and sign
          mov   DX,AX                   ; - DH = 0 for +ve, 02h for -ve
          and   DH,80h                  ; - isolate sign bit
          rol   DH,1                    ; - shift into position
          shl   DH,1                    ; - ...
          and   AX,7FFFh                ; - isolate exponent
          _quif ne                      ; - quit if not 0
          mov   AX,6[BX]                ; - check fraction to see if 0
          or    AX,4[BX]                ; - ...
          or    AX,2[BX]                ; - ...
          or    AX,[BX]                 ; - ...
          mov   AL,XAM_DENORMAL         ; - assume DENORMAL number
          _if   e                       ; - if fraction is 0
            mov   AX,CS:tagtab[DI].t_zero;- - get tag word mask for a zero
            or    DS:[BP].tags,AX       ; - - tag number as a zero
            mov   AL,XAM_ZERO           ; - - indicate number is ZERO
          _endif                        ; - endif
        _admit                          ; guess: NAN or INFINITY
          cmp   AX,7FFFh                ; - quit if not NAN or INFINITY
          _quif ne                      ; - ...
          mov   AX,CS:tagtab[DI].t_nan  ; - get tag word mask for a nan
          or    DS:[BP].tags,AX         ; - tag number as a nan
          mov   AX,4[BX]                ; - check fraction to see if 0
          or    AX,2[BX]                ; - ...
          or    AX,[BX]                 ; - ...
          _if   e                       ; - if low order 3 words are zero
            mov   AX,6[BX]              ; - - get high order word of fraction
            cmp   AX,8000h              ; - - check for infinity
          _endif                        ; - endif
          mov   AL,XAM_NAN              ; - assume NAN number
          _if   e                       ; - if fraction is 8000 0000 0000 0000
            mov   AL,XAM_INFINITY       ; - - indicate number is INFINITY
          _endif                        ; - endif
        _admit                          ; admit: NORMAL or UNNORMAL
          test  byte ptr 7[BX],80h      ; - check implied 1 bit
          mov   AL,XAM_NORMAL           ; - assume NORMAL number
          _if   e                       ; - if implied 1 bit is not on
            mov   AX,CS:tagtab[DI].t_nan; - - get tag word mask for a nan
            or    DS:[BP].tags,AX       ; - - tag number as a nan
            mov   AL,XAM_DENORMAL       ; - - indicate DENORMAL number
          _endif                        ; - endif
        _endguess                       ; endguess
        or      AL,DH                   ; merge in sign bit
        mov     DX,DS:[BP].status       ; get status word
        and     DX,NOT SW_CC            ; turn off C0,C1,C2,C3
        or      DH,AL                   ; turn on new bits
        mov     DS:[BP].status,DX       ; save in status word
        pop     DX                      ; restore DX
        pop     BX                      ; restore BX
        ret                             ; return
__fxam  endp

