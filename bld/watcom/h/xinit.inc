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


include langenv.inc

PNEAR   equ     0
PFAR    equ     1

xinit   macro   rtn,priority
XI  segment word public 'DATA'
if _MODEL and _BIG_CODE
        db      PFAR
        db      priority
        dd      rtn
else
if _MODEL and _USE_32_SEGS
        db      PNEAR
        db      priority
        dd      rtn
if COMP_CFG_COFF EQ 1
        dw      0
endif
else
        db      PNEAR
        db      priority
        dw      rtn
        dw      0
endif
endif
XI      ends
        endm

xfini   macro   rtn,priority
YI      segment word public 'DATA'
if _MODEL and _BIG_CODE
        db      PFAR
        db      priority
        dd      rtn
else
if _MODEL and _USE_32_SEGS
        db      PNEAR
        db      priority
        dd      rtn
if COMP_CFG_COFF EQ 1
        dw      0
endif
else
        db      PNEAR
        db      priority
        dw      rtn
        dw      0
endif
endif
YI      ends
        endm

INIT_PRIORITY_THREAD   equ 1    ; priority for thread data init
INIT_PRIORITY_RUNTIME  equ 10   ; priority for run/time initialization
INIT_PRIORITY_IOSTREAM equ 20   ; priority for IOSTREAM
INIT_PRIORITY_LIBRARY  equ 32   ; default library-initialization priority
DEF_PRIORITY           equ 32   ; ...
INIT_PRIORITY_PROGRAM  equ 64   ; default program-initialization priority
DTOR_PRIORITY          equ 40   ; priority for module DTOR

DGROUP  group   XIB,XI,XIE,YIB,YI,YIE

XIB segment word public 'DATA'
XIB ends
XI  segment word public 'DATA'
XI  ends
XIE segment word public 'DATA'
XIE ends

YIB segment word public 'DATA'
YIB ends
YI  segment word public 'DATA'
YI  ends
YIE segment word public 'DATA'
YIE ends
