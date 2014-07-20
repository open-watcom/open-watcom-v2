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
;   interface to floating point library for trig functions
;
.387
include mdef.inc
include struct.inc
include math87.inc

        extern_chipbug
        xrefp           __8087  ; indicate that NDP instructions are present

        modstart        trig87

        ;datasegment
        ;enddata

        xdefp   "C",sin         ; calc sin(fac1)
        xdefp   "C",cos         ; calc cos(fac1)
        xdefp   "C",tan         ; calc tan(fac1)

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
status  equ     -4
endif                           ; __386__

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     SIN and COS functions for 8087                                  <>
;<>                                                                     <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     sin8087 - compute sin of st(0)                                  <>
;<>                                                                     <>
;<>     let st(0) be theta. must reduce theta into range 0 to pi/4      <>
;<>     and determine which octant theta is in. if theta is in an odd   <>
;<>     octant then define beta to be - (alpha - pi/4), and use beta.   <>
;<>     fptan instruction computes Y and X which can be used to         <>
;<>     compute either sin or cos.                                      <>
;<>     we use the octant number to determine which answer is required. <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

ifdef __386__
    datasegment
endif
TwoPi   db      035h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h,001h,040h
ifndef __FPI87__
piby2   db      035h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h,0ffh,03fh
piby4   db      035h,0c2h,068h,021h,0a2h,0dah,00fh,0c9h,0feh,03fh
;;;piby2   dt      1.57079632679489661923
;;;piby4   dt      0.78539816339744830961

ifdef __386__
octtab  dd      oct0            ; sin(alpha)
        dd      oct1            ; cos(beta)
        dd      oct2            ; cos(alpha)
        dd      oct3            ; sin(beta)
        dd      oct4            ; - sin(alpha)
        dd      oct5            ; - cos(beta)
        dd      oct6            ; - cos(alpha)
        dd      oct7            ; - sin(beta)
else
octtab  dw      oct0            ; sin(alpha)
        dw      oct1            ; cos(beta)
        dw      oct2            ; cos(alpha)
        dw      oct3            ; sin(beta)
        dw      oct4            ; - sin(alpha)
        dw      oct5            ; - cos(beta)
        dw      oct6            ; - cos(alpha)
        dw      oct7            ; - sin(beta)
endif                           ; __386__
endif                           ; __FPI87__
ifdef __386__
    enddata
endif


        public  IF@COS
        public  IF@DCOS
        defp    IF@DCOS
        defp    IF@COS
ifndef __FPI87__
        chk387
        mov     AL,2            ; cos(x) = sin(x+pi/2)
        jne     Sin8087         ;   which is 2 octants over
endif                           ; __FPI87__
        _loop                   ; loop
          fcos                  ; - calculate cos
          call  chk_C2          ; - see if argument was in range
        _until  c               ; until done
        ret                     ; return
        endproc IF@COS
        endproc IF@DCOS


        public  IF@SIN
        public  IF@DSIN
        defp    IF@DSIN
        defp    IF@SIN
ifdef __FPI87__
        _loop                   ; loop
          fsin                  ; - calculate sin
          call  chk_C2          ; - see if argument was in range
        _until  c               ; until done
        ret                     ; return
else
        chk387                  ; check for 387
        mov     AL,0            ; no octant adjustment required
        _if     e               ; if 387
          _loop                 ; - loop
            fsin                ; - - calculate sin
            call  chk_C2        ; - - see if argument was in range
          _until  c             ; - until done
          ret                   ; - return
        _endif                  ; endif

Sin8087:
ifdef __386__
        push    EBX             ; save EBX
        sub     EBX,EBX         ; zero whole register
        mov     BL,AL           ; save octant adjustment
        fld     tbyte ptr piby4 ; load pi/4
        fxch    st(1)           ; exchange
        ftst                    ; get sign of argument
        fstsw   AX              ; . . .
        sahf                    ; get flags
        _if     b               ; if number is negative
          fchs                  ; - make argument positive
          cmp   BL,0            ; - if computing SIN function
          _if   e               ; - then
            mov   BL,4          ; - - set octant adjustment
          _endif                ; - endif
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi/4)
          fprem                 ; - calculate partial remainder
          fstsw AX              ; - store status word
          test  AX,0400h        ; - check C2
        _until  e               ; until C2 is clear
        mov     AL,0            ; set octant # to 0
        test    AH,02h          ; if C1 is on
        _if     ne              ; then
          or    AL,1            ; - set low order bit of octant
