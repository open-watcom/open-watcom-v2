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
;* Description:  Long jump helper routine for NLMs.
;*
;*****************************************************************************


.386p

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

_text           ends

                end
