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
include exitwmsg.inc

        modstart        stk

_DATA   segment dword public 'DATA'
        extrn   "C",_STACKLOW : dword
SS_seg  dw      0
msg     db      "Stack Overflow!", 0dh, 0ah, 0
_DATA   ends

include xinit.inc

        xinit   _init_stk,DEF_PRIORITY

        assume  ds:DGROUP

        xdefp       __STK
        xdefp       __CHK
        xdefp       __GRO
        if __WASM__ ge 100
            xdefp  "C",__STKOVERFLOW
        else
            xdefp  <"C",__STKOVERFLOW>
        endif


        defp    _init_stk
        mov     SS_seg,SS       ; save original SS value
        ret                     ; return
        endproc _init_stk


        defpe   __CHK
        xchg    eax,4[esp]
        call    __STK
        mov     eax,4[esp]
        ret     4
        endproc __CHK


        defpe   __GRO
        ret     4
        endproc __GRO


        defpe   __STK
        _guess                  ; guess: no overflow
          cmp   eax,esp         ; - quit if user asking for too much
          _quif ae              ; - . . .
          sub   eax,esp         ; - calculate new low point
          neg   eax             ; - calc what new SP would be
          cmp   eax,_STACKLOW   ; - quit if too much
          _quif be              ; - . . .
          ret                   ; - return
        _endguess               ; endguess

;       We could have reached this point because we are in an interrupt
;       routine with different SS:ESP values from our DGROUP.

        _guess                  ; guess: SS != DS       07-nov-89
          mov   ax,ss           ; - get ss
          cmp   ax,SS_seg       ; - see if SS has been changed
          _quif e               ; - quit if same
          ret                   ; - return (running with a different stack)
        _endguess               ; endguess


__STKOVERFLOW:
ifdef __STACK__
        push    1
        push    offset msg
else
        mov     eax,offset msg
        mov     edx,1
endif
        call    __fatal_runtime_error
        endproc __STK

        endmod
        end
