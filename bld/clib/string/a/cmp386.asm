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


include mdef.inc
include struct.inc

ifdef _PROFILE
include p5prof.inc
endif

        modstart _strcmp,para

cmp4    macro   off
        mov     eax,off[ebx]            ; get dword from op1
        mov     ecx,off[edx]            ; get dword from op2
        cmp     ecx,eax                 ; compare them
        jne     short unequal           ; quit if not equal
        ife     off-12
         add    ebx,off+4               ; point to next group of dwords
         add    edx,off+4               ; ...
        endif
        not     ecx                     ; ...
        add     eax,0FEFEFEFFh          ; determine if '\0' is one of bytes
        and     eax,ecx                 ; ...
        and     eax,80808080h           ; eax will be non-zero if '\0' found
        endm

        defpe   strcmp
        xdefp   "C",strcmp
    ifdef _PROFILE
        P5Prolog
    endif
    ifdef __STACK__
        push    edx                     ; save edx
        mov     eax,8[esp]              ; get p1
        mov     edx,12[esp]             ; get p2
    endif
        push    ebx                     ; save registers
        push    ecx                     ; ...
        mov     ebx,eax                 ; get p1
        cmp     eax,edx                 ; if pointers not equal
        _if     ne                      ; then
          _loop                         ; - loop
            cmp4  0                     ; - - compare first dword
            _quif ne                    ; - - quit if end of string
            cmp4  4                     ; - - compare second dword
            _quif ne                    ; - - quit if end of string
            cmp4  8                     ; - - compare third dword
            _quif ne                    ; - - quit if end of string
            cmp4  12                    ; - - compare fourth dword
          _until  ne                    ; - until end of string
        _endif                          ; endif
equal:  sub     eax,eax                 ; indicate strings equal
        pop     ecx                     ; restore registers
        pop     ebx                     ; ...
    ifdef __STACK__
        pop     edx                     ; ...
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return

unequal:                                ; dword was not equal
        _guess                          ; guess strings are equal
          cmp   al,cl                   ; - check low bytes
          _quif ne                      ; - quit if not equal
          cmp   al,0                    ; - stop if end of string
          je    equal                   ; - ...
          cmp   ah,ch                   ; - check next bytes
          _quif ne                      ; - quit if not equal
          cmp   ah,0                    ; - stop if end of string
          je    equal                   ; - ...
          shr   eax,16                  ; - shift top 2 bytes to bottom
          shr   ecx,16                  ; - ...
          cmp   al,cl                   ; - check third byte
          _quif ne                      ; - quit if not equal
          cmp   al,0                    ; - stop if end of string
          je    equal                   ; - ...
          cmp   ah,ch                   ; - check high order byte
;;        we know at this point that ah != ch, just have to do the compare
;;        _quif ne                      ; - quit if not equal
;;        cmp   ah,0                    ; - stop if end of string
;;        je    equal                   ; - ...
        _endguess                       ; endguess
        sbb     eax,eax                 ; eax = 0 if op1>op2, -1 if op1<op2
        or      al,1                    ; eax = 1 if op1>op2, -1 if op1<op2
        pop     ecx                     ; restore registers
        pop     ebx                     ; ...
    ifdef __STACK__
        pop     edx                     ; ...
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return
        endproc strcmp

        endmod
        end
