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
        modstart        atan387

        xdefp   __atan87        ; calc atan(fac1)


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                     <>
;<>     _atan   - compute arctan(st0) using 8087                        <>
;<>                                                                     <>
;<>     input:  EDX:EAX                                                 <>
;<>     output: EDX:EAX                                                 <>
;<>                                                                     <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
        defp    __atan87
        push    EBP             ; save BP
        mov     EBP,ESP         ; get access to stack
        push    EDX             ; push argument
        push    EAX             ; . . .
        fld     qword ptr -8[EBP]; load argument
        mov     DL,0            ; init 'negate' flag

        ftst                    ; get sign of argument
        fstsw   AX              ; . . .
        sahf                    ; get flags ZF = C3, CF = C0
        _if     b               ; if argument is negative
          fchs                  ; - make argument positive
          not   DL              ; - indicate result to be negated
        _endif                  ; endif

        fld1                    ; load 1.0
        fcom    st(1)           ; compare argument with 1.0
        fstsw   AX              ; get result of compare
        and     AX,4100h        ; isolate condition codes
        _if     ne              ; if argument > 1.0
          fxch  st(1)           ; - st = arg, st1 = 1.0
          do_fpatan             ; - calculate arctangent(1/argument)
          mov   AX,2            ; - compute pi/2 - atn(1/argument)
          mov   -2[EBP],AX      ; - push 2
          fldpi                 ; - load pi
          fidiv word ptr -2[EBP]; - calculate pi/2
          fsubrp st(1),st(0)    ; - calculate pi/2 - atn(1/argument)
        _else                   ; else
          do_fpatan             ; - calculate artangent(argument)
        _endif                  ; endif
        cmp     DL,0            ; if original argument was negative
        _if     ne              ; then
          fchs                  ; - negate the answer
        _endif                  ; endif
        fstp    qword ptr -8[EBP]; store result
        fwait                   ; wait
        pop     EAX             ; load result
        pop     EDX             ; . . .
        pop     EBP             ; restore BP
        ret                     ; return
        endproc __atan87

        endmod
        end
