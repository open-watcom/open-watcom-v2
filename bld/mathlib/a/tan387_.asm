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

.287
        modstart        tan387

        xdefp   __tan87         ; calc tan(fac1)

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     TAN function for 8087                                           <>
;<>                                                                     <>
;<>     tan8087 - compute tan of st(0)                                  <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

ifdef __386__
    datasegment
endif
piby2   dt      1.57079632679489661923
piby4   dt      0.78539816339744830961
ifdef __386__
    enddata
endif


        defp    __tan87
        push    EBP             ; save BP
        mov     EBP,ESP         ; get access to stack
        push    EDX             ; push argument
        push    EAX             ; . . .
        fldpi                   ; load pi
        xor     DX,DX           ; init 'invert' flag / init 'negate' flag
        fld     qword ptr -8[EBP]; load argument
        ftst                    ; get sign of argument
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if argument is negative
          fchs                  ; - make argument positive
          not   DH              ; - indicate result to be negated
        _endif                  ; endif
        _loop                   ; loop (reduce argument to mod pi)
          fprem                 ; - calculate partial remainder
          fstsw   AX            ; - store status word
          sahf                  ; - ... PF = C2
        _until  np              ; until C2 is clear
        fld     tbyte ptr piby2 ; load pi/2
        fstp    st(2)           ; stack is r, pi/2
        fcom                    ; compare r to pi/2
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     a               ; if r > pi/2
          fsub  st,st(1)        ; - tan(r) = -1/tan(r - pi/2)
          not   DL              ; - set the 'invert' flag
          not   DH              ; - complement the 'negate' flag
        _endif                  ; endif
        fld     tbyte ptr piby4 ; load pi/4
        fcomp                   ; compare pi/4 to r and pop
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if r > pi/4
          fsubr st,st(1)        ; - tan(r) = 1/tan(pi/2 - r)
          not   DL              ; - complement the 'invert' flag
        _endif                  ; endif
        fstp    st(1)           ; copy r up over pi/2
        do_fptan                ; st(1)/st(0) = tangent, st(0) = X, st(1) = Y
        cmp     DL,0            ; if result is inverted
        _if     ne              ; then
          do_fdivrp 1,0         ; - 1/tangent = st(0)/st(1)
        _else                   ; else
          do_fdivp  1,0         ; - tangent = st(1)/st(0)
        _endif                  ; endif
        cmp     DH,0            ; if result to be negated
        _if     ne              ; then
          fchs                  ; - negate result
        _endif                  ; endif
        fstp    qword ptr -8[EBP]; store result
        fwait                   ; wait
        pop     EAX             ; load result
        pop     EDX             ; . . .
        pop     EBP             ; restore BP
        ret                     ; return
        endproc __tan87

        endmod
        end
