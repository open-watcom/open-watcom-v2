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
;* Description:  Windows 16-bit (Win16, Windows 3.x) DLL startup code.
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm libentry.asm -bt=WINDOWS -ms -0r
;               wasm libentry.asm -bt=WINDOWS -mm -0r
;               wasm libentry.asm -bt=WINDOWS -mc -0r
;               wasm libentry.asm -bt=WINDOWS -ml -0r
;

include xinit.inc

DGROUP group _NULL,_DATA,CONST,STRINGS,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS

public  pLocalHeap
public  pAtomTable
public  pStackTop
public  pStackMin
public  pStackBot

pLocalHeap      equ     0006H
pAtomTable      equ     0008H
pStackTop       equ     000AH
pStackMin       equ     000CH
pStackBot       equ     000EH

; cloned from windows.inc
GMEM_SHARE      equ     2000h

        .dosseg

FAR_DATA segment byte public 'FAR_DATA'
FAR_DATA ends

_BSS    segment word public 'BSS'
_BSS    ends

DATA    segment word public 'DATA'
DATA    ends

CONST   segment word public 'DATA'
CONST   ends

STRINGS segment word public 'DATA'
STRINGS ends

XIB     segment word public 'DATA'
XIB     ends
XI      segment word public 'DATA'
XI      ends
XIE     segment word public 'DATA'
XIE     ends

YIB     segment word public 'DATA'
YIB     ends
YI      segment word public 'DATA'
YI      ends
YIE     segment word public 'DATA'
YIE     ends

_NULL   segment para public 'BEGDATA'
__nullarea label word
           dw   0,0
           dw   5
           dw   0               ; pLocalHeap
           dw   0               ; pAtomTable
_STACKLOW  dw   0               ; pStackTop: lowest address in stack
_STACKTOP  dw   0               ; pStackMin:
           dw   0               ; pStackBot: highest address in stack
        public  __nullarea
_NULL   ends

_DATA segment word public 'DATA'
;*
;*** externals we need
;*
assume es:nothing
assume ss:nothing
assume ds:dgroup
assume cs:_TEXT

        extrn   __AHSHIFT                   : word
        extrn   "C",__win_alloc_flags       : dword
        extrn   "C",__win_realloc_flags     : dword

        public  "C",_curbrk
        public  "C",_psp
        public  "C",_osmajor
        public  "C",_osminor
        public  "C",_osmode
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  "C",_cbyte
        public  __child
        public  __no87
        public  "C",_HShift
        public  __get_ovl_stack
        public  __restore_ovl_stack
        public  "C",__FPE_handler
        public  "C",_LpCmdLine

__aaltstkovr dw -1              ; alternate stack overflow routine address
_curbrk    dw 0                 ; top of usable memory
_psp       dw 0                 ; segment addr of program segment prefix
_osmajor   db 0                 ; major DOS version number
_osminor   db 0                 ; minor DOS version number
_osmode    db 0                 ; 0 => DOS real mode
_HShift    db 0                 ; Huge Shift value
_cbyte     dw 0                 ; used by getch, getche
__child    dw 0                 ; non-zero => a spawned process is running
__no87     db 0                 ; always try to use the 8087
__get_ovl_stack dw 0,0          ; get overlay stack pointer
__restore_ovl_stack dw 0,0      ; restore overlay stack pointer
__FPE_handler dd 0              ; FPE handler
_LpCmdLine dw 0,0               ; lpCmdLine (for _argc, _argv processing)
           db 0                 ; slack byte

_DATA ends

;*
;*** the windows extender code lies here
;*
_TEXT segment word public 'CODE'

        extrn   LIBMAIN     : far       ; startup code
        extrn   LOCALINIT   : far       ; Windows heap init routine
        extrn   __FInitRtns : far       ; initializer
        extrn   __FFiniRtns : far       ; finalizer

public          _large_code_
_large_code_    equ 0

;****************************************************************************
;***                                                                      ***
;*** LibEntry - 16-bit library entry point                                ***
;***                                                                      ***
;****************************************************************************
LibEntry proc far
        public  LibEntry
__DLLstart_:
        public  __DLLstart_

        mov     ax,ds            ; prologue
        nop
        inc     bp
        push    bp
        mov     bp,sp
        push    ds
        mov     ds,ax

        push    di               ; handle of the module instance
        push    ds               ; library data segment
        push    cx               ; heap size
        push    es               ; command line segment
        push    si               ; command line offset
        jcxz    callc            ; skip heap init
        xor     ax,ax
        push    ds
        push    ax
        push    cx
        call    LOCALINIT
        or      ax,ax            ; did it do it ok ?
        jz      _error           ; quit if it failed

callc:
        or      word ptr __win_alloc_flags, GMEM_SHARE
        or      word ptr __win_realloc_flags, GMEM_SHARE
        mov     ax,offset __AHSHIFT ; get huge shift value
        mov     _HShift,al       ; ...
        cmp     al,12            ; real mode?
        je      notprot          ; yes, so leave osmode alone
        mov     al,1
        mov     _osmode,al      ; protected mode!
notprot:
        mov     ax,offset __null_FPE_rtn; initialize floating-point exception
        mov     word ptr __FPE_handler,ax       ; ... handler address
        mov     word ptr __FPE_handler+2,cs     ; ...
        mov     ax,0ffh         ; run all initializers
        call    __FInitRtns     ; call initializer routines
        call    LIBMAIN         ; invoke the 'C' routine (result in AX)
        jmp     short exit      ; LibMain is responsible for stack clean up

_error:

        pop     si               ; clean up stack on a LocalInit error
        pop     es
        pop     cx
        pop     ds
        pop     di
        jmp     short exit

__exit:
        public  "C",__exit

        push    ax              ; save return code
        xor     ax,ax           ; run finalizers
        mov     dx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FFiniRtns     ; call finalizer routines
        pop     ax              ; restore return code
        mov     ah,04cH         ; DOS call to exit with return code
        int     021h            ; back to DOS

exit:
        lea     sp,-2H[bp]
        pop     ds
        pop     bp
        dec     bp
        ret
LibEntry    endp

__null_FPE_rtn proc far
        ret                             ; return
__null_FPE_rtn endp

public  __GETDS
__GETDS proc    near
        ret                             ; return
__GETDS endp

_TEXT   ends
        end     LibEntry
