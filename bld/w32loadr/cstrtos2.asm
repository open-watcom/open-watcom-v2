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
;* Description:  Special OS/2 runtime library startup module for
;*               OSI programs.
;*
;*****************************************************************************


        name    cstrtos2
.386p
        assume  nothing

        extrn   __OS2Main       : near
        extrn   ___begtext      : near
        extrn   _end            : byte

_TEXT   segment use32 word public 'CODE'

        public  _cstart_
        public  _LaunchPgm_

_OS_OS2 equ     1

        assume  cs:_TEXT

_cstart_ proc near
        jmp     __OS2Main
_cstart_ endp

_LaunchPgm_ proc near
        push    cs              ; simulate far call
        mov     bx,cs           ; pass in our CS value
        mov     ah,_OS_OS2      ; indicate OS/2
        lea     ecx,_end        ; get address of stack low
        call    esi             ; invoke program
        ret                     ; return
_LaunchPgm_ endp

copyright proc  near
        dd      ___begtext      ; reference module with segment definitions
;
; copyright message
;
        db      "WATCOM C/C++32 Run-Time system. "
        db      "(c) Copyright by WATCOM International Corp. 1988-1995."
        db      " All rights reserved."
copyright endp

_TEXT   ends

        end     _cstart_
