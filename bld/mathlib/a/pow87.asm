;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


;
;   double pow( double x, double y )
;
ifdef __386__
 .387
else
 .8087
endif
include mdef.inc
include struct.inc
include math87.inc

        extern_chipbug
        xref    __8087  ; indicate that NDP instructions are present

        modstart    pow87

        xref    "C",__pow87_err
        xref    __@DEXP

        xdefp   "C",pow ; calc pow(x,y)

ifndef __386__
        if _MODEL and _BIG_CODE
         argx    equ     6
        else
         argx    equ     4
        endif
endif


        public  IF@DPOW
        public  IF@POW
        defp    IF@DPOW
        defp    IF@POW
ifndef __386__
        local   tmp:QWORD,func:WORD,datay:QWORD,datax:QWORD
elseifdef __STACK__
        local   tmp:QWORD,sedx:DWORD,secx:DWORD,func:DWORD,datay:QWORD,datax:QWORD
else
        local   tmp:QWORD,func:DWORD,datay:QWORD,datax:QWORD
endif
        ftst                            ; test sign of x
        fstsw   word ptr func           ; get status word
        fst     qword ptr datax         ; save x
        mov     AH,byte ptr func+1      ; ...
        sahf                            ; set flags
        _guess                          ; guess: x is 0.0
          _quif ne                      ; - quit if not 0.0
          mov   AL,0                    ; - indicate x==0.0 error
pow_err1: fstp  st(0)                   ; - clean up 80x87 stack
pow_err:  fstp  qword ptr datay         ; - store y
          mov   func,_AX                ; - push code
ifdef __STACK__
          mov   sedx,EDX                ; - save EDX (-3s)
          mov   secx,ECX                ; - save ECX (-3s)
          call  __pow87_err             ; - pow error
          push  EDX                     ; - load result into 80x87
          push  EAX                     ; - ...
          fld   qword ptr 0[ESP]        ; - ...
          mov   ECX,secx                ; - restore ECX (-3s)
          mov   EDX,sedx                ; - restore EDX (-3s)
          fwait                         ; - ...
else
          call  __pow87_err             ; - pow error
endif
          jmp   pow_end                 ; - done
        _endguess                       ; endguess
        _guess                          ; admit: base not 0
          fld   st(1)                   ; - get exponent y
          frndint                       ; - determine if it is integral
          fcomp st(2)                   ; - if int(y) != y
          fstsw word ptr tmp            ; - get status word
          fwait                         ; - ...
          mov   AH,byte ptr tmp+1       ; - ...
          _if   c                       ; - if base is negative
            mov   AL,1                  ; - - indicate negative ** fraction
            sahf                        ; - - set flags
            jne   pow_err1              ; - - if int(y) != y, then error

;           need to determine if y is even or odd

            mov   AX,2                  ; - - load 2.0
            mov   word ptr tmp,AX       ; - - ...
            fild  word ptr tmp          ; - - ...
            fld   st(2)                 ; - - load y
            fprem                       ; - - calc fprem(y,2)
            fstsw word ptr tmp          ; - - get status word
            fwait                       ; - - ...
            mov   AH,byte ptr tmp+1     ; - - see if exponent > 64
            sahf                        ; - - set flags
            mov   AH,0                  ; - - assume exponent even
            _if   np                    ; - - if exponent < 64
              ftst                      ; - - - test remainder to see if 0
              fstsw word ptr tmp        ; - - - get status word
              fwait                     ; - - - ...
              mov   AH,byte ptr tmp+1   ; - - - see if rem is 0
              sahf                      ; - - - set flags
              mov   AH,0                ; - - - assume even
              _if   ne                  ; - - - if remainder not 0
                mov   AH,1              ; - - - - indicate exponent was odd
              _endif                    ; - - - endif
            _endif                      ; - - endif
            mov   byte ptr func+1,AH    ; - - save as flags (carry = low bit)
            fstp  st(0)                 ; - - clean up stack
            fstp  st(0)                 ; - - clean up stack
          _else                         ; - else base is positive
            sahf                        ; - - set flags
            _if ne
            jmp   exp_log               ; - - quit if int(y) != y
            _endif
          _endif                        ; - endif
          fld   st(1)                   ; - get y
          fstp  qword ptr datay         ; - store in memory
          fwait                         ; - wait
          mov   AX,word ptr datay+6     ; - get exponent
          and   AX,7FF0h                ; - get rid of sign bit
          sub   AX,3FF0h                ; - get rid of bias
          cmp   AX,16 shl 4             ; - quit if number too large
          _quif ae                      ; - ...
          fld   st(1)                   ; - get y
          fistp dword ptr tmp           ; - store exponent as an integer
          fwait                         ; - wait for fistp to complete
          mov   AX,word ptr tmp+2       ; - get high order word of exponent
          or    AX,AX                   ; - if 0
          _if   e                       ; - then
            mov   AX,word ptr tmp       ; - - get low order word of exponent
            call  pow_ri                ; - - calculate power
          _else                         ; - else
            inc   AX                    ; - - see if high order word is -1
            jne   exp_log               ; - - quit if not -1
            or    AX,word ptr tmp       ; - - get low order word
            je    exp_log               ; - - quit if 0
            neg   AX                    ; - - negate the exponent
            call  pow_ri                ; - - calculate power
            fld1                        ; - - take reciprical
            do_fdivrp 1,0               ; - - take reciprical
          _endif                        ; - endif
          fst   qword ptr tmp           ; - store result
          fwait                         ; - wait
          mov   AX,word ptr tmp         ; - check for infinity
          or    AX,word ptr tmp+2       ; - ...
          or    AX,word ptr tmp+4       ; - ...
          _if   e                       ; - if fraction is 0
            mov   AX,word ptr tmp+6     ; - - get exponent
            shl   AX,1                  ; - - get rid of sign bit
            cmp   AX,0FFE0h             ; - - if infinity
            _if   e                     ; - - then
              mov   AL,2                ; - - - indicate value too large
              jmp   pow_err1            ; - - - handle error
            _endif                      ; - - endif
          _endif                        ; - endif
          fstp  st(1)                   ; - copy answer over top of y
        _admit                          ; admit: use exp( log(x) * y )
