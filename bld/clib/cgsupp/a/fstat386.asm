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


;                               of AX:DX
;
include mdef.inc
include struct.inc
include fstatus.inc

        modstart        fstat386

        xref    "C",__set_ERANGE
        xref    __FPE_exception_

        datasegment
        enddata
        assume  ss:nothing


FPS_OK                  =       0
FPS_UNDERFLOW           =       1
FPS_OVERFLOW            =       2
FPS_DIVIDE_BY_0         =       3
FPS_BAD_ARG             =       4

jmps    macro   dsym
        jmp     short dsym
        endm


        xdefp   F8UnderFlow
        xdefp   F4UnderFlow
        xdefp   F8InvalidOp
        xdefp   F8DivZero
        xdefp   F4DivZero
        xdefp   F8OverFlow
        xdefp   F4OverFlow
        xdefp   F8RetInf_
        xdefp   _F8RetInf_

        defpe   F8UnderFlow
        sub     EDX,EDX         ; . . .

        defpe   F4UnderFlow
;;      mov     EAX,FPE_UNDERFLOW; indicate underflow
;;      call    __FPE_exception_;
        sub     EAX,EAX         ; return a zero
        ret                     ; return
        endproc F4UnderFlow
        endproc F8UnderFlow


        defpe   F8InvalidOp
        mov     EAX,FPE_ZERODIVIDE; indicate divide by 0
        call    __FPE_exception_;
        jmps    F8RetInf9       ; return infinity
        endproc F8InvalidOp

        defpe   F8DivZero
        mov     EAX,FPE_ZERODIVIDE; indicate divide by 0
        call    __FPE_exception_;
        jmps    F8RetInf9       ; return infinity
        endproc F8DivZero

        defpe   F8OverFlow
;
;       F8RetInf( sign : int ) : reallong
;
        defpe   F8RetInf_
        defpe   _F8RetInf_
        push    EAX             ; save sign of result
        call    __set_ERANGE    ; errno = ERANGE
        mov     EAX,FPE_OVERFLOW ; indicate overflow
        call    __FPE_exception_;
        pop     EAX             ; restore sign of result
F8RetInf9: and  EAX,80000000h   ; get sign
        or      EAX,7FF00000h   ; set infinity
        mov     EDX,EAX         ;
        sub     EAX,EAX         ; ...
        ret                     ; return
        endproc _F8RetInf_
        endproc F8RetInf_
        endproc F8OverFlow


        defpe   F4DivZero
        mov     EAX,FPE_ZERODIVIDE; indicate divide by 0
        call    __FPE_exception_;
        jmps    F4RetInf9       ; return infinity
        endproc F4DivZero

        defpe   F4OverFlow
        defp    F4RetInf        ; return infinity
        push    EAX             ; save sign of result
        call    __set_ERANGE    ; errno = ERANGE
        mov     EAX,FPE_OVERFLOW ; indicate overflow
        call    __FPE_exception_;
        pop     EAX             ; restore sign of result
F4RetInf9: and  EAX,80000000h   ; get sign
        or      EAX,7F800000h
        ret
        endproc F4RetInf
        endproc F4OverFlow


        endmod
        end
