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
 include mdef.inc
 .387
else
 include mdef.inc
 .8087
endif
include struct.inc
include math387.inc

    if __WASM__ ge 100
        xref    "C",__math87_err
    else
        xref    <"C",__math87_err>
    endif
        xref    __8087  ; indicate that NDP instructions are present

        modstart    sqrt387

    if __WASM__ ge 100
        xdefp   "C",sqrt        ; calc sqrt(fac1)
    else
        xdefp   <"C",sqrt>      ; calc sqrt(fac1)
    endif

        defp    sqrt
ifdef __386__
        _guess                          ; guess: number is negative
          test  byte ptr 4+7[ESP],80h   ; - quit if argument is positive
          _quif e                       ; - ...
          mov   EAX,4+4[ESP]            ; - get high word
          and   EAX,7FFFFFFFh           ; - get rid of sign bit
          or    EAX,4[ESP]              ; - or in lower word
          _quif e                       ; - quit if -0.0
          push  FUNC_SQRT               ; - indicate "sqrt"
          push  8+4[ESP]                ; - push argument
          push  8+4[ESP]                ; - ...
          call  __math87_err            ; - error
 ifdef __STACK__
          add   ESP,8+4                 ; - remove arguments
 endif
        _admit                          ; admit: number is +ve or -0.0
          fld   qword ptr 4[ESP]        ; - load argument x
          fsqrt                         ; - calculate sqrt root
          loadres                       ; - load result
        _endguess                       ; endguess
else
        if _MODEL and _BIG_CODE
         argx    equ     6
        else
         argx    equ     4
        endif

        prolog
        _guess                          ; guess: number is negative
          test  byte ptr argx+7[BP],80h ; - quit if argument is positive
          _quif e                       ; - ...
          mov   AX,argx+6[BP]           ; - get high word
          and   AX,7FFFh                ; - get rid of sign bit
          or    AX,argx+4[BP]           ; - check for -0.0
          or    AX,argx+2[BP]           ; - ...
          or    AX,argx+0[BP]           ; - ...
          _quif e                       ; - quit if -0.0
          mov   AX,FUNC_SQRT            ; - indicate "sqrt"
          push  AX                      ; - ...
          push  argx+6[BP]              ; - push argument
          push  argx+4[BP]              ; - ...
          push  argx+2[BP]              ; - ...
          push  argx+0[BP]              ; - ...
          call  __math87_err            ; - error
        _admit                          ; admit: +ve or -0.0
          fld   qword ptr argx[BP]      ; - load argument
          fsqrt                         ; - calculate sqrt root
        _endguess                       ; endguess
        epilog
endif
        ret_pop 8                       ; return
        endproc sqrt


        public  IF@SQRT
        public  IF@DSQRT
        defp    IF@DSQRT
        defp    IF@SQRT
        mov     AL,FUNC_SQRT            ; indicate sqrt function

        public  __@DSQRT
        defp    __@DSQRT
        prolog
        ftst                            ; test sign of argument
        sub     _SP,16                  ; allocate space
        fstsw   word ptr -16+8[_BP]     ; get status
        fwait                           ; wait for it
        xchg    AX,-16+8[_BP]           ; save func code and get status word
        sahf                            ; set flags
        _if     b                       ; if number is negative
          fstp  qword ptr -16+0[_BP]    ; - push argument on stack
          mov   _AX,-16+8[_BP]          ; - load function code
          cmp   AL,FUNC_SQRT            ; - if not "sqrt"
          _if   ne                      ; - then
            fstp  qword ptr -16+0[_BP]  ; - - get the argument for "asin","acos"
                                        ; - - 8087 stack now in proper state
          _endif                        ; - endif
          mov   -16+12[_BP],_DX         ; - save DX (-3s) clobbers EDX 17-mar-92
          call  __math87_err            ; - error (func code in AL)
ifdef __386__
 ifdef __STACK__
          push  EDX                     ; - load result into 8087
          push  EAX                     ; - ...
          fld   qword ptr 0[ESP]        ; - ...
          mov   EDX,-16+12[EBP]         ; - restore EDX 17-mar-92
          fwait                         ; - ...
 endif
endif
          mov   AL,1                    ; - indicate error
        _admit                          ; admit: +ve or -0.0
          fsqrt                         ; - calculate sqrt using 8087
          mov   AL,0                    ; - indicate no error
        _endguess                       ; endguess
        mov     _SP,_BP                 ; reset stack pointer
        epilog
        ret                             ; return
        endproc __@DSQRT
        endproc IF@SQRT
        endproc IF@DSQRT


        endmod
        end
