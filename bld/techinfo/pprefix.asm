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


;***********************************************************************
;                                                                      *
; PPrefix - Determine Whether Intel 8086/8088                          *
;           or NEC V30/V20, Intel 80186/80188/80286/80386              *
;                                                                      *
; int PPrefix()                                                        *
;       returns:                                                       *
;               0 - Intel 8086/8088                                    *
;               1 - NEC V30/V20, Intel 80186/80188/80286/80386         *
;                                                                      *
;***********************************************************************
NOPINS  equ     90h
DECBXINS equ    4Bh

_TEXT   segment byte public 'code'
        assume  CS:_TEXT

        public  PPrefix_
PPrefix_ proc    near
        PUSH    CX                      ; save CX
        PUSH    SI                      ; save SI
        MOV     CX,0FFFFh               ; set counter
REP     LODS    byte ptr ES:PPrefix_    ; will we remember both prefixes?
        MOV     AX,1                    ; assume not Intel 8086/8088
        JCXZ    PRN86                   ; if CX <> 0 then
        DEC     AX                      ; - it's an Intel 8086/8088
PRN86:                                  ; end if
        POP     SI                      ; restore SI
        POP     CX                      ; restore CX
        RET                             ; return to caller

PPrefix_ endp
_TEXT   ends
        end
