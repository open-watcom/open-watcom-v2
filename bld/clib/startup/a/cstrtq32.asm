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
; startup code for WATCOM C/C++32 under QNX
;
;       This must be assembled using one of the following commands:
;               wasm cstrtq32 -bt=QNX -ms -3r
;               wasm cstrtq32 -bt=QNX -ms -3s
;

        name    cstart

.386p
.387

        assume  nothing

        extrn   __CMain                     : near
        extrn   "C",__qnx_exit              : near
        extrn   "C",__fatal_runtime_error   : near
        extrn   __QNXseg__                  : near

        extrn   "C",_STACKTOP               : dword
        extrn   "C",_STACKLOW               : dword
        extrn   "C",_curbrk                 : dword
        extrn   "C",_endheap                : dword
ifdef __SLIB32
        extrn   slibentry_                  : near
endif

DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT segment use32 dword public 'CODE'
        assume  cs:BEGTEXT
        int 0       ; cause a fault
___begtext label byte
        nop
        nop
        nop
        nop
        assume  cs:nothing
BEGTEXT ends

_TEXT   segment use32 dword public 'CODE'

        assume  ds:DGROUP

        INIT_VAL        equ 0101h
        NUM_VAL         equ 16


_NULL   segment para public 'BEGDATA'
__nullarea label word
        dw      NUM_VAL dup(INIT_VAL)
        public  __nullarea

_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
end_null dw      0                       ; nullchar for string at address 0
_AFTERNULL ends

CONST   segment word public 'DATA'
NullAssign      db      0ah,'*** NULL assignment detected',0ah,0
CONST   ends

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

_DATA   segment word public 'DATA'
__slib_func   dd     2 dup(0)
        public  __slib_func
_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_BSS    segment word public 'BSS'
_BSS    ends

STACK_SIZE      equ     1000h

STACK   segment para stack 'STACK'
        db      (STACK_SIZE) dup(?)
STACK   ends

        assume  nothing
        public  _cstart_

        assume  cs:_TEXT


        assume  ds:DGROUP
        align   4
ifdef __SLIB32
_cstart_ proc near
 ;* kludge -- for build files, mov SLIB to priority 29 immediately
 ;*
        push    eax
        push    ebx
        push    edx
        mov     al,08
        mov     edx,0
        mov     ebx,29
        int     0f2h
        pop     edx
        pop     ebx
        pop     eax

        push    cs
        push    offset  _cstart2
        jmp     slibentry_
        align   4
        dd      'SLIB'
_cstart2:
else
        dd      'SLIB'
_cstart_ proc near
endif
        cld
        pop     __slib_func
        pop     __slib_func+4
        mov     _STACKTOP,esp        ; set stack top
        mov     _STACKLOW,edx
        mov     _curbrk,ebx
        mov     _endheap,ecx
        add     _endheap,ebx

        pop     eax         ; argc
        mov     edx,esp         ; argv
        lea     ebx,4[edx+eax*4] ; arge
        jmp     __CMain

;
; copyright message
;
        db      "WATCOM C/C++32 Run-Time system. "
        db      "(c) Copyright by Sybase, Inc. 1989-2000."
        db      " All rights reserved."
        dd      ___begtext      ; make sure dead code elimination
                                ; doesn't kill BEGTEXT segment

_cstart_ endp

__exit proc near
        public  "C",__exit
ifndef __STACK__
        push    eax                     ; only if not already on the stack
endif
        mov     dx,ss
        mov     ds,dx
        cld                             ; check lower region for altered values
        mov     edi,offset DGROUP:__nullarea; set es:di for scan
        mov     es,dx
        mov     ecx,offset DGROUP:end_null
        sub     ecx,edi
        shr     ecx,1
        mov     ax,INIT_VAL
        repe    scasw
        pop     eax                     ; restore return code
        je      __qnx_exit
;
; low memory has been altered
;
ifdef __STACK__
        push    eax
        push    offset DGROUP:NullAssign; point to msg
else
        mov     edx,eax                 ; get exit code
        mov     eax,offset DGROUP:NullAssign; point to msg
endif
        call    __fatal_runtime_error
        jmp     __qnx_exit
__exit endp

_TEXT   ends

        end     _cstart_
