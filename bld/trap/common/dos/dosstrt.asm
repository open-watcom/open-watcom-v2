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


                name    TRAPSTRT


if __LARGE__
public          _big_code_
_large_code_      equ 0

dgroup group    _DATA,_BSS,CONST,STACK
else
public          _small_code_
_small_code_      equ 0

dgroup group    _TEXT,_DATA,_BSS,CONST,STACK
endif

_TEXT           segment byte public 'CODE'

                extrn   TrapInit_               :far
                extrn   TrapRequest_            :far
                extrn   TrapFini_               :far

assume  CS:_TEXT

        org     0H

        start label near
validate        dw      0DEAFh
init            dw      TrapInit_
req             dw      TrapRequest_
fini            dw      TrapFini_


no_load         db      "Can not be run from the command line$"

dos_start       label far
                push    cs
                pop     ds
                mov     dx, offset no_load
                mov     ah,9H
                int     21H
                mov     ax, 4cffH
                int     21H

_TEXT   ends

_BSS            segment byte public 'BSS'
_BSS            ends

_DATA           segment byte public 'DATA'
_DATA           ends

CONST           segment byte public 'DATA'
CONST           ends

STACK           segment byte stack 'STACK'
    db          1   ; this causes the _BSS segment to be allocated
STACK           ends

        end     dos_start
