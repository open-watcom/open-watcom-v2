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
        name    bexpand

include struct.inc

_TEXT segment dword public 'CODE'
assume  cs:_TEXT

        public  BPE_Expand_
;
;  input:
;       EAX - char *dst;
;       EDX - char *src;
;       EBX - char *srcend;
;
left    equ     [ESP]
right   equ     256[ESP]
cstack  equ     512[ESP]

BPE_Expand_ proc near
        push    ECX                     ; save registers
        push    ESI                     ; ...
        push    EDI                     ; ...
        push    EBP                     ; ...
        sub     ESP,256+256+32          ; allocate auto arrays
        mov     EBP,EBX                 ; save addr of end of src
        mov     ESI,EDX                 ; get source pointer
        mov     EDI,EAX                 ; get destination address
;;      _loop                           ; loop
        _guess                          ; guess
nxtblk:   sub   EBX,EBX                 ; - zero i
          _loop                         ; - loop (set left[i] = i;)
            mov   left[EBX],BL          ; - - ...
            inc   EBX                   ; - - ...
            mov   left[EBX],BL          ; - - ...
            inc   EBX                   ; - - ...
            mov   left[EBX],BL          ; - - ...
            inc   EBX                   ; - - ...
            mov   left[EBX],BL          ; - - ...
            inc   EBX                   ; - - ...
            cmp   BL,0                  ; - - quit if i == 256
          _until  e                     ; - until done
          sub   EBX,EBX                 ; - zero c
          _loop                         ; - loop (read pair table)
            sub   EAX,EAX               ; - - count = 0;
            mov   AL,[ESI]              ; - - count = *src++;
            inc   ESI                   ; - - ...
            test  AL,80h                ; - - if count > 127
            _if   ne                    ; - - then
              sub   AL,127              ; - - - subtract 127
              add   EBX,EAX             ; - - - c += count - 127
              cmp   BL,0                ; - - - quit if c == 256
              je    short getsize       ; - - - ...
              sub   EAX,EAX             ; - - - count = 0
            _endif                      ; - - endif
            _loop                       ; - - loop (read pairs, skip if lit)
              mov   DL,[ESI]            ; - - - DL = *src++;
              inc   ESI                 ; - - - ...
              mov   left[EBX],DL        ; - - - left[c] = DL;
              cmp   left[EBX],BL        ; - - - if( c != left[c] )
              _if   ne                  ; - - - then
                mov   DL,[ESI]          ; - - - - DL = *src++;
                inc   ESI               ; - - - - ...
                mov   right[EBX],DL     ; - - - - right[c] = DL;
              _endif                    ; - - - endif
              inc   EBX                 ; - - - c++
              dec   EAX                 ; - - - count--
            _until  s                   ; - - until( count < 0 )
            cmp   BL,0                  ; - - check for c == 256
          _until  e                     ; - until done
getsize:  sub   ECX,ECX                 ; - zero size
          mov   CH,[ESI]                ; - get high byte of size
          inc   ESI                     ; - ...
          mov   CL,[ESI]                ; - get low byte of size
          inc   ESI                     ; - ...
          sub   EBX,EBX                 ; - c = 0
          _loop                         ; - loop (unpack the data)
looptop:    dec   ECX                   ; - - decrement size
            _quif s                     ; - - quit if done
            mov   BL,[ESI]              ; - - get character
            inc   ESI                   ; - - ...
            sub   EDX,EDX               ; - - i = 0
            _loop                       ; - - loop
              _loop                     ; - - - loop
                cmp   BL,left[EBX]      ; - - - - quit if c == left[c]
                je    short L9          ; - - - - ...
                mov   AL,right[EBX]     ; - - - - push right[c] on stack
                mov   cstack[EDX],AL    ; - - - - ...
                inc   EDX               ; - - - - i++
                mov   BL,left[EBX]      ; - - - - c = left[c]
                cmp   BL,left[EBX]      ; - - - - quit if c == left[c]
                je    short L9          ; - - - - ...
                mov   AL,right[EBX]     ; - - - - push right[c] on stack
                mov   cstack[EDX],AL    ; - - - - ...
                inc   EDX               ; - - - - i++
                mov   BL,left[EBX]      ; - - - - c = left[c]
              _endloop                  ; - - - endloop
L9:           mov   [EDI],BL            ; - - - *dst++ = c;
              inc   EDI                 ; - - - ...
              dec   EDX                 ; - - - decrement i
              js    looptop             ; - - - jump to top of loop if i==0
              mov   BL,cstack[EDX]      ; - - - c = cstack[i]
            _endloop                    ; - - endloop
          _endloop                      ; - endloop
          cmp   ESI,EBP                 ; - quit if done
          _quif e                       ; - ...
          jmp   nxtblk                  ; - go upack next block
        _endguess                       ; endguess
        add     ESP,256+256+32          ; deallocate arrays
        pop     EBP                     ; restore registers
        pop     EDI                     ; ...
        pop     ESI                     ; ...
        pop     ECX                     ; ...
        ret                             ; return
BPE_Expand_ endp

_TEXT   ends
        end
