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
;* Description:  Linux i386 32-bit startup code.
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm cstrtl32 -bt=LINUX -ms -3r
;               wasm cstrtl32 -bt=LINUX -ms -3s
;
        name    cstart
.386p
        assume  nothing

        extrn   __LinuxMain     : near
        extrn   ___begtext      : near
        extrn   "C",_STACKTOP   : dword

_TEXT   segment use32 word public 'CODE'

        public  _cstart_
        public  _start          ; Needed for debugging with GDB

        assume  cs:_TEXT

; This is the main executable entry point. The SVR4/i386 ABI (pages 3-31, 3-32)
; says that when the entry point runs, most registers' values are unspecified,
; except for:
;
;   EDX - Contains a function pointer to be registered with `atexit'.
;         This is how the dynamic linker arranges to have DT_FINI
;         functions called for shared libraries that have been loader
;         before this code runs.
;
;   ESP - The stack contains the arguments and environment:
;
;           0(%esp)			argc
;           4(%esp)			argv[0]
;           ...
;           (4*argc)(%esp)		NULL
;           (4*(argc+1))(%esp)	envp[0]
;           ...
;           NULL

_cstart_:
_start proc near

	    xor     ebp,ebp         ; Clear frame pointer
        mov     _STACKTOP,esp   ; set stack top
        pop     ecx             ; Pop the argument count.
        mov     esi,esp         ; argv starts just at the current stack top
        lea     eax,[esi + ecx*4 + 4]
        push    eax             ; Push arge (pointer to environment)
        push    esi             ; Push argv
        push    ecx             ; Push argc
        call    __LinuxMain
        dd      ___begtext      ; reference module with segment definitions
;
; copyright message
;
include msgrt32.inc
include msgcpyrt.inc

_start endp

_TEXT   ends

        end     _start