;         compute beta = - ( alpha - pi/4 ); and use beta as the angle

          fsub  st(0),st(1)     ; - subtract pi/4 from the angle
          fchs                  ; - and change the sign
        _endif                  ; endif
        test    AH,40h          ; if C3 is on
        _if     ne              ; then
          or    AL,2            ; - set next bit in octant number
        _endif                  ; endif
        test    AH,01h          ; if C0 is on
        _if     ne              ; then
          add   AL,04h          ; - set next bit in octant #
        _endif                  ; endif

;       AL now has the octant number

        fstp    st(1)           ; copy angle up over pi/4
        fptan                   ; st(0) := X,  st(1) := Y
        fld     st(0)           ; duplicate X
        fmul    st(0),st(0)     ; calculate X*X
        fld     st(2)           ; duplicate Y
        fmul    st(0),st(0)     ; calculate Y*Y
        faddp   st(1),st(0)     ; calculate X*X + Y*Y
        fsqrt                   ; R = sqrt(X*X + Y*Y)
        add     BL,AL           ; add in octant adjustment
        and     BL,7            ; reduce to mod 8
        lea     EBX,octtab[EBX*4] ; point to table entry
        mov     EAX,cs:[EBX]    ; get address of routine from table
        pop     EBX             ; restore EBX
        jmp     EAX             ; jump to appropriate routine
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to locals
        push    BX              ; save BX
        mov     BX,AX           ; save octant adjustment
        push    AX              ; allocate status word
        fld     tbyte ptr piby4 ; load pi/4
        fxch    st(1)           ; exchange
        ftst                    ; get sign of argument
        fstsw   status[BP]      ; . . .
        fwait                   ; . . .
        mov     AX,status[BP]   ; . . .
        sahf                    ; get flags
        _if     b               ; if number is negative
          fchs                  ; - make argument positive
          cmp   BL,0            ; - if computing SIN function
          _if   e               ; - then
            mov   BL,4          ; - - set octant adjustment
          _endif                ; - endif
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi/4)
          fprem                 ; - calculate partial remainder
          fstsw status[BP]      ; - store status word
          fwait                 ; - wait for store to complete
          mov   AX,status[BP]   ; - get status
          test  AX,0400h        ; - check C2
        _until  e               ; until C2 is clear
        mov     AL,0            ; set octant # to 0
        test    AH,02h          ; if C1 is on
        _if     ne              ; then
          or    AL,1            ; - set low order bit of octant
;         compute beta = - ( alpha - pi/4 ); and use beta as the angle

          fsub  st(0),st(1)     ; - subtract pi/4 from the angle
          fchs                  ; - and change the sign
        _endif                  ; endif
        test    AH,40h          ; if C3 is on
        _if     ne              ; then
          or    AL,2            ; - set next bit in octant number
        _endif                  ; endif
        test    AH,01h          ; if C0 is on
        _if     ne              ; then
          add   AL,04h          ; - set next bit in octant #
        _endif                  ; endif

;       AL now has the octant number

        fstp    st(1)           ; copy angle up over pi/4
        fptan                   ; st(0) := X,  st(1) := Y
        fld     st(0)           ; duplicate X
        fmul    st(0),st(0)     ; calculate X*X
        fld     st(2)           ; duplicate Y
        fmul    st(0),st(0)     ; calculate Y*Y
        faddp   st(1),st(0)     ; calculate X*X + Y*Y
        fsqrt                   ; R = sqrt(X*X + Y*Y)
        add     BL,AL           ; add in octant adjustment
        and     BL,7            ; reduce to mod 8
        shl     BL,1            ; double it for index
        mov     BH,0            ; zero high byte
        lea     BX,octtab[BX]   ; point to table entry
        mov     AX,cs:[BX]      ; get address of routine from table
        pop     BX              ; clean up the stack
        pop     BX              ; restore BX
        pop     BP              ; restore BP
        jmp     AX              ; jump to appropriate routine
