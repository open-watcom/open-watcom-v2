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


.386p
                NAME    try

                extrn   DOS32WRITE:near, DOS32DEBUG:near
DGROUP          GROUP _STACK

_STACK          SEGMENT STACK DWORD USE32 'STACK'
buffer          dd  DGROUP:DOS32DEBUG
selectors       dw  3 dup (?)
dummy           dd  1
                db  0f00h dup( ? )
_STACK          ENDS


_TEXT           SEGMENT PUBLIC DWORD USE32 'CODE'
                ASSUME CS:_TEXT, DS:DGROUP
startup         label near
                mov selectors+0, cs     ; stash CS value
                mov selectors+2, ds     ; stash DS value
                mov selectors+4, ss     ; stash SS value
                mov esi,16[esp]         ; get command line pointer
lup:
                inc esi                 ; point at next character
                cmp byte ptr -1[esi], 0 ; found end?
                jne lup                 ; try next one

                xor eax,eax             ; get handle
lup2:
                mov al,[esi]            ; ...
                inc esi
                cmp al, ' '
                je  lup2
                sub eax,'0'             ; ...

                push    offset dummy    ; loc to store num written
                push    10              ; num bytes to write
                push    offset buffer   ; start of buffer to write
                push    eax             ; handle to write to
                call    DOS32WRITE
                add     esp,4*4
                ret
_TEXT           ENDS

                END startup
