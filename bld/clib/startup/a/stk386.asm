;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Stack checking for 32-bit DOS and Win386.
;*
;*****************************************************************************


include langenv.inc
include mdef.inc
include struct.inc
include exitwmsg.inc

        modstart        stk

datasegment
        extrn   "C",_STACKLOW : dword
SS_seg  dw      0
msg     db      "Stack Overflow!", 0
enddata

        assume  ds:DGROUP

        xdefp   __STK
        xdefp   __CHK
        xdefp   __GRO
        xdefp   "C",__STKOVERFLOW


        defp    _init_stk
        mov     SS_seg,SS               ; save original SS value
        ret                             ; return
        endproc _init_stk

        defpe   __GRO
        ret     4
        endproc __GRO

        defpe   __STK
        xchg    eax,[esp]               ; exchange parm with return addr
        push    eax                     ; push return addr
        ; fall into __CHK

        defpe   __CHK
        push    eax                     ; save eax
        mov     eax,8[esp]              ; get parameter
        _guess                          ; guess: no overflow
          cmp   eax,esp                 ; - check if user asking for too much
        _quif ae                        ; quit if user asking for too much
          sub   eax,esp                 ; - calculate new low point
          neg   eax                     ; - calc what new SP would be
          cmp   eax,_STACKLOW           ; - check if too much
          _if be                        ; - if too much
;       We could have reached this point because we are in an interrupt
;       routine with different SS:ESP values from our DGROUP.
            mov   ax,ss                 ; - - get ss
            cmp   ax,SS_seg             ; - - see if SS has been changed
          _endif                        ; - endif
        _quif e                         ; quit if too much
          pop   eax                     ; - restore EAX
          ret   4                       ; - return (running with a different stack)
        _endguess                       ; endguess
        pop     eax                     ; throw away saved eax value

__STKOVERFLOW:
ifdef __STACK__
        push    1                       ; exit code
        push    offset msg              ; the error message
else
        mov     eax,offset msg          ; the error message
        mov     edx,1                   ; exit code
endif
        jmp     __fatal_runtime_error   ; display msg and exit
        ; never return
        endproc __CHK
        endproc __STK

        endmod

include xinit.inc

        xinit   _init_stk,DEF_PRIORITY

        end
