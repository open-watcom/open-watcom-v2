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

        xrefp       "C",matherr
        modstart    __mtherr

_DATA   segment word public 'DATA'
_DATA   ends
        DGROUP  group   _DATA
        assume  ds:DGROUP

        xdefp   __matherr       ; int __matherr( struct exception * );

        defp    __matherr
ifdef __386__
        jmp     matherr        ; just jump to users matherr function
else
 if ((_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0))
        prolog
        push    DS              ; save DS
        push    CX              ; save CX
        mov     CX,DGROUP       ; set DS = DGROUP
        mov     DS,CX           ; ...
        call    matherr        ; call users matherr function
        pop     CX              ; restore CX
        pop     DS              ; restore DS
        epilog
        ret                     ; return
 else
        jmp     matherr        ; just jump to users matherr function
 endif
endif
        endproc __matherr

        endmod
        end
