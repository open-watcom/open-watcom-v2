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


.386p
extrn   _DataSelector:WORD
extrn   _CodeEntry:FWORD
extrn   _CodeLoadAddr:DWORD

assume cs:BEGTEXT
extrn Cleanup_ : proc
BEGTEXT segment word public 'CODE' use16
        ;
        ; NOTE: this stuff is very, very important that it be right
        ; here in the order that it is in.  DEADBEEF is used by
        ; the debugger, Dr. WATCOM and the Windows Sampler to identify
        ; the app as one of our 32-bit extended ones, and the 3 pointers
        ; are used so that those tools can find the associated information.
        ; Change this only if you know ALL of the ramifications.
        ;
public _deadbeef
_deadbeef  dw      0DEADh
        dw      0BEEFh
        dw      offset _DataSelector
        dw      offset _CodeEntry
        dw      offset _CodeLoadAddr
        call    Cleanup_
        mov     ax,4c00h
        int     21h
        dw      0DEADh          ; indicate new extender
        dw      0BEEFh
BEGTEXT ends
        end
