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
        xref            __8087  ; indicate that NDP instructions are present

        modstart        atan87

        datasegment
        enddata

        xdefp   "C",atan        ; double atan( double x )
        xdefp   "C",atan2       ; double atan2( double y, double x )

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
endif

sgnY    equ     0
sgnX    equ     2


ifndef __FPI87__
piby2   dt      1.57079632679489661923
endif

        public  IF@ATAN
        public  IF@DATAN
        public  IF@ATAN2
        public  IF@DATAN2

        defp    IF@DATAN
        defp    IF@ATAN
ifdef __FPI87__
          fld1                  ; - load 1.0
          do_fpatan             ; - calculate fpatan
          ret                   ; - return
else
        chk387                  ; check to see if 387
        _if     e               ; if 387
          fld1                  ; - load 1.0
          do_fpatan             ; - calculate fpatan
          ret                   ; - return
        _endif                  ; endif

        defp    calc_atan
        push    _BP             ; save BP
        sub     _SP,4           ; allocate temporaries
        mov     _BP,_SP         ; get access to stack
        ftst                    ; get sign of argument
        fstsw   sgnX[_BP]       ; get status
        fabs                    ; make sure argument is positive
        fld1                    ; load 1.0
        fcom    st(1)           ; compare argument with 1.0
        fstsw   sgnY[_BP]       ; get result of compare
        fwait                   ; ...
        mov     AH,sgnY+1[_BP]  ; ...
        sahf                    ; ...
        _if     b               ; if argument > 1.0
          fxch  st(1)           ; - st = arg, st(1) = 1.0
          fpatan                ; - calculate arctangent(1/argument)
          fld   tbyte ptr piby2 ; - load pi/2
          fsubrp st(1),st       ; - calculate pi/2 - atn(1/argument)
        _else                   ; else
          fpatan                ; - calculate artangent(argument)
        _endif                  ; endif
        mov     AH,sgnX+1[_BP]  ; get status of argument
        sahf                    ; if original argument was negative
        _if     b               ; then
          fchs                  ; - negate the answer
        _endif                  ; endif
        add     _SP,4           ; remove temporaries
        pop     _BP             ; restore BP
        ret                     ; return
        endproc calc_atan
endif
        endproc IF@ATAN
        endproc IF@DATAN


        defp    IF@DATAN2
        defp    IF@ATAN2
;
;       Y is in st(0)
;       X is in st(1)
;
ifdef __FPI87__
          fxch  st(1)           ; - get arguments in correct order
          do_fpatan             ; - calculate fpatan
          ret                   ; - return
else
        chk387                  ; check to see if 387
        _if     e               ; if 387, then
          fxch  st(1)           ; - get arguments in correct order
          do_fpatan             ; - calculate fpatan
          ret                   ; - return
        _endif                  ; endif
        ftst                    ; test Y
        push    _BP             ; save BP
        sub     _SP,4           ; allocate temporaries
        mov     _BP,_SP         ; get access to temporaries
        fstsw   sgnY[_BP]       ; get status of Y
        fxch    st(1)           ; exchange arguments (X on top, Y in st(1))
        ftst                    ; test X
        fstsw   sgnX[_BP]       ; get status of X
        fwait                   ; wait for it
        _guess                  ; guess: Y == 0
          mov   AH,sgnY+1[_BP]  ; - get status of Y
          sahf                  ; - ...
          mov   AH,sgnX+1[_BP]  ; - get status of X
          _quif ne              ; - quit if Y != 0
          fstp  st(0)           ; - throw away X and Y
          fstp  st(0)           ; - ...
          sahf                  ; - get status of X
          _if   b               ; - if X < 0
            fldpi               ; - - return pi
          _else                 ; - else
            fldz                ; - - return 0.0
          _endif                ; - endif
        _admit                  ; guess: X == 0
          sahf                  ; - get status of X
          _quif ne              ; - quit if X != 0
          fstp  st(0)           ; - throw away X and Y
          fstp  st(0)           ; - ...
          fld   tbyte ptr piby2 ; - load pi/2
          mov   AH,sgnY+1[_BP]  ; - get status of Y
          sahf                  ; - ...
          _if   b               ; - if Y < 0
            fchs                ; - - return - pi/2
          _endif                ; - endif
        _admit                  ; admit: normal values for X and Y
          fdivp st(1),st        ; - calculate single argument Y/X
          lcall calc_atan       ; - calculate arctan(Y/X)
          ftst                  ; - test result
          fstsw sgnX[_BP]       ; - ...
          mov   AH,sgnY+1[_BP]  ; - get status of Y
          sahf                  ; - ...
          fwait                 ; - wait for status of result
          mov   AH,sgnX+1[_BP]  ; - get it
          _if   b               ; - if Y < 0
            sahf                ; - - get status of result
            _if   a             ; - - if result > 0
              fldpi             ; - - - subtract pi from result
              fsubp  st(1),st   ; - - - ...
            _endif              ; - - endif
          _else                 ; - else
            sahf                ; - - get status of result
            _if   b             ; - - if result < 0
              fldpi             ; - - - add pi to result
              faddp  st(1),st   ; - - - ...
            _endif              ; - - endif
          _endif                ; - endif
        _endguess               ; endguess
        add     _SP,4           ; remove temporaries
        pop     _BP             ; restore BP
        ret                     ; return
endif
        endproc IF@ATAN2
        endproc IF@DATAN2


;  input:       x - on the stack
;  output:      arctan of x in st(0)
;
        defp    atan
ifdef __386__
        fld     qword ptr 4[ESP]; load x
        call    IF@DATAN        ; calculate atan(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DATAN        ; calculate atan(x)
        pop     BP              ; restore BP
endif
        ret_pop 8               ; return
        endproc atan

        defp    atan2
ifdef __386__
        fld     qword ptr 12[ESP]; load x
        fld     qword ptr 4[ESP]; load y
        call    IF@DATAN2       ; calculate atan2(y,x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        fld     qword ptr argx+8[BP]; load x
        fld     qword ptr argx[BP]; load y
        lcall   IF@DATAN2       ; calculate atan2(y,x)
        pop     BP              ; restore BP
endif
        ret_pop 16              ; return
        endproc atan2


        endmod
        end
