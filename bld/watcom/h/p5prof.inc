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

extrn __PON : near
extrn __POFF : near

        __pln = 0

_pushlabel      macro base,ext
                push    offset base&&ext
                endm

P5Prolog        macro lbl
@@:
                TI segment use32 byte public 'DATA'
                __pln = __pln + 1
                _label  _p,%__pln
                        dd 0            ;flag
                        dd 0            ;semaphore
                        dd 0            ;stack
                        dd 0            ;esp
                        dd 0            ;dynamic
                        dd 0            ;lo_count
                        dd 0            ;hi_count
                        dd 0            ;lo_cycle
                        dd 0            ;hi_cycle
                        dd 0            ;lo_start_time
                        dd 0            ;hi_start_time
                        dd offset @B    ;caller
                        dd 0            ;call_ins
                        dd 0            ;callee
                TI ends
                _pushlabel _p,%__pln
                call    __PON
                endm

P5Epilog        macro
                _pushlabel _p,%__pln
                call    __POFF
                endm

if COMP_CFG_COFF == 1
else
DGROUP  group   TIB,TI,TIE
TIB     segment use32 byte public 'DATA'
TIB     ends
TI      segment use32 byte public 'DATA'
TI      ends
TIE     segment use32 byte public 'DATA'
TIE     ends
endif
