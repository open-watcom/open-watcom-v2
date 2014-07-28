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

        modstart    log87

        xrefp   "C",__log87_err

        xdefp   "C",log         ; calc log(fac1)
        xdefp   "C",log2        ; calc log2(fac1)
        xdefp   "C",log10       ; calc log10(fac1)

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
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        mov     AL,FP_FUNC_LOG          ; indicate log
do_log:
        ftst                            ; test sign of argument
        fstsw   word ptr func           ; get status word
        fwait                           ; ...
        mov     AH,byte ptr func+1      ; ...
        sahf                            ; set flags
        _if     be                      ; if number is <= 0.0
          fstp  qword ptr data          ; - push argument on stack
          mov   func,_AX                ; - push code
ifdef __STACK__
          mov   sedx,EDX                ; - save EDX (-3s)
          mov   secx,ECX                ; - save ECX (-3s)
          call  __log87_err             ; - log error
          push  EDX                     ; - load result into 8087
          push  EAX                     ; - ...
          fld   qword ptr 0[ESP]        ; - ...
          mov   ECX,secx                ; - restore ECX (-3s)
          mov   EDX,sedx                ; - restore EDX (-3s)
          fwait                         ; - ...
else
          call  __log87_err             ; - log error
endif
          mov   AL,1                    ; - indicate error
        _else                           ; else
          cmp   AL,FP_FUNC_LOG10        ; - if "log10"
          _if   e                       ; - then
            fldlg2                      ; - - load log10(2)
          _else                         ; - else
            cmp   AL,FP_FUNC_LOG2       ; - - if "log2"
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
        ret                             ; return
        endproc IF@LOG
        endproc IF@DLOG


        public  IF@LOG2
        public  IF@DLOG2
        defp    IF@DLOG2
        defp    IF@LOG2
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        mov     AL,FP_FUNC_LOG2         ; indicate log2
        jmp     do_log                  ; calculate log2
        endproc IF@LOG2
        endproc IF@DLOG2


        public  IF@LOG10
        public  IF@DLOG10
        defp    IF@DLOG10
        defp    IF@LOG10
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        mov     AL,FP_FUNC_LOG10        ; indicate log10
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
        lcall   IF@DLOG                 ; calculate log(x)
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
        lcall   IF@DLOG10               ; calculate log10(x)
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
        lcall   IF@DLOG2                ; calculate log2(x)
        epilog
endif
        ret_pop 8                       ; return
        endproc log2

        endmod
        end
