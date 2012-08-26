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


PI_EXP  equ     4000h
PI_HW   equ     0C90FDAA2h
PI_LW   equ     02168C235h

PIBY2_EXP equ   (PI_EXP-1)
PIBY4_EXP equ   (PI_EXP-2)

;       _PIby6  dt      0.523598775598298873078

PIBY6_EXP equ   3FFEh
PIBY6_HW  equ   0860A91C1h
PIBY6_LW  equ   06B9B2C23h

ONE_EXP equ     3FFFh
ONE_HW  equ     80000000h
ONE_LW  equ     0

;       sqrt(3.0)

SQRT3_EXP equ   3FFFh
SQRT3_HW equ    0DDB3D742h
SQRT3_LW equ    0C265539Eh

TAN15_EXP equ   3FFDh
TAN15_HW  equ   08930A2F4h
TAN15_LW  equ   0F66AB09Bh

; 1/log2(e)  dt      1.4426950408889634075

LN2R_EXP equ    3FFFh
LN2R_HW  equ    0B8AA3B29h
LN2R_LW  equ    05C17F0BDh