exp_log:  fldln2                        ; - load ln(2)
          fmul    st,st(2)              ; - calc. y * ln(2)
          fxch    st(1)                 ; - flip
          fabs                          ; - make sure x is positive
          fyl2x                         ; - calc log(x) * y
          mov     AL,FUNC_POW           ; - indicate pow function
          call    __@DEXP               ; - calc. exp( log(x) * y )
          cmp     AL,0                  ; - check for error
          jne     pow_err2              ; - if argument to exp was too large
          mov     AH,byte ptr func+1    ; - get flags
          sahf                          ; - set flags
          _if     c                     ; - if x was negative
            fchs                        ; - - negate the answer
          _endif                        ; - endif
          fstp  st(1)                   ; - copy answer over top of y
        _endguess                       ; endguess
pow_end:
        fstp    qword ptr tmp           ; force value to be double 30-jul-92
        fld     qword ptr tmp           ; reload value
        fwait                           ; wait for load to complete
        ret                             ; return

pow_err2:
        mov     AL,2                    ; indicate value too large
        fxch    st(1)                   ; pass parm to __@DEXP to __pow87_err
        jmp     pow_err1                ; handle error

        endproc IF@POW
        endproc IF@DPOW

;
;       st(0) contains base
;       AX contains integer exponent
;
;       calculate st(0) ** AX

pow_ri  proc    near
        _loop                           ; loop
          shr   AX,1                    ; - shift exponent right 1
          _quif be                      ; - quit if exponent is 0 or carry set
          fmul  st,st(0)                ; - square the base
        _endloop                        ; endloop
        _if     c                       ; if carry on
          fld   st(0)                   ; - this is first term of result
          _loop                         ; - loop
            _quif  e                    ; - - quit if done
            fmul   st,st(0)             ; - - square the base
            shr    AX,1                 ; - - shift exponent right 1
            _if    c                    ; - - if bit is on
              fmul   st(1),st           ; - - - multiply result by base
            _endif                      ; - - endif
          _endloop                      ; - endloop
          fstp  st(0)                   ; - throw away base
        _else                           ; else (exponent was 0)
          fstp  st(0)                   ; - throw away base
          fld1                          ; - set result to 1
        _endif                          ; endif
        ret                             ; return
        endproc pow_ri

        defp    pow
ifdef __386__
        fld     qword ptr 4+8[ESP]      ; load y
        fld     qword ptr 4[ESP]        ; load x
        call    IF@DPOW                 ; calculate pow(x,y)
        loadres                         ; load result
else
        prolog
        fld     qword ptr argx+8[BP]    ; load y
        fld     qword ptr argx[BP]      ; load x
        lcall   IF@DPOW                 ; calculate pow(x,y)
        epilog
endif
        ret_pop 16                      ; return
        endproc pow

        endmod
        end
