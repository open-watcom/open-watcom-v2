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


include mdef.inc
ifdef __386__
 .387
else
 .8087
endif
include struct.inc
include math387.inc

        extern_chipbug
    if __WASM__ ge 100
        xref    "C",__log87_err
    else
        xref    <"C",__log87_err>
    endif
        xref    __8087  ; indicate that NDP instructions are present

        modstart    atanh387

    if __WASM__ ge 100
        xdefp   "C",atanh       ; double atanh( double x )
    else
        xdefp   <"C",atanh>     ; double atanh( double x )
    endif

;
;       atanh(x) = log( (1.0 + x) / (1.0 - x) ) / 2.0
;
ifdef __386__
    datasegment
endif
two     dw      2
ifdef __386__
    enddata
endif

        public  IF@DATANH       ; double atanh( double x )
        public  IF@ATANH        ; double atanh( double x )
        defp    IF@DATANH
        defp    IF@ATANH
        prolog
        fld1                    ; get 1.0
        fld     st(1)           ; duplicate the number
        fabs                    ; get absolute value of number
        fcompp                  ; compare against 1.0
        sub     _SP,16          ; allocate space
        fstsw   word ptr -16[_BP] ; get status word
        fwait                   ; ...
        mov     AH,-16+1[_BP]   ; ...
        sahf                    ; set flags
        _if     ae              ; if number is >= 1.0
          mov   -16+12[_BP],_DX ; - save DX (-3s) clobbers EDX 17-mar-92
          mov   AL,FUNC_ATANH   ; - indicate "atanh"
          mov   -16+8[_BP],_AX  ; - push code
          fstp  qword ptr -16[_BP]; - push argument on stack
          call  __log87_err     ; - log error
ifdef __386__
 ifdef __STACK__
          push  EDX             ; - load result into 8087
          push  EAX             ; - ...
          fld   qword ptr 0[ESP]; - ...
          mov   EDX,-16+12[EBP] ; - restore EDX 17-mar-92
          fwait                 ; - ...
 endif
endif
        _else                   ; else
          fld1                  ; - 1.0
          fsub    st,st(1)      ; - 1.0 - x
          fld1                  ; - 1.0
          faddp   st(2),st      ; - 1.0 + x
          do_fdivp 1,0          ; - (1.0 + x) / (1.0 - x)
          fldln2                ; - load ln(2)
          fxch  st(1)           ; - get arguments in right order
          fyl2x                 ; - calc. log( (1.0 + x) / (1.0 - x) )
          fidiv two             ; - divide result by 2.0
        _endif                  ; endif
        mov     _SP,_BP         ; reset SP
        epilog
        ret                     ; return
        endproc IF@ATANH
        endproc IF@DATANH


;  input:       x - on the stack
;  output:      atanh of x in st(0)
;
        defp    atanh
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DATANH       ; calculate atanh(x)
        loadres                 ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        prolog
        fld     qword ptr argx[BP]; load argument x
        call    IF@DATANH       ; calculate atanh(x)
        epilog
endif
        ret_pop 8               ; return
        endproc atanh

        endmod
        end
