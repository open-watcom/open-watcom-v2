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
;* Description:  Win32 console startup code adapted for OSI executables.
;*
;*****************************************************************************


        name    cstrtwnt
.386p
        assume  nothing

        extrn   __NTMain        : near
        extrn   ___begtext      : near

_TEXT   segment use32 word public 'CODE'

        public  _cstart_
        public  _LaunchPgm_

OS_NT   equ     2

        assume  cs:_TEXT

_cstart_ proc near
        jmp     __NTMain
_cstart_ endp

_LaunchPgm_ proc near
        push    cs              ; simulate far call
        mov     bx,cs           ; pass in our CS value
        mov     ah,OS_NT        ; indicate NT
        sub     ecx,ecx         ; get address of stack low
        call    esi             ; invoke program
        ret                     ; return
_LaunchPgm_ endp

copyright proc  near
        dd      ___begtext      ; reference module with segment definitions
;
; copyright message
;
        db      "Open Watcom C/C++32 Run-Time system. "
        db      "Portions Copyright (C) Sybase, Inc. 1988-2002."
copyright endp

_TEXT   ends

        end     _cstart_
