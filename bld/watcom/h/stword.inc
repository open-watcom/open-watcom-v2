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


; Define 8087 status word:
; ========================

ST_EF_IO   equ       0001h             ; invalid operation
ST_EF_DO   equ       0002h             ; denormalized operand
ST_EF_ZD   equ       0004h             ; zero divide
ST_EF_OF   equ       0008h             ; overflow flag
ST_EF_UF   equ       0010h             ; underflow flag
ST_EF_PR   equ       0020h             ; precision
ST_EF_SF   equ       0040h             ; stack under/overflow
ST_EF_ES   equ       0080h             ; error summary

ST_C0      equ       0100h             ; condition code 0
ST_C1      equ       0200h             ; condition code 1
ST_C2      equ       0400h             ; condition code 2
ST_C3      equ       4000h             ; condition code 3

ST_IR      equ       0080h             ; interrupt request

ST_ST      equ       3800h             ; stack top

ST_BF      equ       8000h             ; busy flag

; Define 8087 control word:
; =========================

                            ; Exception masks (1 = exception masked)
CW_IM      equ       0001h  ;   invalid operation
CW_DM      equ       0002h  ;   denormalized operand
CW_ZM      equ       0004h  ;   zero divide
CW_OM      equ       0008h  ;   overflow
CW_UM      equ       0010h  ;   underflow
CW_PM      equ       0020h  ;   precision

                            ; reserved - xxx0 0000 0x00 0000
CW_RESERVE equ      0e040h  ; RESERVED

                            ; interrupt enable mask 0000 0000 x000 0000
CW_IEM_EN  equ       0000h  ;   enabled
CW_IEM_DIS equ       0080h  ;   disabled

CW_PC_MASK equ      0fcffh  ; precision control - 0000 00xx 0000 0000
CW_PC_64   equ       0300h  ; precision control - 64 bits
CW_PC_53   equ       0200h  ; precision control - 53 bits
CW_PC_RES  equ       0100h  ; precision control - reserved
CW_PC_24   equ       0000h  ; precision control - 24 bits

CW_RC_MASK equ      0f3ffh  ; round control - 0000 xx00 0000 0000
CW_RC_CHOP equ       0c00h  ; round control - chop
CW_RC_UP   equ       0800h  ; round control - round up
CW_RC_DOWN equ       0400h  ; round control - round down
CW_RC_NEAR equ       0000h  ; round control - round nearest

                            ; infinity control - 000x 0000 0000 0000
CW_IC_PROJ equ       0000h  ;   projective
CW_IC_AFF  equ       1000h  ;   affine
