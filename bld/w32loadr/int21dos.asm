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
;       __Int21 interface to OS int 21h translator
;
.386p

_TEXT segment word public 'CODE'
assume  cs:_TEXT

        public  __Int21
        public  __fInt21

__fInt21 proc  far
        call    __Int21
        ret
__fInt21 endp

__Int21 proc   near
        cmp     AH,48h                  ; allocate memory?
        je      short AllocMem          ; branch if yes
        cmp     AH,4Eh                  ; DOS Find First?
        je      short FindFirst         ; branch if yes
        cmp     AH,4Fh                  ; DOS Find Next?
        je      short FindNext          ; branch if yes
        int     21h                     ; issue int 21
        ret                             ; return
AllocMem:push   ECX                     ; save registers
        push    ESI                     ; ...
        push    EDI                     ; ...
        mov     CX,BX                   ; get low word of length
        shr     EBX,16                  ; get high word of length
        mov     AX,0501h                ; DPMI allocate
        int     31h                     ; ...
        jc      short AllocFail         ; check for fail
        mov     AX,BX                   ; linear address returned in BX:CX
        shl     EAX,16                  ; ...
        mov     AX,CX                   ; ...
        mov     [EAX],DI                ; store handle in block
        mov     2[EAX],SI               ; ...
        clc                             ; indicate allocation succeeded
AllocFail:
        pop     EDI                     ; restore registers
        pop     ESI                     ; ...
        pop     ECX                     ; ...
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
__Int21 endp

_TEXT   ends
        end
