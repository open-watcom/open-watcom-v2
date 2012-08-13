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


;
; FSHAND.ASM -- routine providing signature of our FS handler
;

ifdef __NT__
THUNK   equ         ___wcpp_4_fs_handler_
HANDLER equ         ___wcpp_4_fs_handler_rtn_
else
THUNK   equ         __wcpp_4_fs_handler_
HANDLER equ         __wcpp_4_fs_handler_rtn_
endif

        name        FSHAND
        extern      HANDLER : proc

_text   segment byte public 'code'

assume cs:_text

        public      THUNK

THUNK   proc        near

        jmp HANDLER
        db  'W'
        db  'A'
        db  'T'
        db  'C'
        db  'O'
        db  'M'

THUNK   endp

_text   ends

        end
