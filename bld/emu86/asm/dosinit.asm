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
;* Description:  routines for 16-bit DOS FPU emulator control
;*
;*****************************************************************************

; !!!!! must be compiled with -fpi option !!!!!

;
;  interface to 16-bit DOS emulator for FPU functions required by clib
;
;    function                    used in  
;  1  init emulator       clib/startup/c/chk8087.c
;  2  init old stack      clib/fpu/a/old87.asm
;  3  read control word   clib/fpu/c/cntrl87.c
;  4  write control word  clib/fpu/c/cntrl87.c

include mdef.inc

DGROUP  group   _DATA
        assume  ds:DGROUP

_DATA   segment word public 'DATA'
        extrn   __8087cw        : word
_DATA   ends

_TEXT segment word public 'CODE'

        xdefp   ___dos87emucall

___dos87emucall proc near
        cmp     ax,1
        jnz     l1
        finit                         ; initialize the 8087 EMU
        fldcw   word ptr __8087cw     ; load default control word
        ret
        
l1:     cmp     ax,2
        jnz     l2
        fldz                          ; put 8087 EMU into 4 empty / 4 full state
        fldz                          ; ...
        fldz                          ; ...
        fldz                          ; ...
        ret

l2:     cmp     ax,3
        jnz     l3
        xchg    bx,bp                 ; read 8087 EMU control word
        fstcw   word ptr [bp]         ; ...
        fwait                         ; ...
        xchg    bx,bp                 ; ...
        ret
        
l3:     cmp     ax,4
        jnz     return
        xchg    bx,bp                 ; set 8087 EMU control word
        fldcw   word ptr [bp]         ; ...
        xchg    bx,bp                 ; ...
return: ret
___dos87emucall endp

_TEXT   ends

        end
