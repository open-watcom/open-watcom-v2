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

        xref            __8087  ; indicate that NDP instructions are present

        modstart        asinh387

    if __WASM__ ge 100
        xdefp   "C",asinh       ; double asinh( double x )
    else
        xdefp   <"C",asinh>     ; double asinh( double x )
    endif

;
;       asinh(x) = log(x + sqrt(x*x + 1.0)));
;       asinh(x) = - asinh( -x );
;

        public  IF@DASINH       ; double asinh( double x )
        public  IF@ASINH        ; double asinh( double x )
        defp    IF@DASINH
        defp    IF@ASINH
        prolog
        sub     _SP,16                  ; allocate space
        fst     qword ptr -16[_BP]      ; make copy of argument
        fmul    st(0),st                ; calc. x*x
        fld1                            ; 1.0
        faddp   st(1),st                ; x*x + 1.0
        fsqrt                           ; sqrt( x*x + 1.0 )
        test    byte ptr -16+7[_BP],80h ; if x < 0.0
        _if     ne                      ; then
          fsub  qword ptr -16[_BP]      ; - sqrt( x*x + 1.0 ) - x
          fldln2                        ; - load ln(2)
          fxch  st(1)                   ; - get arguments in right order
          fyl2x                         ; - calc. log( sqrt( x*x + 1.0 ) - x )
          fchs                          ; - negate result
        _else                           ; else
          fadd  qword ptr -16[_BP]      ; - sqrt( x*x + 1.0 ) + x
          fldln2                        ; - load ln(2)
          fxch  st(1)                   ; - get arguments in right order
          fyl2x                         ; - calc. log( x + sqrt( x*x + 1.0 ) )
        _endif                          ; endif
        mov     _SP,_BP                 ; reset SP
        epilog                          ; epilog
        ret                             ; return
        endproc IF@ASINH
        endproc IF@DASINH


;  input:       x - on the stack
;  output:      asinh of x in st(0)
;
        defp    asinh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DASINH       ; calculate asinh(x)
        loadres                 ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        call    IF@DASINH       ; calculate asinh(x)
        pop     BP              ; restore BP
endif
        ret_pop 8               ; return
        endproc asinh

        endmod
        end
