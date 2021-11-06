;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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

;
; Calling convention depends on Operating system
;
; NT        __cdecl
; OS/2      __system
; others    __watcall

        name        FSHAND

ifdef __NT__
        extern      C __wcpp_4_fs_handler_rtn_ : proc
        public      C __wcpp_4_fs_handler_
else
        extern      __wcpp_4_fs_handler_rtn_ : proc
        public      __wcpp_4_fs_handler_
endif

_TEXT   segment byte public 'code'

assume cs:_text

__wcpp_4_fs_handler_    proc

        jmp __wcpp_4_fs_handler_rtn_
        db  'W'
        db  'A'
        db  'T'
        db  'C'
        db  'O'
        db  'M'

__wcpp_4_fs_handler_    endp

_TEXT   ends

        end
