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
;* Description:  C runtime read/write data (i86 version).
;*
;*****************************************************************************


include mdef.inc
.286p
        name    crwdata

        assume  nothing

ifdef _PROT_MODE_
 include xinit.inc
else
 DGROUP group   _DATA
endif

_DATA   segment word public 'DATA'

        public  "C",_curbrk
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  "C",_cbyte
        public  "C",_child
        public  __no87
        public  "C",__FPE_handler
ifndef __QNX__
        public  "C",_psp
        public  __get_ovl_stack
        public  __restore_ovl_stack
        public  __close_ovl_file
        public  "C",_LpCmdLine
        public  "C",_LpPgmName
        public  "C",_osmajor
        public  "C",_osminor
        public  "C",_osmode
        public  "C",_HShift
endif

        assume  DS:DGROUP

_curbrk    dw 0                 ; top of usable memory
ifndef __QNX__
_psp       dw 0                 ; segment addr of program segment prefix
_osmajor   db 0                 ; major DOS version number
_osminor   db 0                 ; minor DOS version number
ifdef _PROT_MODE_
_osmode    db 1                 ; 0 => DOS real mode, 1 => protect-mode
_HShift    db 3                 ; Huge Shift amount (real-mode=12,prot-mode=3)
else
_osmode    db 0                 ; 0 => DOS real mode
_HShift    db 12                ; Huge Shift amount (real-mode=12,prot-mode=3)
endif
endif
_STACKLOW  dw 0                 ; lowest address in stack
_STACKTOP  dw 0                 ; highest address in stack
_cbyte     dw 0                 ; used by getch, getche
_child     dw 0                 ; non-zero => a spawned process is running
__no87     db 0                 ; non-zero => "NO87" environment var present
ifndef __QNX__
__get_ovl_stack     dw 0,0      ; get overlay stack pointer
__restore_ovl_stack dw 0,0      ; restore overlay stack pointer
__close_ovl_file    dw 0,0      ; close the overlay file handle
endif
__FPE_handler dd 0              ; FPE handler
ifndef __QNX__
_LpCmdLine dw 0,0               ; lpCmdLine (for _argc, _argv processing)
_LpPgmName dw 0,0               ; lpPgmName (for _argc, _argv processing)
endif

_DATA   ends

ifdef _PROT_MODE_

        xrefp   __HeapInit_

_TEXT   segment word public 'CODE'
        assume  cs:_TEXT
        assume  ds:DGROUP

InitHeap proc   far
        push    dx                      ; save dx
        mov     ax,ds                   ; get data segment selector
        lsl     ax,ax                   ; get size of data segment
        and     al,0FEh                 ; round down to multiple of 2
        mov     _curbrk,ax              ; set top of memory owned by process
        mov     dx,ax                   ; get size of segment
        mov     ax,_STACKTOP            ; get start address of heap
        sub     dx,ax                   ; subtract start of heap
        call    __HeapInit_             ; initialize the heap
        pop     dx                      ; restore dx
        ret                             ; return
InitHeap endp
_TEXT   ends

        xinit   InitHeap,0

endif

        end
