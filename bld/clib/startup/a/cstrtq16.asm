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
;* Description:  QNX 16-bit startup code.
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm cstrtq16 -bt=QNX -ms -0r
;               wasm cstrtq16 -bt=QNX -mm -0r
;               wasm cstrtq16 -bt=QNX -mc -0r
;               wasm cstrtq16 -bt=QNX -ml -0r
;               wasm cstrtq16 -bt=QNX -mh -0r
;
include mdef.inc
include exitwmsg.inc

        name    cstart

        assume  nothing

if _MODEL and _BIG_CODE
        extrn   __CMain                 : far
        extrn   __qnx_exit_             : far
else
        extrn   __CMain                 : near
        extrn   __qnx_exit_             : near
endif

        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)
        extrn   "C",_STACKTOP           : word  ; top of stack

        extrn   __QNXseg__              : byte

 DGROUP group _NULL,_AFTERNULL,CONST,STRINGS,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

if ( _MODEL and _BIG_CODE ) eq 0

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size, or else the signal function
; will get confused.

BEGTEXT  segment word public 'CODE'
        assume  cs:BEGTEXT
        int     0       ; cause a fault
___begtext label byte
        nop
        nop
        nop
        nop
        public ___begtext
        assume  cs:nothing
BEGTEXT  ends

endif

_TEXT   segment word public 'CODE'

FAR_DATA segment byte public 'FAR_DATA'
FAR_DATA ends

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

_DATA   segment word public 'DATA'
_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_BSS          segment word public 'BSS'
_BSS          ends

STACK_SIZE      equ     800h

STACK   segment para stack 'STACK'
stk     label   word
        db      (STACK_SIZE) dup(?)
end_stk label   word
STACK   ends

        assume  nothing
        public  _cstart_

        assume  cs:_TEXT

 _cstart_ proc near
        jmp     around

;
; copyright message
;
include msgrt16.inc
include msgcpyrt.inc

;
; miscellaneous code-segment messages
;
NullAssign      db      '*** NULL assignment detected',0

if ( _MODEL and _BIG_CODE ) eq 0
        dw      ___begtext      ; make sure dead code elimination
endif                           ; doesn't kill BEGTEXT segment

around:

        assume  ds:DGROUP

        cld                             ; set direction forward
        mov     ds:_STACKTOP,sp         ; set stack top
        mov     ax,bp                   ; place pid in AX
        xor     bp,bp                   ; zero value

        jmp    __CMain
_cstart_ endp

;       don't touch AX in __exit, it has the return code

__exit  proc near
        public  "C",__exit
        push    ax
        mov     dx,DGROUP
        mov     ds,dx
        cld                             ; check lower region for altered values
        lea     di,__nullarea           ; set es:di for scan
        mov     es,dx
        mov     cx,offset DGROUP:end_null
        shr     cx,1
        mov     ax,INIT_VAL
        repe    scasw
        pop     ax                      ; restore return code
        je      ok
;
; low memory has been altered
;
        mov     bx,ax                   ; get exit code
        mov     ax,offset NullAssign    ; point to msg
        mov     dx,cs                   ; . . .
        mov     sp,offset DGROUP:end_stk; set a good stack pointer
        call    __fatal_runtime_error 
ok:
        jmp     __qnx_exit_
__exit  endp


_TEXT   ends

        end     _cstart_
