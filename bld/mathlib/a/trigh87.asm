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


ifdef __386__
 .387
else
 .8087
endif
include mdef.inc
include struct.inc
include math87.inc

        extern_chipbug
        xrefp           __8087  ; indicate that NDP instructions are present

        modstart        trigh87

ifdef __386__
    datasegment
endif
OneHalf dd      0.5
two2neg26 dw    0000h,03280h            ; 2.0**-26
ifdef __386__
    enddata
endif

        xrefp           __@DEXP
        xrefp           __@DEXP1
        xrefp           IF@DEXP

        xdefp   "C",sinh        ; calc sinh(fac1)
        xdefp   "C",cosh        ; calc cosh(fac1)
        xdefp   "C",tanh        ; calc tanh(fac1)

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
status  equ     -6
endif
;
;       sinh(x) = ( exp(x) - exp(-x) ) / 2.0
;          let z = exp(x)
;          then sinh(x) = ( z - 1/z ) / 2.0
;

        public  IF@DSINH        ; double sinh( double x )
        public  IF@SINH         ; double sinh( double x )
        defp    IF@DSINH
        defp    IF@SINH
        call    ChkMag          ; check magnitude of argument
        _if     nc              ; if argument is not small
          mov     AL,FUNC_SINH  ; - indicate "sinh" function
          call    __@DEXP       ; - calc. z = exp(x)
          cmp     AL,0          ; - if no overflow
          _if     e             ; - then
            fld1                ; - - calc. 1/z
            do_fdiv  0,1        ; - - ...
            fsubp st(1),st      ; - - calc. z - 1/z
            fmul  dword ptr OneHalf; - - multiply by .5 (or divide by 2)
          _endif                ; - endif
        _endif                  ; endif
        ret                     ; return
        endproc IF@SINH
        endproc IF@DSINH


;
;       cosh(x) = ( exp(x) + exp(-x) ) / 2.0
;          let z = exp(x)
;          then cosh(x) = ( z + 1/z ) / 2.0
;

        public  IF@DCOSH        ; double cosh( double x )
        public  IF@COSH         ; double cosh( double x )
        defp    IF@DCOSH
        defp    IF@COSH
        call    ChkMag          ; check magnitude of argument
        _if     nc              ; if argument is not small
          mov     AL,FUNC_COSH  ; - indicate "cosh" function
          call    __@DEXP       ; - calc. z = exp(x)
          cmp     AL,0          ; - if no overflow
          _if     e             ; - then
            fld1                ; - - calc. 1/z
            do_fdiv 0,1         ; - - ...
            faddp   st(1),st    ; - - calc. z + 1/z
            fmul    dword ptr OneHalf; - - multiply by .5 (or divide by 2)
          _endif                ; - endif
        _else                   ; else
          fstp  st(0)           ; - throw away argument
          fld1                  ; - set result to 1.0
        _endif                  ; endif
        ret                     ; return
        endproc IF@COSH
        endproc IF@DCOSH

;
;       sinh(x) = ( exp(x) - exp(-x) ) / 2.0
;       cosh(x) = ( exp(x) + exp(-x) ) / 2.0
;       tanh(x) = sinh(x) / cosh(x)
;          let z = exp(x)
;          then tanh(x) = ( z - 1/z ) / ( z + 1/z )
;

        public  IF@DTANH        ; double tanh( double x )
        public  IF@TANH         ; double tanh( double x )
        defp    IF@DTANH
        defp    IF@TANH
        fld     st(0)           ; compute |argument|
        fabs                    ; . . .
        push    _BP             ; save BP
        _guess                  ; guess argument too big
          mov   AX,64           ; - get upper bound of magnitude
          push  _AX             ; - allocate a temporary
          mov   _BP,_SP         ; - point to temporary
          ficom word ptr 0[_BP] ; - compare against upper limit
          fstsw word ptr 0[_BP] ; - get status word
          fwait                 ; - . . .
          pop   _AX             ; - . . .
          sahf                  ; - set condition codes
          _quif c               ; - quit if argument <= 64
          fstp  st(0)           ; - throw away duplicated argument
          sub   _SP,8           ; - allocate space
          mov   _BP,_SP         ; - point to temporary
          fstp  qword ptr 0[_BP]; - save operand
          fld1                  ; - return 1
          test  byte ptr 7[_BP],80h ; - check negative flag
          _if   ne              ; - if argument negative
            fchs                ; - - return -1.0
          _endif                ; - endif
          add   _SP,8           ; - remove temporary
          pop   _BP             ; - restore BP
          ret                   ; - return
        _endguess               ; endguess
        pop     _BP             ; restore BP

        call    ChkMag1         ; check magnitude of argument
        _if     nc              ; if argument is not small
          call    __@DEXP1      ; - calc. z = exp(x)
          fld     st(0)         ; - duplicate z
          fld1                  ; - calc. 1/z
          do_fdiv 0,1           ; - ...
          fsub    st(2),st      ; - calc. z - 1/z
          faddp   st(1),st      ; - calc. z + 1/z
          do_fdivp 1,0          ; - calc. ( z - 1/z ) / ( z + 1/z )
        _endif                  ; endif
        ret                     ; return
        endproc IF@TANH
        endproc IF@DTANH


ChkMag  proc    near            ; check magnitude of number on stack
        fld     st(0)           ; duplicate the number
        fabs                    ; get absolute value of number
ChkMag1:
        fcomp   dword ptr two2neg26  ; compare against lower limit
ifdef __386__
        fstsw   AX              ; get status word
else
        push    BP              ; save BP
        push    AX              ; allocate a temp
        mov     BP,SP           ; point at it
        fstsw   word ptr 0[BP]  ; get result of compare
        fwait                   ; wait for store to complete
        pop     AX              ; get flags
        pop     BP              ; restore BP
endif
        sahf                    ; set condition codes
        ret                     ; return
        endproc ChkMag


        defp    cosh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DCOSH        ; calculate cosh(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DCOSH        ; calculate cosh(x)
        pop     BP              ; restore BP
endif
        ret_pop 8               ; return
        endproc cosh


        defp    sinh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DSINH        ; calculate sinh(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DSINH        ; calculate sinh(x)
        pop     BP              ; restore BP
endif
        ret_pop 8               ; return
        endproc sinh


        defp    tanh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DTANH        ; calculate tanh(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DTANH        ; calculate tanh(x)
        pop     BP              ; restore BP
endif
        ret_pop 8               ; return
        endproc tanh


        endmod
        end
