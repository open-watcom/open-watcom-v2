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
;* Description:  DOS32GETMESSAGE stub which munges arguments and calls
;*               DOS32TRUEGETMESSAGE
;*
;*****************************************************************************

.386p
.model flat

    PUBLIC    sig32
;    PUBLIC    DOSGETMESSAGE
;    PUBLIC    DOS32GETMESSAGE
    PUBLIC    DosGetMessage
    PUBLIC    Dos32GetMessage
    EXTRN     DOS32TRUEGETMESSAGE:BYTE

.code _MSGSEG32

sig32:
    db        0FFh, 'MSGSEG32', 0, 1, 80h, 0, 0
    dd        offset L1

;DOSGETMESSAGE:
;DOS32GETMESSAGE:
DosGetMessage:
Dos32GetMessage:
    lea       eax,sig32
    push      eax
    push      ebp
    mov       ebp,esp
    mov       eax,dword ptr 4H[ebp]
    xchg      eax,dword ptr 8H[ebp]
    mov       dword ptr 4H[ebp],eax
    pop       ebp
    jmp       near ptr FLAT:DOS32TRUEGETMESSAGE
    db        0FFh
L1:
    dw        0
    dw        0FFFFh

              END
