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


; These compiler support routines are called by the Fast code generator
; when compiling with the -fpc option.
;
        name    fdmath

        extrn   __FDA : near
        extrn   __FDS : near
        extrn   __FDM : near
        extrn   __FDD : near
        extrn   __FDC : near

_TEXT   segment use32 dword public 'CODE'
        assume  cs:_TEXT

        public  __FADD
__FADD  proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ebx,12[esp]     ; load op2
        mov     ecx,16[esp]     ; ...
        call    __FDA           ; do the add
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op2
__FADD  endp

        public  __FSUB
__FSUB  proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ebx,12[esp]     ; load op2
        mov     ecx,16[esp]     ; ...
        call    __FDS           ; do the subtract
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op2
__FSUB  endp

        public  __FSUBR
__FSUBR proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ecx,edx         ; load op2
        mov     ebx,eax         ; ...
        mov     eax,12[esp]     ; load op1
        mov     edx,16[esp]     ; ...
        call    __FDS           ; do the subtract
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op1
__FSUBR endp

        public  __FMUL
__FMUL  proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ebx,12[esp]     ; load op2
        mov     ecx,16[esp]     ; ...
        call    __FDM           ; do the multiply
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op2
__FMUL  endp

        public  __FDIV
__FDIV  proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ebx,12[esp]     ; load op2
        mov     ecx,16[esp]     ; ...
        call    __FDD           ; do the divide
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op2
__FDIV  endp

        public  __FDIVR
__FDIVR proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ecx,edx         ; load op2
        mov     ebx,eax         ; ...
        mov     eax,12[esp]     ; load op1
        mov     edx,16[esp]     ; ...
        call    __FDD           ; do the divide
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op1
__FDIVR endp

        public  __FCMP
__FCMP  proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ebx,12[esp]     ; load op2
        mov     ecx,16[esp]     ; ...
        call    __FDC           ; do the compare
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op2
__FCMP  endp

        public  __FCMPR
__FCMPR proc    near
        push    ecx             ; save regs
        push    ebx             ; ...
        mov     ecx,edx         ; load op2
        mov     ebx,eax         ; ...
        mov     eax,12[esp]     ; load op1
        mov     edx,16[esp]     ; ...
        call    __FDC           ; do the compare
        pop     ebx             ; restore regs
        pop     ecx             ; ...
        ret     8               ; return and remove op1
__FCMPR endp

_TEXT   ends
        end
