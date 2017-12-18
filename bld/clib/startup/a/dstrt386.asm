;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Win32 and OS/2 DLL startup code.
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm dstrt386 -ms -3r
;               wasm dstrt386 -ms -3s
;
        name    dstrt386
.386p
        assume  nothing

        extrn   __LibMain       : near
        extrn   ___begtext      : near

_TEXT   segment use32 word public 'CODE'

        public  __DLLstart_
        public  __DLLstartw_
ifdef __NT__
        public  _DllMainCRTStartup@12:
        public  _wDllMainCRTStartup@12:
endif

        assume  cs:_TEXT

__DLLstart_ proc near
__DLLstartw_ proc near
ifdef __NT__
_DllMainCRTStartup@12 proc near
_wDllMainCRTStartup@12 proc near
endif
        jmp     __LibMain
ifdef __NT__
_wDllMainCRTStartup@12 endp
_DllMainCRTStartup@12 endp
endif
__DLLstartw_ endp
__DLLstart_ endp

        dd      ___begtext      ; reference module with segment definitions

;
; copyright message
;
include msgcpyrt.inc

_TEXT   ends

        end     __DLLstart_
