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


;========================================================================
;==     Name:           LDIV                                           ==
;==     Operation:      Signed 4 byte divide                           ==
;==     Inputs:         DX;AX   Dividend                               ==
;==                     CX;BX   Divisor                                ==
;==                     SS:SI   pointer to result structure            ==
;==     Outputs:        DX;AX   Quotient                               ==
;==                     CX;BX   Remainder (same sign as dividend)      ==
;==     Volatile:       none                                           ==
;========================================================================
include mdef.inc
include struct.inc

        modstart        ldiv086

        xref    __I4D

        defpe   ldiv
        xdefp   "C",ldiv
        docall  __I4D           ; do the division
    if _MODEL and (_BIG_DATA or _HUGE_DATA)
        mov     ss:[si],ax      ; store quotient
        mov     ss:2[si],dx     ; ...
        mov     ss:4[si],bx     ; store remainder
        mov     ss:6[si],cx     ; ...
    else
        mov     [si],ax         ; store quotient
        mov     2[si],dx        ; ...
        mov     4[si],bx        ; store remainder
        mov     6[si],cx        ; ...
    endif
        ret                     ; return
        endproc ldiv

        endmod
        end
