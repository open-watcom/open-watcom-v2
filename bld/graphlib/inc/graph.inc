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


ifdef _386

    .386                ; allow generation of 386 code
    extptr      macro var
                extrn var : dword
                endm
    func_table  macro table,list
                table dd list
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

    eax equ <ax>        ; map 32-bit registers to 16-bit
    ebx equ <bx>
    ecx equ <cx>
    edx equ <dx>
    esi equ <si>
    edi equ <di>
    ebp equ <bp>
    movzx equ <mov>
    extptr      macro var
                extrn var : word
                endm
    doicall     macro func
                call dword ptr func
                endm
    func_table  macro table,list
                table dw list
                endm
    _retf       macro
                retf
                endm

endif

include mdef.inc
include struct.inc
