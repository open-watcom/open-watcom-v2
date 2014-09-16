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

        modstart        i64tos

;
;
;       int _CmpBigInt( int sigdigits, int near *bigint )
;                              EAX             EDX
;
        xdefp   __CmpBigInt
        xdefp   __Rnd2Int
        xdefp   __Bin2String

        defpe   __CmpBigInt
        push    EDI             ; save EDI
        push    ECX             ; save ECX
        call    getpow10        ; get address of Power of 10 table
        inc     EAX             ; ++sigdigits
        lea     EDI,[EDI+EAX*8] ; point to Pow10Table[sigdigits+1]
        mov     ECX,[EDX]       ; get 64-bit integer
        mov     EDX,4[EDX]      ; ...(high part)
        sub     EAX,EAX         ; set adjustment to 0
        _loop                   ; loop
          cmp   EDX,cs:[EDI]    ; - check against 10**k
          _if   e               ; - if high parts equal
            cmp   ECX,cs:4[EDI] ; - - compare low part
          _endif                ; - endif
          _quif b               ; - quit if num < 10**k
          add   EDI,8           ; - set pointer to 10**(k+1)
          inc   EAX             ; - increment adjustment word
        _endloop                ; endloop
        sub     EDI,8           ; point at 10**(k-1)
        _loop                   ; loop
          cmp   EDX,cs:[EDI]    ; - check against 10**k
          _if   e               ; - if high parts equal
            cmp   ECX,cs:4[EDI] ; - - compare low part
          _endif                ; - endif
          _quif nb              ; - quit if num >= 10**(k-1)
          sub   EDI,8           ; - set pointer to 10**(k-2)
          dec   EAX             ; - increment adjustment word
        _endloop                ; endloop
        pop     ECX             ; restore ECX
        pop     EDI             ; restore EDI
        ret                     ; return to caller
        endproc __CmpBigInt

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] Rnd2int rounds the real pointed to by EAX to a 64 bit integer.
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       void _Rnd2Int( double near *realnum, int near *bigint )
;                               EAX                 EDX
;
        defpe   __Rnd2Int
        push    EBX             ; save registers
        push    ECX             ; save ECX
        push    EDX             ; save EDX
        push    EBP             ; save EBP
        push    ESI             ; save ESI
        push    EDX             ; save address of bigint array
        mov     EBP,EAX         ; get address of realnum
        mov     ECX,[EBP]       ; load the number
        mov     EBX,4[EBP]      ; . . .
        mov     EBP,EBX         ; save high word
        and     EBP,0FFF00000h  ; isolate exponent in EBP
        xor     EBX,EBP         ; isolate mantissa in EDX
        xor     EBX,00100000h   ; turn on implied '1' bit in mantissa
        shr     EBP,20          ; move exponent to bottom part of word
        sub     EBP,0433h       ; calculate difference from 2**53
        _if     ne              ; if not already the right size
          _if   a               ; - if too big
            _loop               ; - - loop
              shl   ECX,1       ; - - - shift real left by one
              rcl   EBX,1       ; - - - . . .
              dec   EBP         ; - - - decrement count
            _until e            ; - - until count = 0
          _else                 ; - else
            sub EAX,EAX         ; - - clear remainder
            sub ESI,ESI         ; - - clear remainder bit bucket
            _loop               ; - - loop
              shr   EBX,1       ; - - - shift real right by one
              rcr   ECX,1       ; - - - . . .
              rcr   EAX,1       ; - - - save remainder
              adc   ESI,ESI     ; - - - remember if any bits fell off end
              inc   EBP         ; - - - increment count
            _until e            ; - - until e
            _guess rup          ; - - do we have to round up?
              cmp   EAX,80000000h;- - - compare remainder with .5000000
              _quif b,rup       ; - - - kick out if less than .5
              _if   e           ; - - - magical stuff if looks like a .5
                or      ESI,ESI ; - - - any bits dropped off the bottom
                _if     e       ; - - - if not
                  test  ECX,1   ; - - - - - is bottom digit even?
                  _quif e,rup   ; - - - - - kick out if it is
                _endif          ; - - - - endif
              _endif            ; - - - endif
              add   ECX,01      ; - - - round up the number
              adc   EBX,00      ; - - - . . .
            _endguess           ; - - endguess
          _endif                ; - endif
        _endif                  ; endif
        pop     EBP             ; get address of bigint array
        mov     [EBP],ECX       ; store 64-bit integer
        mov     4[EBP],EBX      ; . . .
        pop     ESI             ; restore ESI
        pop     EBP             ; restore EBP
        pop     EDX             ; restore EDX
        pop     ECX             ; restore ECX
        pop     EBX             ; restore EBX
        ret                     ; return
        endproc __Rnd2Int

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] Bin2string  converts a binary integer into a string
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       void _Bin2String(
;               int near *bigint,       /* EAX */
;               char near *bufptr,      /* EDX */
;               int sigdigits )         /* EBX */
;
        defpe   __Bin2String
        push    EBP             ; save EBP
        push    EDI             ; save EDI
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        mov     EBP,EAX         ; get address of bigint array
        mov     ECX,[EBP]       ; get 64-bit integer
        mov     EAX,4[EBP]      ; . . .
        mov     EBP,EDX         ; get buffer pointer
        add     EBP,EBX         ; point to end of buffer
        mov     byte ptr [EBP],0; put in null character
