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
;     interface to floating point library for trig functions
;
ifdef __386__
 .387
else
 .8087
endif
include mdef.inc
include struct.inc
include math87.inc

        xrefp   __8087  ; indicate that NDP instructions are present

        modstart    exp87

        ;datasegment
        ;enddata

        xrefp   "C",__math87_err

        xdefp   "C",exp         ; calc exp(fac1)

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
endif

ifdef __386__
    datasegment
endif
MaxExpVal dq    709.7827128933839
;MaxExpVal DB   0eeH,39H,0faH,0feH,42H,2eH,86H,040H
MinExpVal dq    -709.7827128933839
;MinExpVal DB   0eeH,39H,0faH,0feH,42H,2eH,86H,0c0H
ifdef __386__
    enddata
endif

        public  __@DEXP1
        defp    __@DEXP1                ; calc. exp(x) with no check
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        jmp     short calc_exp          ; go calculate exp(x)
        endproc __@DEXP1

        public  IF@EXP
        public  IF@DEXP
        defp    IF@DEXP
        defp    IF@EXP

;       calculate exp(st(0)) using 387
        mov     AL,FP_FUNC_EXP          ; indicate exp function

        public  __@DEXP
        defp    __@DEXP
ifndef __386__
        local   func:WORD,data:QWORD
elseifdef __STACK__
        local   sedx:DWORD,secx:DWORD,func:DWORD,data:QWORD
else
        local   func:DWORD,data:QWORD
endif
        fcom    qword ptr MaxExpVal     ; compare with largest valid argument
        fstsw   word ptr func           ; get status
        fwait                           ; wait for it
        mov     AH,byte ptr func+1      ; get status word
        sahf                            ; set flags
        _if     a                       ; if number is too large
          cmp   AL,FP_FUNC_POW          ; - if not "pow" function
          _if   ne                      ; - then
            fstp  qword ptr data        ; - - push argument on stack
            mov   func,_AX              ; - - set func code
ifdef __STACK__
            mov   sedx,EDX              ; - - save EDX (-3s)
            mov   secx,ECX              ; - - save ECX (-3s)
            call  __math87_err          ; - - math error
            push  EDX                   ; - - load result into 8087
            push  EAX                   ; - - ...
            fld   qword ptr 0[ESP]      ; - - ...
            mov   ECX,secx              ; - - restore ECX (-3s)
            mov   EDX,sedx              ; - - restore EDX (-3s)
            fwait                       ; - - ...
else
            call  __math87_err          ; - - math error
endif
          _endif                        ; - endif
          mov   AL,1                    ; - indicate error
          ret                           ; - return
        _endif                          ; endif
calc_exp:
        _guess                          ; - guess: argument too small
          fcom  qword ptr MinExpVal     ; - - compare with smallest valid arg
          fstsw word ptr func           ; - - get status
          fwait                         ; - - wait for it
          mov   AH,byte ptr func+1      ; - - get status word
          sahf                          ; - - set flags
          _quif a                       ; - - quit if number is not too small
          fldz                          ; - - set result to 0.0
        _admit                          ; - guess: 387
ifndef __FPI87__
          chk387
          _quif  ne                     ; - - quit if not 387
endif                                   ; __FPI87__
          fldl2e                        ; - - load log2(e)
          fmulp   st(1),st(0)           ; - - calculate argument * log2(e)
          fld     st(0)                 ; - - duplicate fac1
          frndint                       ; - - round to an integer
          fxch    st(1)                 ; - - flip around
          fsub    st,st(1)              ; - - calc. x - int(x)
          f2xm1                         ; - - calc. 2 ** ( x - int(x) )
          fld1                          ; - - load 1.0
          faddp   st(1),st(0)           ; - - add 1 to result
          fscale                        ; - - multiply by 2 ** int(x)
ifndef __FPI87__
        _admit                          ; - admit: argument in valid range
          fldl2e                        ; - - load log2(e)
          fmulp   st(1),st(0)           ; - - calculate argument * log2(e)
          fld1                          ; - - load -1 for dividing by 2
          fchs                          ; - - ...
          fld   st(1)                   ; - - duplicate x
          fstcw word ptr func           ; - - save control word
          fwait                         ; - - wait
          mov   AL,byte ptr func+1      ; - - save it
          and   byte ptr func+1,0F3h    ; - - mask out rounding control bits
          or    byte ptr func+1,04h     ; - - set to round down to - infinity
          fldcw word ptr func           ; - - load new control word
          frndint                       ; - - round to an integer
          mov   byte ptr func+1,AL      ; - - set original control word
          fldcw word ptr func           ; - - restore original control word
          fxch  st(2)                   ; - - exchange int(x) with x
          fsub  st,st(2)                ; - - calc. x - int(x)
          fscale                        ; - - divide by 2
          f2xm1                         ; - - calc 2 ** x
          fsubrp st(1),st               ; - - subtract -1 (or add 1)
          fmul  st,st(0)                ; - - square the result
          fscale                        ; - - multiply by 2 ** int(x)
endif                                   ; __FPI87__
        _endguess                       ; - endguess
        fstp  st(1)                     ; - copy answer up
        mov   AL,0                      ; - indicate no error
        ret                             ; return
        endproc __@DEXP
        endproc IF@EXP
        endproc IF@DEXP


        defp    exp
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DEXP         ; calculate exp(x)
        loadres                 ; load result
else
        prolog
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DEXP         ; calculate exp(x)
        epilog
endif
        ret_pop 8               ; return
        endproc exp

        endmod
        end
