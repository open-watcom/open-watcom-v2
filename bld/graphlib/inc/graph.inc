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

ifdef __386__

    .386                ; allow generation of 386 code
    _eax equ eax        ; map 32-bit registers to 32-bit
    _ebx equ ebx
    _ecx equ ecx
    _edx equ edx
    _esi equ esi
    _edi equ edi
    _ebp equ ebp
    _movzx equ movzx
    extptr      macro var
                extrn var : dword
                endm
    ifdef _QNX
        doicall     macro func
                    call pword ptr func
                    endm
        _retf       macro
                    db 0cbh     ;retf
                    endm
    else
        doicall     macro func
                    call dword ptr func
                    endm
        _retf       macro
                    ret
                    endm
    endif

else

    _eax equ ax        ; map 32-bit registers to 16-bit
    _ebx equ bx
    _ecx equ cx
    _edx equ dx
    _esi equ si
    _edi equ di
    _ebp equ bp
    _movzx equ mov
    extptr      macro var
                extrn var : word
                endm
    doicall     macro func
                call dword ptr func
                endm
    _retf       macro
                retf
                endm

endif
