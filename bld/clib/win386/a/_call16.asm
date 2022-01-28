;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  WIN386 interface for calling 16-bit Windows function
;*
;*****************************************************************************


;****************************************************************************
;***                                                                      ***
;*** _CALL16.ASM - interface to __Call16 to call a 16-bit PASCAL function ***
;***    DWORD _Call16( FARPROC lpFunc, char *fmt, ... );                  ***
;***                                                                      ***
;****************************************************************************

        extrn   _LocalPtr       : word
        extrn   __Call16Addr    : fword

_TEXT   segment use32 word public 'CODE'
        assume  cs:_TEXT

        public  "C",_Call16
_Call16 proc   near
        push    ebp                     ; save registers
        push    edi                     ; ...
        push    esi                     ; ...
        push    ebx                     ; ...
        push    ecx                     ; ...
        push    edx                     ; ...
        lea     edi,((6+1)*4)[esp]      ; point to parms
        mov     ds,_LocalPtr            ; load up extenders DS
        call    fword ptr es:__Call16Addr; invoke _call16 routine in extender
        pop     edx                     ; restore registers
        pop     ecx                     ; ...
        pop     ebx                     ; ...
        pop     esi                     ; ...
        pop     edi                     ; ...
        pop     ebp                     ; ...
        ret                             ; return
_Call16 endp

_TEXT   ends
        end
