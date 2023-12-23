;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


include mdef.inc
include struct.inc
include math87.inc

        xrefp   __8087  ; indicate that NDP instructions are present

        modstart    sqrt87

        xrefp   "C",__math87_err

        xdefp   "C",sqrt        ; calc sqrt(fac1)

        defp    sqrt
ifdef __386__
        _guess                          ; guess: number is negative
          test  byte ptr argx+7[ESP],80h; - quit if argument is positive
          _quif e                       ; - ...
          mov   EAX,argx+4[ESP]         ; - get high word
          and   EAX,7FFFFFFFh           ; - get rid of sign bit
          or    EAX,argx[ESP]           ; - or in lower word
          _quif e                       ; - quit if -0.0
 ifdef __STACK__
          push  ECX
          push  FP_FUNC_SQRT            ; - indicate "sqrt"
          push  argx+8+4[ESP]           ; - push argument
          push  argx+8+4[ESP]           ; - ...
          call  __math87_err            ; - math error
          add   ESP,3*4                 ; - remove arguments
          pop   ECX
 else
          push  FP_FUNC_SQRT            ; - indicate "sqrt"
          push  argx+8[ESP]             ; - push argument
          push  argx+8[ESP]             ; - ...
          call  __math87_err            ; - math error
 endif
        _admit                          ; admit: number is +ve or -0.0
          fld   qword ptr argx[ESP]     ; - load argument x
          fsqrt                         ; - calculate sqrt root
          loadres                       ; - load result
        _endguess                       ; endguess
else
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
          mov   AX,FP_FUNC_SQRT         ; - indicate "sqrt"
          push  AX                      ; - ...
          push  argx+6[BP]              ; - push argument
          push  argx+4[BP]              ; - ...
          push  argx+2[BP]              ; - ...
          push  argx+0[BP]              ; - ...
          call  __math87_err            ; - math error
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
        mov     AL,FP_FUNC_SQRT         ; indicate sqrt function

        public  __@DSQRT
        defp    __@DSQRT

ifdef __386__
 ifdef __STACK__
        local   sedx:DWORD,secx:DWORD
 endif
endif
ifdef __386__
        local   func:DWORD,data:QWORD
else
        local   func:WORD,data:QWORD
endif

        ftst                            ; test sign of argument
        fstsw   word ptr func           ; get status
        fwait                           ; wait for it
        mov     AH,byte ptr func+1      ; save func code and get status word
        sahf                            ; set flags
        _if     b                       ; if number is negative
          fstp  qword ptr data          ; - push argument on stack
          cmp   AL,FP_FUNC_SQRT         ; - if not "sqrt"
          _if   ne                      ; - then
            fstp  qword ptr data        ; - - get the argument for "asin","acos"
                                        ; - - 8087 stack now in proper state
          _endif                        ; - endif
          mov   func,_AX                ; - load function code
ifdef __STACK__
          mov   sedx,EDX                ; - save EDX (-3s)
          mov   secx,ECX                ; - save ECX (-3s)
          call  __math87_err            ; - math error
          push  EDX                     ; - load result into 8087
          push  EAX                     ; - ...
          fld   qword ptr 0[ESP]        ; - ...
          mov   ECX,secx                ; - restore ECX (-3s)
          mov   EDX,sedx                ; - restore EDX (-3s)
          fwait                         ; - ...
else
          call  __math87_err            ; - math error
endif
          mov   AL,1                    ; - indicate error
        _admit                          ; admit: +ve or -0.0
          fsqrt                         ; - calculate sqrt using 8087
          xor   AL,AL                   ; - indicate no error
        _endguess                       ; endguess
        ret                             ; return
        endproc __@DSQRT
        endproc IF@SQRT
        endproc IF@DSQRT

        endmod
        end
