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
;* Description:  Stack thunking routines for OS/2. These are needed for
;*               mixed 32-bit and 16-bit code. 
;*
;*****************************************************************************



                name    thunk

.386p

                assume  nothing

extrn __STK                     : near
extrn "C",__init_threadstksize  : near

dgroup  group   stack

stack   segment para stack 'STACK'
        db      8192 dup(?)
stack   ends

_TEXT           segment use32 word public 'CODE'

                assume          cs:_TEXT


;
;               generated code sequence
;               - push  esp
;               - push  parm_size
;               - push  local_size
;               - call  __TNK
;               - .... normal prolog
;               - .... .... insert favorite routine here
;               - .... normal epilog
;               - pop   esp
;               - ret
;
                public  __TNK
                public  __TNC                   ; thunk with stack check

__TNC           proc    near export
                push    eax                     ; save register
                mov     eax,8[esp]              ; get local size
                add     eax,12[esp]             ; add parm size
                add     eax,8*1024              ; add 8k
                call    __STK                   ; check the stack
                pop     eax                     ; ...
__TNK           proc    near export
                push    eax                     ; ...
                push    ebx                     ; ...
                push    ecx                     ; ...
                push    esi                     ; ...
                push    edi                     ; ...
                lea     edi,[9*4+esp]           ; point to fn return addr
                mov     eax,-8[edi]             ; get size of parms
                mov     ebx,-12[edi]            ; get local size
                add     ebx,2000H               ; plus an extra 8k
                lea     esi,[edi+eax]           ; point at the first parm
                mov     ecx,esi                 ; ...
                sub     cx,bx                   ; subtract locals required
                jae     stackok                 ; if stack will cross 64K bdry
                xor     di,di                   ; round edi down to crossed bdry
                mov     esi,esp                 ; esi -> stuff we want to copy
                lea     ecx,[10*4+eax]          ; - parms + save area
                sub     edi,ecx                 ; - new stack area - below bdry
                sub     esp,4                   ; - bump down stack pointer
                and     sp,0f000H               ; - round down to 4K bdry

loop64k:        cmp     esp,edi                 ; - while esp > edi
                jbe     end64k                  ; - - ...
                mov     [esp],eax               ; - - touch the page
                sub     esp,1000H               ; - - down another 4K
                jmp     short loop64k           ; - endwhile
end64k:                                         ; - ...
                mov     esp,edi                 ; - use the new location as stk
                shr     ecx,2                   ; - adjust for dword move
                rep     movsd                   ; - ...
stackok:                                        ; endif
                pop     edi                     ; restore regs
                pop     esi                     ; ...
                pop     ecx                     ; ...
                pop     ebx                     ; ...
                pop     eax                     ; ...
                ret     8
__TNK           endp
__TNC           endp

_TEXT           ends

                end
