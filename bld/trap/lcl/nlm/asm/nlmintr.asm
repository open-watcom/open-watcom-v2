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


COMMENT ~

        Copyright (C) 1989 by WATCOM Systems Inc. All rights
        reserved. No part of this software may be reproduced in any
        form or by any means - graphic, electronic or mechanical,
        including photocopying, recording, taping or information
        storage and retrieval systems - except with the written
        permission of WATCOM Systems Inc.


        Date            By              Reason
        ====            ==              ======
        90-01-25        John Dahms      created

~

.386p
.387

                name            nlmintr

extrn           ReturnESP       : dword

assume  cs:_text

_text segment byte public 'CODE'

public          Return
Return          proc    near
                mov     esp,ReturnESP
                ret
Return          endp

;
;       void DoALongJumpTo( eax, ebx, ecx, edx, esi, edi, ebp, esp, efl, eip )
public          DoALongJumpTo
DoALongJumpTo   proc    near
                pushfd                          ; turn off NT bit in flags
                pop     eax                     ; ...
                and     eax,0FFFFBFFFH          ; ...
                push    eax                     ; ...
                popfd                           ; ...
                pop     eax                     ; fling out return address
                mov     eax,28[esp]             ; get new esp value
                pop     -16[eax]                ; get eax on new stack
                pop     ebx                     ; ...
                pop     ecx                     ; ...
                pop     edx                     ; ...
                pop     esi                     ; ...
                pop     edi                     ; ...
                pop     ebp                     ; ...
                pop     eax                     ; this is esp value again
                pop     -4[eax]                 ; ...
                mov     -8[eax],cs
                pop     -12[eax]                ; put return address on new esp
                lea     esp,-16[eax]            ; ...
                pop     eax
                iretd
DoALongJumpTo   endp

public          Read387
Read387         proc    near
                mov     eax,4[esp]
                fsave   [eax]
                ret
Read387         endp

public          Write387
Write387        proc    near
                mov     eax,4[esp]
                frstor  [eax]
                ret
Write387        endp

public          GetCR0
GetCR0          proc    near
                mov     eax,cr0
                ret
GetCR0          endp

public          GetDS
GetDS           proc    near
                mov     ax,ds
                ret
GetDS           endp

public          GetCS
GetCS           proc    near
                mov     ax,cs
                ret
GetCS           endp

        public  NPXType
NPXType proc     near
                push    ebx                     ; save ebx
                sub     ebx,ebx                 ; set initial control word to 0
                push    ebx                     ; push it on stack
                mov     eax,cr0                 ; get control word
                push    eax                     ; save it
                and     eax,NOT 4               ; turn off EM bit
                mov     cr0,eax                 ; ...
;
                fninit                          ; initialize math coprocessor
                fnstcw  4[esp]                  ; store control word in memory
                mov     al,0                    ; assume no coprocessor present
                mov     ah,5[esp]               ; upper byte is 03h if
                cmp     ah,03h                  ;   coprocessor is present
                jne     exit                    ; exit if no coprocessor present
                finit                           ; use default infinity mode
                fld1                            ; generate infinity by
                fldz                            ;   dividing 1 by 0
                fdiv                            ; ...
                fld     st                      ; form negative infinity
                fchs                            ; ...
                fcompp                          ; compare +/- infinity
                fstsw   4[esp]                  ; equal for 87/287
                fwait                           ; wait fstsw to complete
                mov     ax,4[esp]               ; get NDP control word
                mov     al,2                    ; assume 80287
                sahf                            ; store condition bits in flags
                jz      exit                    ; it's 287 if infinities equal
                mov     al,3                    ; indicate 80387
exit:           pop     ebx                     ; restore control word
                mov     cr0,ebx                 ; ...
                pop     ebx                     ; clear the stack
                pop     ebx                     ; restore ebx
                ret                             ; return
NPXType         endp

_text           ends

                end
