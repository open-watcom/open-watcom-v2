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

        xrefp   __8087  ; indicate that NDP instructions are present

        modstart    acosh87

        xrefp   "C",__log87_err

        xdefp   "C",acosh       ; double acosh( double x )
;
;       acosh(x) = log(x + sqrt(x*x - 1.0)));
;
        public  IF@DACOSH       ; double acosh( double x )
        public  IF@ACOSH        ; double acosh( double x )
        defp    IF@DACOSH
        defp    IF@ACOSH
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        fld1                        ; get 1.0
        fcomp                       ; compare against 1.0
        fstsw   word ptr func       ; get status word
        fwait                       ; ...
        mov     AH,byte ptr func+1  ; ...
        sahf                        ; set flags
        _if     a                   ; if 1.0 > x
          fstp  qword ptr data      ; - push argument on stack
          mov   AL,FP_FUNC_ACOSH    ; - indicate "acosh"
          mov   func,_AX            ; - push code
ifdef __STACK__
          mov   sedx,EDX            ; - save EDX (-3s)
          mov   secx,ECX            ; - save ECX (-3s)
          call  __log87_err         ; - log error
          push  EDX                 ; - load result into 8087
          push  EAX                 ; - ...
          fld   qword ptr 0[ESP]    ; - ...
          mov   ECX,secx            ; - restore ECX (-3s)
          mov   EDX,sedx            ; - restore EDX (-3s)
          fwait                     ; - ...
else
          call  __log87_err         ; - log error
endif
        _else                       ; else
          fld   st(0)               ; - duplicate x
          fmul  st(0),st            ; - calc. x*x
          fld1                      ; - 1.0
          fsubp st(1),st            ; - x*x - 1.0
          fsqrt                     ; - sqrt( x*x - 1.0 )
          faddp st(1),st            ; - x + sqrt( x*x - 1.0 )
          fldln2                    ; - load ln(2)
          fxch  st(1)               ; - get arguments in right order
          fyl2x                     ; - calc. log( x + sqrt( x*x - 1.0 ) )
        _endif                      ; endif
        ret                         ; return
        endproc IF@ACOSH
        endproc IF@DACOSH


;  input:       x - on the stack
;  output:      acosh of x in st(0)
;
        defp    acosh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DACOSH       ; calculate acosh(x)
        loadres                 ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        prolog
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DACOSH       ; calculate acosh(x)
        epilog
endif
        ret_pop 8               ; return
        endproc acosh

        endmod
        end
