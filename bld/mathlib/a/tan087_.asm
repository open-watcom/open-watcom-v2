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

        modstart        tan087
.8087
        xdefp   __tan87         ; calc tan(fac1)

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     TAN function for 8087                                           <>
;<>                                                                     <>
;<>     tan8087 - compute tan of st(0)                                  <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

piby2:  dt      1.57079632679489661923
piby4:  dt      0.78539816339744830961


        defp    __tan87
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; push argument
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        fldpi                   ; load pi
        sub     BX,BX           ; init 'invert' flag
        sub     CX,CX           ; init 'negate' flag
        fld     qword ptr -8[BP]; load argument
        ftst                    ; get sign of argument
        fstsw   -2[BP]          ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if argument is negative
          fchs                  ; - make argument positive
          not   CX              ; - indicate result to be negated
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi)
          fprem                 ; - calculate partial remainder
          fstsw -2[BP]          ; - store status word
          fwait                 ; - wait for store to complete
          mov   AX,-2[BP]       ; - get status
          sahf                  ; - ... PF = C2
        _until  np              ; until C2 is clear
        fld     tbyte ptr piby2 ; load pi/2
        fstp    st(2)           ; stack is r, pi/2
        fcom                    ; compare r to pi/2
        fstsw   -2[BP]          ; . . .
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
        fstsw   -2[BP]          ; . . .
        fwait                   ; . . .
        mov     AX,-2[BP]       ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if r > pi/4
          fsubr st,st(1)        ; - tan(r) = 1/tan(pi/2 - r)
          not   BX              ; - complement the 'invert' flag
        _endif                  ; endif
        fstp    st(1)           ; copy r up over pi/2
        do_fptan                ; st(1)/st(0) = tangent, st(0) = X, st(1) = Y
        or      BX,BX           ; if result is inverted
        _if     ne              ; then
          do_fdivrp 1,0         ; - 1/tangent = st(0)/st(1)
        _else                   ; else
          do_fdivp  1,0         ; - tangent = st(1)/st(0)
        _endif                  ; endif
        or      CX,CX           ; if result to be negated
        _if     ne              ; then
          fchs                  ; - negate result
        _endif                  ; endif
        fstp    qword ptr -8[BP]; store result
        fwait                   ; wait
        pop     DX              ; load result
        pop     CX              ; . . .
        pop     BX              ; . . .
        pop     AX              ; . . .
        pop     BP              ; restore BP
        ret                     ; return
        endproc __tan87

        endmod
        end
