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
;       __Int21 interface to OS int 21h translator for Pharlap
;
.386p

        extrn   __curbrk        : dword

_TEXT segment word public 'CODE'
assume  cs:_TEXT

        public  __Int21_
        public  __fInt21_

__fInt21_ proc  far
        call    __Int21_
        ret
__fInt21_ endp

__Int21_ proc   near
        cmp     AH,48h                  ; allocate memory?
        je      short AllocMem          ; branch if yes
        cmp     AH,4Eh                  ; DOS Find First?
        je      short FindFirst         ; branch if yes
        cmp     AH,4Fh                  ; DOS Find Next?
        je      short FindNext          ; branch if yes
        int     21h                     ; issue int 21
        ret                             ; return
AllocMem:push   ECX                     ; save registers
        add     EBX,__curbrk            ; add old break value
        add     EBX,0FFFh               ; round up to 4K page boundary
        shr     EBX,12                  ; calc. # of 4K pages
        mov     AH,4Ah                  ; resize memory block
        int     21h                     ; ...
        jc      short AllocFail         ; check for fail
        mov     EAX,__curbrk            ; return old break value
        shl     EBX,12                  ; calc new break value
        mov     __curbrk,EBX            ; save new break value
        clc                             ; indicate allocation succeeded
AllocFail:
        pop     ECX                     ; restore registers
        ret                             ; return
FindFirst:
        push    EDX                     ; save filename address
        mov     EDX,EBX                 ; get DTA address
        mov     AH,1Ah                  ; set DTA address
        int     21h                     ; ...
        pop     EDX                     ; restore filename address
        mov     AH,4Eh                  ; find first
        int     21h                     ; ...
        ret                             ; return
FindNext:
        cmp     AL,0                    ; if not FIND NEXT
        jne     short findret           ; then return
        push    EDX                     ; save EDX
        mov     AH,1Ah                  ; set DTA address
        int     21h                     ; ...
        mov     AH,4Fh                  ; find next
        int     21h                     ; ...
        pop     EDX                     ; restore EDX
findret:ret                             ; return
__Int21_ endp

_TEXT   ends
        end
