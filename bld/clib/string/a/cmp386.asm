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
;* Description:  Intel 386 implementation strcmp().
;*
;*****************************************************************************


include mdef.inc
include struct.inc

ifdef _PROFILE
include p5prof.inc
endif

        modstart _strcmp,para

cmp4    macro   off
        mov     ebx,off[eax]            ; get dword from op1
        mov     ecx,off[edx]            ; get dword from op2
        cmp     ecx,ebx                 ; compare them
        jne     unequal                 ; quit if not equal
        ife     off-12
         add    eax,off+4               ; point to next group of dwords
         add    edx,off+4               ; ...
        endif
        not     ecx                     ; ...
        add     ebx,0FEFEFEFFh          ; determine if '\0' is one of bytes
        and     ebx,ecx                 ; ...
        and     ebx,80808080h           ; ebx will be non-zero if '\0' found
        endm

        defpe   strcmp
        xdefp   "C",strcmp
    ifdef _PROFILE
        P5Prolog
    endif
    ifdef __STACK__
        mov     eax,4[esp]              ; get p1
        mov     edx,8[esp]              ; get p2
    endif
        cmp     eax,edx                 ; pointers equal ?
        je      equalnorst              ; yes, return 0
    ifndef __STACK__
        push    ecx                     ; save register
    endif
        test    al,3                    ; p1 aligned ?
        jne     realign                 ; no, go and realign
        test    dl,3                    ; p2 aligned ?
        jne     slowcpy                 ; no, do the slow copy (impossible to align both)

fastcpy:
        push    ebx                     ; save register

        align   4
        _loop                           ; - loop
          cmp4  0                       ; - - compare first dword
          _quif ne                      ; - - quit if end of string
          cmp4  4                       ; - - compare second dword
          _quif ne                      ; - - quit if end of string
          cmp4  8                       ; - - compare third dword
          _quif ne                      ; - - quit if end of string
          cmp4  12                      ; - - compare fourth dword
        _until  ne                      ; - until end of string
equalrst:                               ; strings equal, restore registers
        pop     ebx                     ; restore register
    ifndef __STACK__
        pop     ecx                     ; ...
    endif
equalnorst:                             ; strings equal, skip register restore
        sub     eax,eax                 ; indicate strings equal
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return

unequal:                                ; dword was not equal
        _guess                          ; guess strings are equal
          cmp   bl,cl                   ; - check low bytes
          _quif ne                      ; - quit if not equal
          test  bl,bl                   ; - stop if end of string
          je    equalrst                ; - ...
          cmp   bh,ch                   ; - check next bytes
          _quif ne                      ; - quit if not equal
          test  bh,bh                   ; - stop if end of string
          je    equalrst                ; - ...
          shr   ebx,16                  ; - shift top 2 bytes to bottom
          shr   ecx,16                  ; - ...
          cmp   bl,cl                   ; - check third byte
          _quif ne                      ; - quit if not equal
          test  bl,bl                   ; - stop if end of string
          je    equalrst                ; - ...
          cmp   bh,ch                   ; - check high order byte
;;        we know at this point that bh != ch, just have to do the compare
;;        _quif ne                      ; - quit if not equal
;;        cmp   bh,0                    ; - stop if end of string
;;        je    equalrst                ; - ...
        _endguess                       ; endguess
        sbb     eax,eax                 ; eax = 0 if op1>op2, -1 if op1<op2
        or      al,1                    ; eax = 1 if op1>op2, -1 if op1<op2
        pop     ebx                     ; restore registers
    ifndef __STACK__
        pop     ecx                     ; ...
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return

        align   4
realign:
        _loop                           ; - loop
          mov   cl,[eax]                ; get byte from p1
          inc   eax                     ; point to next byte
          mov   ch,[edx]                ; get byte from p2
          inc   edx                     ; point to next byte
          cmp   cl,ch                   ; bytes equal?
          jne   unequal2                ; unequal, quit
          or    cl,cl                   ; end of string ?
          je    equal2                  ; yes, quit
          test  al,3                    ; check alignment
        _until  e                       ; until aligned
        test    dl,3                    ; p2 aligned ?
        je fastcpy                      ; yes

        align   4
slowcpy:
        _loop
          mov   ecx,[eax]               ; get aligned 4 bytes
          cmp   cl,[edx]                ; check 1st byte
          jne   unequal2                ; bytes not equal
          or    cl,cl                   ; end of string?
          je    equal2                  ; yes, quit
          cmp   ch,[edx+1]              ; check 2nd byte
          jne   unequal2                ; bytes not equal
          or    ch,ch                   ; end of string?
          je    equal2                  ; yes, quit
          shr   ecx,16                  ; move next pair of bytes to be tested
          cmp   cl,[edx+2]              ; check 3rd byte
          jne   unequal2                ; bytes not equal
          or    cl,cl                   ; end of string?
          je    equal2                  ; yes, quit
          cmp   ch,[edx+3]              ; check 4th byte
          jne   unequal2                ; bytes not equal
          or    ch,ch                   ; end of string?
          je    equal2                  ; yes, quit
          mov   ecx,[eax+4]             ; get next aligned 4 bytes
          cmp   cl,[edx+4]              ; check 5th byte
          jne   unequal2                ; bytes not equal
          or    cl,cl                   ; end of string?
          je    equal2                  ; yes, quit
          cmp   ch,[edx+5]              ; check 6th byte
          jne   unequal2                ; bytes not equal
          or    ch,ch                   ; end of string?
          je    equal2                  ; yes, quit
          shr   ecx,16                  ; move next pair of bytes to be tested
          cmp   cl,[edx+6]              ; check 7th byte
          jne   unequal2                ; bytes not equal
          or    cl,cl                   ; end of string?
          je    equal2                  ; yes, quit
          cmp   ch,[edx+7]              ; check 8th byte
          jne   unequal2                ; bytes not equal
          add   eax,8                   ; next 8 bytes
          add   edx,8                   ; next 8 bytes
          or    ch,ch                   ; end of string?
        _until  e                       ; until equal
equal2:
        xor     eax,eax                 ; return 0
    ifndef __STACK__
        pop     ecx                     ; restore registers
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return

unequal2:
        sbb     eax,eax                 ; eax = 0 if op1>op2, -1 if op1<op2
        or      al,1                    ; eax = 1 if op1>op2, -1 if op1<op2
    ifndef __STACK__
        pop     ecx                     ; restore registers
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                             ; return
        endproc strcmp

        endmod
        end
