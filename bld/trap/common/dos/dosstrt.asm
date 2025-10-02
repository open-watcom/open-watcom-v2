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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


                name    TRAPSTRT


public  _small_code_
_small_code_    equ 0

DGROUP  group   _DATA,_BSS,CONST,STACK

BEGTEXT         segment byte public 'CODE'
        assume  CS:BEGTEXT

        extrn   TrapInit_               :near
        extrn   TrapRequest_            :near
        extrn   TrapFini_               :near

sign    dw      0DEAFh
init    dw      TrapInit_
req     dw      TrapRequest_
fini    dw      TrapFini_

no_load db      "Can not be run from the command line$"

dos_start label far
        push    cs
        pop     ds
        mov     dx, offset no_load
        mov     ah,9H
        int     21H
        mov     ax, 4cffH
        int     21H

BEGTEXT ends

_TEXT   segment byte public 'CODE'
_TEXT   ends

_BSS    segment byte public 'BSS'
_BSS    ends

_DATA   segment byte public 'DATA'
_DATA   ends

CONST   segment byte public 'DATA'
CONST   ends

STACK   segment byte stack 'STACK'
    db  1   ; this causes the _BSS segment to be allocated
STACK   ends

        end     dos_start
