;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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

; Define 80x87 environment:
; =========================
;
;    ---------------<--- offsets relative  to here
;    |             | stack grows
;    |             |  this way
;    |             |     /|\
;    |             |      |
;    |             |
;    ---------------

FPU_REGS    STRUC
        REGISTER_0   DT      ?
        REGISTER_1   DT      ?
        REGISTER_2   DT      ?
        REGISTER_3   DT      ?
        REGISTER_4   DT      ?
        REGISTER_5   DT      ?
        REGISTER_6   DT      ?
        REGISTER_7   DT      ?
FPU_REGS    ENDS


; Define 8087 environment:
; ========================

ENV87_AREA  STRUC
        ENV87_CONTROL_WORD      DW      ?
        ENV87_STATUS_WORD       DW      ?
        ENV87_TAG_WORD          DW      ?
        ENV87_IP_OFFSET         DW      ?
        ENV87_CS_SLCT           DW      ?
        ; ENV87_CS_SLCT field value bits encoding
        ; 0-7       opcode 2-nd byte
        ; 8-10      opcode 1-st byte low 3 bits
        ; 11        reserved
        ; 12-15     top 4-bits of 20-bit phys.address, extend ENV87_IP_OFFSET
        ENV87_OPERAND_OFFSET    DW      ?
        ENV87_OPERAND_SLCT      DW      ?
ENV87_AREA  ENDS

ENV87_SIZE      equ sizeof ENV87_AREA   ; size of buffer required for 8087 environment

STAT87_AREA STRUC
        STAT87_ENV      ENV87_AREA  <>
        STAT87_REGS     FPU_REGS    <>
STAT87_AREA ENDS

STAT87_SIZE     equ sizeof STAT87_AREA  ; size of buffer required for 8087 state


; Define 80387 environment:
; ========================

ENV387_AREA STRUC
        ENV387_CONTROL_WORD     DW      ?, ?
        ENV387_STATUS_WORD      DW      ?, ?
        ENV387_TAG_WORD         DW      ?, ?
        ENV387_IP_OFFSET        DD      ?
        ENV387_CS_SLCT          DW      ?
        ENV387_OPCODE           DW      ?
        ENV387_OPERAND_OFFSET   DD      ?
        ENV387_OPERAND_SLCT     DW      ?, ?
ENV387_AREA ENDS

ENV387_SIZE     equ sizeof ENV387_AREA  ; size of buffer required for 80387 environment

STAT387_AREA STRUC
        STAT387_ENV     ENV387_AREA <>
        STAT387_REGS    FPU_REGS    <>
STAT387_AREA ENDS

STAT387_SIZE    equ sizeof STAT387_AREA ; size of buffer required for 80387 state
