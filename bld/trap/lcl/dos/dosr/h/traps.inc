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
;* Description:  DOS real mode trap types for WATCOM Debugger
;*
;*****************************************************************************


TRAP_SKIP               equ -1
TRAP_NONE               equ 0       ; no trap (startup time)
TRAP_TRACE_POINT        equ 1       ; T-bit trap
TRAP_BREAK_POINT        equ 2       ; break point trap
TRAP_WATCH_POINT        equ 3       ; watch point trap
TRAP_USER               equ 4       ; user requested trap
TRAP_TERMINATE          equ 5       ; task is done
TRAP_MACH_EXCEPTION     equ 6       ; machine blew up on program
TRAP_OVL_CHANGE_LOAD    equ 7       ; overlay state has changed
TRAP_OVL_CHANGE_RET     equ 8       ; overlay state has changed
