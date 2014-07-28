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
        xrefp   __8087  ; indicate that NDP instructions are present

        modstart    atanh87

        xrefp   "C",__log87_err

        xdefp   "C",atanh       ; double atanh( double x )
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
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        fld1                        ; get 1.0
        fld     st(1)               ; duplicate the number
        fabs                        ; get absolute value of number
        fcompp                      ; compare against 1.0
        fstsw   word ptr func       ; get status word
        fwait                       ; ...
        mov     AH,byte ptr func+1  ; ...
        sahf                        ; set flags
        _if     ae                  ; if number is >= 1.0
          fstp  qword ptr data      ; - push argument on stack
          mov   AL,FP_FUNC_ATANH    ; - indicate "atanh"
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
          fld1                      ; - 1.0
          fsub    st,st(1)          ; - 1.0 - x
          fld1                      ; - 1.0
          faddp   st(2),st          ; - 1.0 + x
          do_fdivp 1,0              ; - (1.0 + x) / (1.0 - x)
          fldln2                    ; - load ln(2)
          fxch  st(1)               ; - get arguments in right order
          fyl2x                     ; - calc. log( (1.0 + x) / (1.0 - x) )
          fidiv two                 ; - divide result by 2.0
        _endif                      ; endif
        ret                         ; return
        endproc IF@ATANH
        endproc IF@DATANH


;  input:       x - on the stack
;  output:      atanh of x in st(0)
;
        defp    atanh
ifdef __386__
        fld     qword ptr 4[ESP]    ; load argument x
        call    IF@DATANH           ; calculate atanh(x)
        loadres                     ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        prolog
        fld     qword ptr argx[BP]  ; load argument x
        lcall   IF@DATANH           ; calculate atanh(x)
        epilog
endif
        ret_pop 8                   ; return
        endproc atanh

        endmod
        end
