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
;* Description:  Linux MIPS 32-bit startup code.
;*
;*****************************************************************************


;       This must be assembled using the following command:
;               wasmps cstrtlmp
;

.globl          _cstart_

.text

; This is the main executable entry point. The SVR4 MIPS ABI supplemet (pages
; 3-30, 3-31) specifies that at process startup, most registers' values are
; unspecified, except for:
;
;   $v0 - Contains a function pointer to be registered with `atexit'.
;         This is how the dynamic linker arranges to have DT_FINI
;         functions called for shared libraries that have been loader
;         before this code runs. If $v0 is zero, no action is required.
;
;   $ra - Contains zero to indicate the last stack frame.
;
;   $sp - The stack contains the arguments and environment:
;
;           0($sp)                  argc
;           4($sp)                  argv[0]
;           ...
;           (4 * argc)($sp)         NULL
;           (4 * (argc + 1))($sp)   envp[0]
;           ...
;           NULL
;           ...                     auxiliary vector


_cstart_:
;        la      $v0,_STACKTOP   ; load _STACKTOP address
;        sw      $sp,$(v0)       ; store stack top
        lw      $a0,($sp)       ; load argc into a0
        li      $v0,4           ; load 4 into v0
        addiu   $a1,$sp,4       ; calculate argv into a1
        sll     $v0,$a0,2       ; calculate size of argv
        addu    $a2,$a1,$v0     ; point a2 at terminating NULL of argv
        addiu   $a2,$a2,4       ; make a2 point at start of arge
        j       _LinuxMain      ; off we go, never to return
;
; signature message
;
        .asciiz "Open Watcom C/C++ MIPS Run-Time system."

.new_section .drectve, "iRr0"
             .asciiz "-entry:_cstart_"
