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
        xref    "C",__log87_err
    else
        xref    <"C",__log87_err>
    endif
        xref    __8087  ; indicate that NDP instructions are present

        modstart    log387

    if __WASM__ ge 100
        xdefp   "C",log         ; calc log(fac1)
        xdefp   "C",log2        ; calc log2(fac1)
        xdefp   "C",log10       ; calc log10(fac1)
    else
        xdefp   <"C",log>       ; calc log(fac1)
        xdefp   <"C",log2>      ; calc log2(fac1)
        xdefp   <"C",log10>     ; calc log10(fac1)
    endif

ifndef __386__
        if _MODEL and _BIG_CODE
         argx    equ     6
        else
         argx    equ     4
        endif
endif


        public  IF@LOG
        public  IF@DLOG
        defp    IF@DLOG
        defp    IF@LOG

        mov     AL,FUNC_LOG             ; indicate log

do_log:
        ftst                            ; test sign of argument
        prolog
        sub     _SP,16                  ; allocate space
        fstsw   word ptr -16[_BP]       ; get status word
        fwait                           ; ...
        mov     AH,-16+1[_BP]           ; ...
        sahf                            ; set flags
        _if     be                      ; if number is <= 0.0
          mov   -16+8[_BP],_AX          ; - push code
          fstp  qword ptr -16[_BP]      ; - push argument on stack
          mov   -16+12[_BP],_DX         ; - save DX (-3s) clobbers EDX 17-mar-92
          call  __log87_err             ; - log error
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
        _else                           ; else
          cmp   AL,FUNC_LOG10           ; - if "log10"
          _if   e                       ; - then
            fldlg2                      ; - - load log10(2)
          _else                         ; - else
            cmp   AL,FUNC_LOG2          ; - - if "log2"
            _if   e                     ; - - then
              fld1                      ; - - - load 1.0
            _else                       ; - - else
              fldln2                    ; - - - load ln(2)
            _endif                      ; - - endif
          _endif                        ; - endif
          fxch  st(1)                   ; - get arguments in right order
          fyl2x                         ; - ln(x) = ln(2) * log2(x)
          mov   AL,0                    ; - indicate success
        _endif                          ; endif
        mov     _SP,_BP                 ; reset SP
        epilog
        ret                             ; return
        endproc IF@LOG
        endproc IF@DLOG


        public  IF@LOG2
        public  IF@DLOG2
        defp    IF@DLOG2
        defp    IF@LOG2
        mov     AL,FUNC_LOG2            ; indicate log2
        jmp     do_log                  ; calculate log2
        endproc IF@LOG2
        endproc IF@DLOG2


        public  IF@LOG10
        public  IF@DLOG10
        defp    IF@DLOG10
        defp    IF@LOG10
        mov     AL,FUNC_LOG10           ; indicate log10
        jmp     do_log                  ; calculate log10
        endproc IF@LOG10
        endproc IF@DLOG10


        defp    log
ifdef __386__
        fld     qword ptr 4[ESP]        ; load argument
        call    IF@DLOG                 ; calculate log(x)
        loadres                         ; load result
else
        prolog
        fld     qword ptr argx[BP]      ; load argument
        call    IF@DLOG                 ; calculate log(x)
        epilog
endif
        ret_pop 8                       ; return
        endproc log

        defp    log10
ifdef __386__
        fld     qword ptr 4[ESP]        ; load argument
        call    IF@DLOG10               ; calculate log10(x)
        loadres                         ; load result
else
        prolog
        fld     qword ptr argx[BP]      ; load argument
        call    IF@DLOG10               ; calculate log10(x)
        epilog
endif
        ret_pop 8                       ; return
        endproc log10


        defp    log2
ifdef __386__
        fld     qword ptr 4[ESP]        ; load argument
        call    IF@DLOG2                ; calculate log2(x)
        loadres                         ; load result
else
        prolog
        fld     qword ptr argx[BP]      ; load argument
        call    IF@DLOG2                ; calculate log2(x)
        epilog
endif
        ret_pop 8                       ; return
        endproc log2

        endmod
        end
