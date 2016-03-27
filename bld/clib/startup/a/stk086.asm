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
;* Description:  Stack checking for 16-bit DOS and Win16.
;*
;*****************************************************************************


include mdef.inc
include struct.inc
include exitwmsg.inc

        modstart        stk

datasegment
        extrn   "C",_STACKLOW : word
SS_seg  dw      0
enddata

include xinit.inc

        xinit   _init_stk,DEF_PRIORITY

        assume  ds:DGROUP

        xdefp   __STK
        xdefp   "C",__STKOVERFLOW

        defp    _init_stk
        mov     SS_seg,SS               ; save original SS value
        ret                             ; return
_init_stk endp

        defpe   __STK
        _guess                          ; guess: no overflow
          cmp   ax,sp                   ; - quit if user asking for too much
          _quif ae                      ; - . . .
          sub   ax,sp                   ; - calculate new low point
if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
          push  ds                      ; - save DS
          push  ax                      ; - save AX
          mov   ax,DGROUP               ; - get access to DGROUP
          mov   ds,ax                   ; - . . .
          pop   ax                      ; - restore AX
endif
          neg   ax                      ; - calc what new SP would be
          cmp   ax,_STACKLOW            ; - if too much
          _if   be                      ; - then

;       We could have reached this point because we are in an interrupt
;       routine or DLL with different SS:SP values from our DGROUP.

            mov   ax,ss                 ; - - get ss
            cmp   ax,SS_seg             ; - - see if SS has been changed
            je    __STKOVERFLOW         ; - - stack overflow if same
          _endif                        ; - endif

if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
          pop   ds                      ; - restore DS
endif
          ret                           ; - return
        _endguess                       ; endguess

__STKOVERFLOW:
        push    cs                      ; set dx=cs
        pop     dx                      ; . . .
        mov     ax,offset msg           ; get msg
        mov     bx,1                    ; error code
        jmp     __fatal_runtime_error   ; display msg and exit
        endproc __STK

msg     db      "Stack Overflow!", 0
        endmod
        end
