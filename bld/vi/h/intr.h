/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


;***************************************************************************
;*** INTR.H  - Include file for interrupt routines                       ***
;***                                                                     ***
;*** By: Craig Eisler                                                    ***
;***     August 2 1990                                                   ***
;***                                                                     ***
;***************************************************************************

;*
;*** offsets of stuff into interrupt data
;*
ino     equ     0
rax     equ     2h
rbx     equ     4h
rcx     equ     6h
rdx     equ     8h
rbp     equ     0ah
rsi     equ     0ch
rdi     equ     0eh
rds     equ     10h
res     equ     12h
flags   equ     14h

FLAG_CARRY           equ        1h
FLAG_PARITY          equ        4h
FLAG_AUX_CARRY       equ        10h
FLAG_ZERO            equ        40h
FLAG_SIGN            equ        80h
FLAG_TRACE           equ        100h
FLAG_INTERRUPT       equ        200h
FLAG_DIRECTION       equ        400h
FLAG_OVERFLOW        equ        800h
