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
include struct.inc
include math87.inc

        extern_chipbug

        modstart        sin087
.8087
        xdefp   __sin87         ; calc sin(fac1)
        xdefp   __cos87         ; calc cos(fac1)

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
;<>     we use the octant number to determine which answer is reuired.  <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

octtab: dw      oct0            ; sin(alpha)
        dw      oct1            ; cos(beta)
        dw      oct2            ; cos(alpha)
        dw      oct3            ; sin(beta)
        dw      oct4            ; - sin(alpha)
        dw      oct5            ; - cos(beta)
        dw      oct6            ; - cos(alpha)
        dw      oct7            ; - sin(beta)


piby4:  dt      0.78539816339744830961


        defp    __cos87
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; push argument
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        mov     AL,2            ; cos(x) = sin(x+pi/2)
        jmp     Sin8087         ;   which is 2 octants over


        defp    __sin87
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; push argument
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        mov     AL,0            ; no octant adjustment required

Sin8087:
        mov     CX,AX           ; save octant adjustment
        fld     tbyte ptr piby4 ; load pi/4
        fld     qword ptr -8[BP]; load argument
        ftst                    ; get sign of argument
        fstsw   -2[BP]          ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        and     AX,4100h        ; isolate condition codes
        cmp     AX,0100h        ; if number is negative
        _if     e               ; then
          fchs                  ; - make argument positive
          cmp   CL,0            ; - if computing SIN function
          _if   e               ; - then
            mov   CL,4          ; - - set octant adjustment
          _endif                ; - endif
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi/4)
          fprem                 ; - calculate partial remainder
          fstsw -2[BP]          ; - store status word
          fwait                 ; - wait for store to complete
          mov   AX,-2[BP]       ; - get status
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
        add     AL,CL           ; add in octant adjustment
        and     AL,7            ; reduce to mod 8
        shl     AL,1            ; double it for index
        mov     AH,0            ; zero high byte
        mov     BX,AX           ; get index
        lea     BX,octtab[BX]   ; point to table entry
        mov     AX,cs:[BX]      ; get address of routine from table
        call    AX              ; call appropriate routine
        fstp    qword ptr -8[BP]; store result
        fwait                   ; wait
        pop     DX              ; load result
        pop     CX              ; . . .
        pop     BX              ; . . .
        pop     AX              ; . . .
        pop     BP              ; restore BP
        ret                     ; return
        endproc __sin87
        endproc __cos87

;       at this point the 8087 stack looks like this:
;
;       st(0) = R
;       st(1) = X
;       st(2) = Y
;       st(3) = fac2
;
;       sin(theta) = Y/R
;       cos(theta) = X/R

oct4    proc    near            ; octant 4
oct7:                           ; octant 7
        fchs                    ; change sign of R, to get -sin(theta)
oct0:                           ; octant 0
oct3:                           ; octant 3
        fstp    st(1)           ; copy R up over X
        do_fdivp   1,0          ; sin(theta) = Y/R
        ret                     ; return

oct5:                           ; octant 5
oct6:                           ; octant 6
        fchs                    ; change sign of R, to get -cos(theta)
oct1:                           ; octant 1
oct2:                           ; octant 2
        do_fdivp   1,0          ; cos(theta) = X/R
        fstp    st(1)           ; copy answer up over Y
        ret                     ; return
        endproc oct4

        endmod
        end
