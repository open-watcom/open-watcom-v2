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
;* Description:  Stack checking for RDOS device-driver.
;*
;*****************************************************************************


include mdef.inc
include struct.inc
include xinit.inc
include exitwmsg.inc

        modstart        stk

        xinit   _init_stk,DEF_PRIORITY

        assume  ds:DGROUP


        defp    _init_stk
        ret                             ; return
        endproc _init_stk


        xdefp   __CHK
        defpe   __CHK                   ; new style stack check
        movzx   esp,sp                  ; make sure high part of ESP i valid
        push    ebp
        mov     ebp,esp
        sub     ebp,[ebp+8]
        jnc     chk_passed
;
        int     3                       ; hit breakpoint if stack is out        

chk_passed:        
        pop     ebp
        ret     4
        endproc __CHK

        endmod
        end
