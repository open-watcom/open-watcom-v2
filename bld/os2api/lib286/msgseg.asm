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
;* Description:  DOSGETMESSAGE stub which munges arguments and calls
;*               DOSTRUEGETMESSAGE
;*
;*****************************************************************************

.286p

    PUBLIC    DOSGETMESSAGE
    PUBLIC    MSGSEGDATA
    EXTRN     DOSTRUEGETMESSAGE:BYTE

MESSAGES      GROUP   _MSGSEG

_MSGSEG       SEGMENT PARA PUBLIC USE16 'MSGSEGCODE'
              ASSUME CS:MESSAGES

    db        0FFh, 'MKMSGSEG', 0, 1, 0, 0, 0, 0, 0

DOSGETMESSAGE:
    xor       ax,ax
    push      ax
    push      cs
    push      bp
    mov       bp,sp
    xchg      ax,word ptr 6H[bp]
    xchg      ax,word ptr 2H[bp]
    xchg      ax,word ptr 8H[bp]
    mov       word ptr 4H[bp],ax
    pop       bp
    jmp       far ptr DOSTRUEGETMESSAGE

MSGSEGDATA:
    dw        0
_MSGSEG       ENDS
              END