endif                           ; __386__

;       at this point the 8087 stack looks like this:
;
;       st(0) = R
;       st(1) = X
;       st(2) = Y
;       st(3) = fac2
;
;       sin(theta) = Y/R
;       cos(theta) = X/R

oct4:                           ; octant 4
oct7:                           ; octant 7
        fchs                    ; change sign of R, to get -sin(theta)
oct0:                           ; octant 0
oct3:                           ; octant 3
        fstp    st(1)           ; copy R up over X
        fdivp   st(1),st(0)     ; sin(theta) = Y/R
        ret                     ; return

oct5:                           ; octant 5
oct6:                           ; octant 6
        fchs                    ; change sign of R, to get -cos(theta)
oct1:                           ; octant 1
oct2:                           ; octant 2
        fdivp   st(1),st(0)     ; cos(theta) = X/R
        fstp    st(1)           ; copy answer up over Y
        ret                     ; return
endif                           ; __FPI87__
        endproc IF@SIN
        endproc IF@DSIN


chk_C2  proc    near
        push    _BP             ; save BP
        mov     _BP,_SP         ; get access to stack
        push    _AX             ; allocate stack space
        fstsw   word ptr -2[_BP]; get status word
        fwait
        mov     AH,-1[_BP]      ; get flags
        or      AH,1            ; force carry bit on (assume arg in range)
        sahf                    ; copy to flags
        _if     p               ; if argument not in range
          fld   tbyte ptr TwoPi ; - load 2*pi
          fxch  st(1)           ; - get them in correct position
          _loop                 ; - loop (reduce argument to range)
            fprem               ; - - reduce the argument
            fstsw  word ptr -2[_BP]; - - get status
            fwait
            mov    AH,-1[_BP]   ; - - ...
            sahf                ; - - ...
          _until  np            ; - until argument in range
          fstp  st(1)           ; - copy up the new argument
          clc                   ; - indicate must retry instruction
        _endif                  ; endif
        pop     _AX             ; clean up stack
        pop     _BP             ; restore BP
        ret                     ; return
        endproc chk_C2


        defp    cos
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DCOS         ; calculate cos(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DCOS         ; calculate cos(x)
        pop     BP              ; restore BP
endif                           ; __386__
        ret_pop 8               ; return
        endproc cos


        defp    sin
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DSIN         ; calculate sin(x)
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DSIN         ; calculate sin(x)
        pop     BP              ; restore BP
endif                           ; __386__
        ret_pop 8               ; return
        endproc sin


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     TAN function for 8087                                           <>
;<>                                                                     <>
;<>     tan387 - compute tan of st(0)                                   <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        public  IF@TAN
        public  IF@DTAN
        defp    IF@DTAN
        defp    IF@TAN
ifdef __FPI87__
        do_fptan                ; calculate tan
        fstp    st(0)           ; get rid of the 1.0 pushed by fptan
        ret                     ; return
else
        chk387                  ; check for 387
        _if     e               ; if 387
          do_fptan              ; - calculate tan
          fstp   st(0)          ; - get rid of the 1.0 pushed by fptan
          ret                   ; - return
        _endif                  ; endif
ifdef __386__
        push    EBX             ; save EBX
        fldpi                   ; load pi
        sub     EBX,EBX         ; init 'invert'(BH) flag and 'negate'(BL) flag
        fxch    st(1)           ; get argument
        ftst                    ; get sign of argument
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if argument is negative
          fchs                  ; - make argument positive
          not   BL              ; - indicate result to be negated
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi)
          fprem                 ; - calculate partial remainder
          fstsw AX              ; - store status word
          sahf                  ; - ... PF = C2
        _until  np              ; until C2 is clear
        fld     tbyte ptr piby2 ; load pi/2
        fstp    st(2)           ; stack is r, pi/2
        fcom                    ; compare r to pi/2
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     a               ; if r > pi/2
          fsub  st,st(1)        ; - tan(r) = -1/tan(r - pi/2)
          not   EBX             ; - set the 'invert' flag, complement 'negate'
        _endif                  ; endif
        fld     tbyte ptr piby4 ; load pi/4
        fcomp                   ; compare pi/4 to r and pop
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if r > pi/4
          fsubr st,st(1)        ; - tan(r) = 1/tan(pi/2 - r)
          not   BH              ; - complement the 'invert' flag
        _endif                  ; endif
        fstp    st(1)           ; copy r up over pi/2
        fptan                   ; st(1)/st(0) = tangent, st(0) = X, st(1) = Y
        cmp     BH,0            ; if result is inverted
        _if     ne              ; then
          fdivr                 ; - 1/tangent = st(0)/st(1)
        _else                   ; else
          fdiv                  ; - tangent = st(1)/st(0)
        _endif                  ; endif
        cmp     BL,0            ; if result to be negated
        _if     ne              ; then
          fchs                  ; - negate result
        _endif                  ; endif
        pop     EBX             ; restore registers
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; allocate auto
        push    AX              ; save registers
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        fldpi                   ; load pi
        sub     BX,BX           ; init 'invert' flag
        sub     CX,CX           ; init 'negate' flag
        fxch    st(1)           ; get argument
        ftst                    ; get sign of argument
        fstsw   word ptr -2[BP] ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if argument is negative
          fchs                  ; - make argument positive
          not   CX              ; - indicate result to be negated
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi)
          fprem                 ; - calculate partial remainder
          fstsw word ptr -2[BP] ; - store status word
          fwait                 ; - wait for store to complete
          mov   AX,-2[BP]       ; - get status
          sahf                  ; - ... PF = C2
        _until  np              ; until C2 is clear
        fld     tbyte ptr piby2 ; load pi/2
        fstp    st(2)           ; stack is r, pi/2
        fcom                    ; compare r to pi/2
        fstsw   word ptr -2[BP] ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     a               ; if r > pi/2
          fsub  st,st(1)        ; - tan(r) = -1/tan(r - pi/2)
          not   BX              ; - set the 'invert' flag
          not   CX              ; - complement the 'negate' flag
        _endif                  ; endif
        fld     tbyte ptr piby4 ; load pi/4
        fcomp                   ; compare pi/4 to r and pop
        fstsw   word ptr -2[BP] ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if r > pi/4
          fsubr st,st(1)        ; - tan(r) = 1/tan(pi/2 - r)
          not   BX              ; - complement the 'invert' flag
        _endif                  ; endif
        fstp    st(1)           ; copy r up over pi/2
        fptan                   ; st(1)/st(0) = tangent, st(0) = X, st(1) = Y
        or      BX,BX           ; if result is inverted
        _if     ne              ; then
          fdivr                 ; - 1/tangent = st(0)/st(1)
        _else                   ; else
          fdiv                  ; - tangent = st(1)/st(0)
        _endif                  ; endif
        or      CX,CX           ; if result to be negated
        _if     ne              ; then
          fchs                  ; - negate result
        _endif                  ; endif
        pop     DX              ; restore registers
        pop     CX              ; . . .
        pop     BX              ; . . .
        pop     AX              ; . . .
        pop     BP              ; clean off auto
        pop     BP              ; restore BP
endif                           ; __386__
        ret                     ; return
endif                           ; __FPI87__
        endproc IF@TAN
        endproc IF@DTAN


        defp    tan
ifdef __386__
        fld     qword ptr 4[ESP]; load argument
        call    IF@DTAN         ; calc the tan
        loadres                 ; load result
else
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DTAN         ; calculate tan(x)
        pop     BP              ; restore BP
endif                           ; __386__
        ret_pop 8               ; return
        endproc tan


        endmod
        end
