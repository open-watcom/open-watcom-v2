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


;
;       __Int21 interface to OS int 21h translator
;
.386p

extrn   __Int21C_ :near

_TEXT segment word public 'CODE'
assume  cs:_TEXT

        public  __Int21
        public  __fInt21

__fInt21 proc  far
        call    __Int21
        ret
__fInt21 endp

__Int21 proc   near
;
;       create a union REGS by pushing registers onto stack in correct order
;
        push    edi                     ; push edi
        push    esi                     ; ...
        push    edx                     ; ...
        push    ecx                     ; ...
        push    ebx                     ; ...
        push    eax                     ; ...
        mov     eax,esp                 ; point to structure
        call    __Int21C_               ; call OS int21 translator
        sahf                            ; set flags (carry)
        pop     eax                     ; restore regs from structure
        pop     ebx                     ; ...
        pop     ecx                     ; ...
        pop     edx                     ; ...
        pop     esi                     ; ...
        pop     edi                     ; ...
        ret                             ; return
__Int21 endp

_TEXT   ends
        end