;
; input:
;       EAX:ECX - 64-bit integer
;       EBP - pointer to buffer for digits
;       EBX - digit count

        push    EAX             ; save high word of quotient
        _loop                   ; loop
          pop   EAX             ; - restore high word of quotient
          mov   EDI,10000       ; - divisor is 10000
          sub   EDX,EDX         ; - zero high word
          or    EAX,EAX         ; - check high word
          jne   div1            ; - do all divides
          or    ECX,ECX         ; - check low order word
          jne   div2            ; - skip first divide
          push  EAX             ; - save high word of quotient
          jmp   short div5      ; - result is 0
div1:     div   EDI             ; - divide EAX:ECX by 10000
div2:     xchg  ECX,EAX         ; - ...
          div   EDI             ; - ...

;   quotient is in ECX:EAX
;   remainder is in EDX

          xchg  ECX,EAX         ; - move quotient to EAX:ECX
          push  EAX             ; - save high word of quotient
          mov   EAX,EDX         ; - get remainder
          mov   DL,100          ; - get divisor
          div   DL              ; - split remainder into 2 parts
          mov   DL,AH           ; - save low order part
          xor   AH,AH           ; - zero
          aam                   ; - split top part into 2 digits
          xchg  EDX,EAX         ; - DH, DL gets top 2 digits, AX gets low part
          xor   AH,AH           ; - zero
          aam                   ; - split low part into 2 digits
div5:     add   AX,3030h        ; - make ASCII digits
          add   DX,3030h        ; - ...
          sub   EBP,4           ; - move back 4
          mov   3[EBP],AL       ; - put low order digit in buffer
          dec   EBX             ; - decrement digit count
          _quif e               ; - quit if done
          mov   2[EBP],AH       ; - ...
          dec   EBX             ; - decrement digit count
          _quif e               ; - quit if done
          mov   1[EBP],DL       ; - ...
          dec   EBX             ; - decrement digit count
          _quif e               ; - quit if done
          mov   [EBP],DH        ; - put high order digit in buffer
          dec   EBX             ; - decrement digit count
        _until  e               ; until done

        pop     EAX             ; remove high word of quotient
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        pop     EDI             ; restore EDI
        pop     EBP             ; restore EBP
        ret                     ; return
        endproc __Bin2String

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                  <>
;<>     64-bit integer powers of 10 table                            <>
;<>                                                                  <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
getpow10 proc   near            ; get address of powers of 10 table
        call    pow10end        ; call around the table
pow10table:                     ; powers of 10 table
        dd      000000000h,000000000h ; 0
        dd      000000000h,000000001h ; 10**00
        dd      000000000h,00000000ah ; 10**01
        dd      000000000h,000000064h ; 10**02
        dd      000000000h,0000003e8h ; 10**03
        dd      000000000h,000002710h ; 10**04
        dd      000000000h,0000186a0h ; 10**05
        dd      000000000h,0000f4240h ; 10**06
        dd      000000000h,000989680h ; 10**07
        dd      000000000h,005f5e100h ; 10**08
        dd      000000000h,03b9aca00h ; 10**09
        dd      000000002h,0540be400h ; 10**10
        dd      000000017h,04876e800h ; 10**11
        dd      0000000e8h,0d4a51000h ; 10**12
        dd      000000918h,04e72a000h ; 10**13
        dd      000005af3h,0107a4000h ; 10**14
        dd      000038d7eh,0a4c68000h ; 10**15
        dd      0002386f2h,06fc10000h ; 10**16
        dd      001634578h,05d8a0000h ; 10**17
        dd      00de0b6b3h,0a7640000h ; 10**18
        dd      08ac72304h,089e80000h ; 10**19
        dd      0ffffffffh,0ffffffffh ; MAX

pow10end proc   near
        pop     EDI             ; get address of table
        ret                     ; return
        endproc pow10end
        endproc getpow10

        endmod
        end
