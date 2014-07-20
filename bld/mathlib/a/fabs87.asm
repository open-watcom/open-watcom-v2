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
include math87.inc

        xrefp    __8087  ; indicate that NDP instructions are present

        modstart fabs87

        xdefp    "C",fabs       ; double fabs( double x )

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
endif

;  input:       x - on the stack
;  output:      absolute value of x in st(0)
;
        defp    fabs
ifdef __386__
        and     byte ptr 4+7[ESP],7Fh   ; turn off sign bit
ifdef __STACK__
        mov     EAX,4[ESP]              ; load value
        mov     EDX,8[ESP]              ; ...
else
        fld     qword ptr 4[ESP]        ; load value
        fwait                           ; wait
endif
else
        push    BP                      ; save BP
        mov     BP,SP                   ; get access to stack
        fld     qword ptr argx[BP]      ; load argument
        fabs                            ; take absolute value
        pop     BP                      ; restore BP
endif
        ret_pop 8                       ; return
        endproc fabs

        public  IF@DABS
        public  IF@ABS
        defp    IF@DABS
        defp    IF@ABS
        fabs                            ; calculate absolute value of argument
        ret                             ; return
        endproc IF@ABS
        endproc IF@DABS

        endmod
        end
