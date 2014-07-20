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
;* Description:  Floating-point exception signaling
;*
;*****************************************************************************


include mdef.inc
include struct.inc
include fstatus.inc

        modstart        fstat086

        xrefp   "C",__set_ERANGE

        datasegment
        extrn   "C",__FPE_handler:dword
        enddata

        assume  ss:nothing

        xdefp   FPUnderFlow
        xdefp   FPInvalidOp
        xdefp   FPDivZero
        xdefp   FPOverFlow
        xdefp   F8UnderFlow
;        xdefp   F8InvalidOp
        xdefp   F8DivZero
        xdefp   F8OverFlow
        xdefp   F8RetInf
        xdefp   F4UnderFlow
;        xdefp   F4InvalidOp
        xdefp   F4DivZero
        xdefp   F4OverFlow
        xdefp   F4RetInf

;
;       FPUnderFlow( void ) : void
;
        defpe   FPUnderFlow
if 0
        push    ax                  ; save AX
        push    ds                  ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP           ; get access to data segment
        mov     ds,ax               ; . . .
endif
;;      mov     ax,FPE_UNDERFLOW    ; indicate underflow
;;      call    __FPE_handler       ;
        pop     ds                  ; restore DS
        pop     ax                  ; restore AX
endif
        ret                         ; return
        endproc FPUnderFlow

;
;       FPInvalidOp( void ) : void
;
        defpe   FPInvalidOp
        push    ax                  ; save AX
        push    ds                  ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP           ; get access to data segment
        mov     ds,ax               ; . . .
endif
        mov     ax,FPE_ZERODIVIDE   ; indicate divide by 0
        call    __FPE_handler       ;
        pop     ds                  ; restore DS
        pop     ax                  ; restore AX
        ret                         ; return
        endproc FPInvalidOp

;
;       FPDivZero( void ) : void
;
        defpe   FPDivZero
        push    ax                  ; save AX
        push    ds                  ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP           ; get access to data segment
        mov     ds,ax               ; . . .
endif
        mov     ax,FPE_ZERODIVIDE   ; indicate divide by 0
        call    __FPE_handler       ;
        pop     ds                  ; restore DS
        pop     ax                  ; restore AX
        ret                         ; return
        endproc FPDivZero

;
;       FPOverFlow( void ) : void
;
        defpe   FPOverFlow
        push    ax                  ; save AX
        push    ds                  ; save DS
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        mov     ax,DGROUP           ; get access to data segment
        mov     ds,ax               ; . . .
endif
        call    __set_ERANGE        ; errno = ERANGE
        mov     ax,FPE_OVERFLOW     ; indicate overflow
        call    __FPE_handler       ;
        pop     ds                  ; restore DS
        pop     ax                  ; restore AX
        ret                         ; return
        endproc FPOverFlow

;
;       F8UnderFlow( void ) : reallong
;
        defp    F8UnderFlow
        xor     bx,bx               ; return zero
        xor     cx,cx               ; . . .
;
;       F4UnderFlow( void ) : real
;
        defp    F4UnderFlow
        lcall   FPUnderFlow         ;
        xor     ax,ax               ; return zero
        xor     dx,dx               ; . . .
        ret                         ; return
        endproc F4UnderFlow
        endproc F8UnderFlow

;
;       F4DivZero( sign : int ) : real
;
        defp    F4DivZero
        lcall   FPDivZero           ; handle divide by 0
        jmp short F4RetInf          ; return Infinity
;
;       F4OverFlow( sign : int ) : real
;
        defp    F4OverFlow
        lcall   FPOverFlow          ; handle overflow
;
;       F4RetInf( sign : int ) : real
;
        defp    F4RetInf
        and     ax,8000h            ; get sign
        or      ax,7f80h            ; set result to infinity
        mov     dx,ax               ; ...
        sub     ax,ax               ; ...
        ret                         ; return
        endproc F4RetInf
        endproc F4OverFlow
        endproc F4DivZero

;
;       F8DivZero( sign : int ) : reallong
;
        defp    F8DivZero
        lcall   FPDivZero           ; handle divide by 0
        jmp short F8RetInf          ; return Infinity
;
;       F8OverFlow( sign : int ) : reallong
;
        defp    F8OverFlow
        lcall   FPOverFlow          ; handle overflow
;
;       F8RetInf( sign : int ) : reallong
;
        defp    F8RetInf
        and     ax,8000h            ; get sign
        or      ax,7ff0h            ; set result to infinity
        sub     bx,bx               ; ...
        sub     cx,cx               ; ...
        sub     dx,dx               ; ...
        ret                         ; return
        endproc F8RetInf
        endproc F8OverFlow
        endproc F8DivZero

        endmod
        end
