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

        modstart        fstat086

        xref    "C",__set_ERANGE

        datasegment
        codeptr ___FPE_handler
        xdefp   _FPStatus_
_FPStatus_      db      0
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
        sub     cx,cx           ; . . .
        sub     bx,bx           ; . . .

        defpe   F4UnderFlow
        push    ds              ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
endif
        mov     byte ptr _FPStatus_,FPS_UNDERFLOW
;;      mov     ax,FPE_UNDERFLOW; indicate underflow
;;      call    ___FPE_handler  ;
        pop     ds              ; restore DS
        sub     ax,ax           ; return a zero
        sub     dx,dx           ; . . .
        ret                     ; return
        endproc F4UnderFlow
        endproc F8UnderFlow


        defpe   F8InvalidOp
        mov     ax,FPE_ZERODIVIDE; indicate divide by 0
        call    ___FPE_handler   ;
        jmps    F8RetInf9       ; return infinity
        endproc F8InvalidOp

        defpe   F8DivZero
        push    ax              ; save AX
        push    ds              ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
endif
        mov     byte ptr _FPStatus_,FPS_DIVIDE_BY_0
        mov     ax,FPE_ZERODIVIDE; indicate divide by 0
        call    ___FPE_handler  ;
        pop     ds              ; restore DS
        pop     ax              ; restore AX
        jmps    F8RetInf9       ; return infinity
        endproc F8DivZero

        defpe   F8OverFlow
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        push    ax              ; save AX
        push    ds              ; save DS
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
        mov     byte ptr _FPStatus_,FPS_OVERFLOW
        pop     ds              ; restore DS
        pop     ax              ; restore AX
else
        mov     byte ptr _FPStatus_,FPS_OVERFLOW
endif


;
;       F8RetInf( sign : int ) : reallong
;
        defpe   F8RetInf_
        defpe   _F8RetInf_
        push    ax              ; save AX
        push    ds              ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
endif
        call    __set_ERANGE   ; errno = ERANGE
        mov     ax,FPE_OVERFLOW ; indicate overflow
        call    ___FPE_handler  ;
        pop     ds              ; restore DS
        pop     ax              ; restore AX
F8RetInf9: and  ax,8000h        ; get sign
        or      AX,7ff0h        ; set result to infinity
        sub     BX,BX           ; ...
        sub     CX,CX           ; ...
        sub     DX,DX           ; ...
        ret                     ; return
        endproc _F8RetInf_
        endproc F8RetInf_
        endproc F8OverFlow


        defpe   F4DivZero
        push    ax              ; save AX
        push    ds              ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
endif
        mov     byte ptr _FPStatus_,FPS_DIVIDE_BY_0
        mov     ax,FPE_ZERODIVIDE; indicate divide by 0
        call    ___FPE_handler  ;
        pop     ds              ; restore DS
        pop     ax              ; restore AX
        jmps    F4RetInf9       ; return infinity
        endproc F4DivZero

        defpe   F4OverFlow
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        push    ax              ; save AX
        push    ds              ; save DS
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
        mov     byte ptr _FPStatus_,FPS_OVERFLOW
        pop     ds              ; restore DS
        pop     ax              ; restore AX
else
        mov     byte ptr _FPStatus_,FPS_OVERFLOW
endif

        defp    F4RetInf        ; return infinity
        push    ax              ; save sign of result
        push    ds              ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP       ; get access to data segment
        mov     ds,ax           ; . . .
endif
        call    __set_ERANGE   ; errno = ERANGE
        mov     ax,FPE_OVERFLOW ; indicate overflow
        call    ___FPE_handler  ;
        pop     ds              ; restore DS
        pop     ax              ; restore sign of result
F4RetInf9: and  ax,8000h        ; get sign
        or      AX,7f80h        ; set result to infinity
        mov     DX,AX           ; ...
        sub     AX,AX           ; ...
        ret                     ; return
        endproc F4RetInf
        endproc F4OverFlow


        endmod
        end
