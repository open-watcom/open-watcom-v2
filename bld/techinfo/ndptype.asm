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


; NDPType - Check for math coprocessor and determine type
;
; int NDPType()
;       returns:
;               0 - no coprocessor present
;               1 - 8087 present
;               2 - 80287 present
;               3 - 80387 present
;
;  program taken from August 1987 issue of PC Tech Journal, page 43
;
        .8087
_TEXT   segment byte public 'code'
        assume  CS:_TEXT

        public  NDPType_
NDPType_ proc    near
        push    bp                      ; save bp
        sub     ax,ax                   ; set initial control word to 0
        push    ax                      ; push it on stack
        mov     bp,sp                   ; point to control word
;
        fninit                          ; initialize math coprocessor
        fnstcw  0[bp]                   ; store control word in memory
        mov     al,0                    ; assume no coprocessor present
        mov     ah,1[bp]                ; upper byte is 03h if
        cmp     ah,03h                  ;   coprocessor is present
        jne     exit                    ; exit if no coprocessor present
        mov     al,1                    ; assume it is an 8087
        and     word ptr 0[bp],NOT 0080h; turn interrupts on (IEM=0)
        fldcw   0[bp]                   ; load control word
        fdisi                           ; disable interrupts (IEM=1)
        fstcw   0[bp]                   ; store control word
        test    word ptr 0[bp], 0080h   ; if IEM=1, then 8087
        jnz     exit                    ;
        finit                           ; use default infinity mode
        fld1                            ; generate infinity by
        fldz                            ;   dividing 1 by 0
        fdiv                            ; ...
        fld     st                      ; form negative infinity
        fchs                            ; ...
        fcompp                          ; compare +/- infinity
        fstsw   0[bp]                   ; equal for 87/287
        fwait                           ; wait fstsw to complete
        mov     ax,0[bp]                ; get NDP control word
        mov     al,2                    ; assume 80287
        sahf                            ; store condition bits in flags
        jz      exit                    ; it's 287 if infinities equal
        mov     al,3                    ; indicate 80387
exit:   cbw                             ; zero ah
        pop     bp                      ; throw away control word
        pop     bp                      ; restore bp
        ret                             ; return

NDPType_ endp
_TEXT   ends
        end
