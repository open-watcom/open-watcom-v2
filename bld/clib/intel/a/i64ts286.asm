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
;                               AX              DX
;
        xdefp   __CmpBigInt
        xdefp   __Rnd2Int
        xdefp   __Bin2String

        defpe   __CmpBigInt
        push    BP              ; save BP
        push    SI              ; save SI
        push    DI              ; save DI
        push    CX              ; save CX
        push    BX              ; save BX
        call    getpow10        ; get address of Power of 10 table
        inc     AX              ; ++sigdigits
        shl     AX,1            ; sigdigits times 8
        shl     AX,1            ; . . .
        shl     AX,1            ; . . .
        add     DI,AX           ; point to Pow10Table[sigdigits+1]
        mov     BP,DX           ; get address of 64-bit integer
        mov     DX,[BP]         ; get 64-bit integer
        mov     CX,2[BP]        ; . . .
        mov     BX,4[BP]        ; . . .
        mov     AX,6[BP]        ; . . .
        sub     BP,BP           ; set adjustment to 0
        _loop                   ; loop
          call  DoCmpBigInt     ; - check against 10**k
          _quif b               ; - quit if num < 10**k
          add   DI,8            ; - set pointer to 10**(k+1)
          inc   BP              ; - increment adjustment word
        _endloop                ; endloop
        sub     DI,8            ; point at 10**(k-1)
        _loop                   ; loop
          call  DoCmpBigInt     ; - check against 10**(k-1)
          _quif nb              ; - quit if num >= 10**(k-1)
          sub   DI,8            ; - set pointer to 10**(k-2)
          dec   BP              ; - increment adjustment word
        _endloop                ; endloop
        mov     AX,BP           ; place adjustment in AX
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DI              ; restore DI
        pop     SI              ; restore SI
        pop     BP              ; restore BP
        ret                     ; return to caller
        endproc __CmpBigInt

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] DoCmpbigint compares AL BX CX DX with the integer in the code segment
;[] pointed to by SI
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

DoCmpbigint proc near
        _guess
          cmp   AX,cs:[DI]      ; compare high words
          _quif ne              ;
          cmp   BX,cs:2[DI]     ; compare next words
          _quif ne              ;
          cmp   CX,cs:4[DI]     ; compare next words
          _quif ne              ;
          cmp   DX,cs:6[DI]     ; compare low words
        _endguess               ;
        ret
        endproc DoCmpbigint

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] Rnd2int rounds the real pointed to by AX to a 64 bit integer.
;[] register use: AX BX CX DX
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       void _Rnd2Int( double near *realnum, int near *bigint )
;                               AX                 DX
;
        defpe   __Rnd2int
        push    DX              ; save registers
        push    CX              ; . . .
        push    BX              ; . . .
        push    BP              ; save BP
        push    SI              ; save SI
        push    DI              ; save DI
        push    DX              ; save address of array
        mov     BP,AX           ; get address of realnum
        mov     DI,[BP]         ; load the number
        mov     CX,2[BP]        ; . . .
        mov     BX,4[BP]        ; . . .
        mov     SI,6[BP]        ; . . .
        mov     DX,SI           ; save SI
        and     DX,0FFF0h       ; isolate exponent in DX
        xor     SI,DX           ; isolate mantissa in SI
        xor     SI,0010h        ; turn on implied '1' bit in SI
        shr     DX,1            ; move exponent to bottom part of word
        shr     DX,1            ; . . .
        shr     DX,1            ; . . .
        shr     DX,1            ; . . .
        sub     DX,0433h        ; calculate difference from 2**53
        _if     ne              ; if not already the right size
          _if   a               ; - if too big
            _loop               ; - - loop
              shl DI,1          ; - - - shift real left by one
              rcl CX,1          ; - - - . . .
              rcl BX,1          ; - - - . . .
              rcl SI,1          ; - - - . . .
              dec DX            ; - - - decrement count
            _until e            ; - - until count = 0
          _else                 ; - else
            sub AX,AX           ; - - zero remainder
            sub BP,BP           ; - - zero remainder bit bucket
            _loop               ; - - loop
              shr       SI,1    ; - - - shift real right by one
              rcr       BX,1    ; - - - . . .
              rcr       CX,1    ; - - - . . .
              rcr       DI,1    ; - - - . . .
              rcr       AX,1    ; - - - save remainder
              adc       BP,0    ; - - - remember if any bits fell off end
              inc       DX      ; - - - increment count
            _until e            ; - - until e
            _guess      rup     ; - - have to round up number?
              cmp       AX,8000h; - - - compare remainder to .500000
              _quif     b,rup   ; - - - kick out if less than .5
              _if       e       ; - - - magicial stuff if looks like a .5
                 or     BP,BP   ; - - - any bits dropped off bottom?
                _if     e       ; - - - - if not
                  test  DI,1    ; - - - - - is bottom digit even?
                  _quif e,rup   ; - - - - - kick out if it is
                _endif          ; - - - - endif
              _endif            ; - - - endif
              add       DI,01   ; - - - round up the number
              adc       CX,00   ; - - - . . .
              adc       BX,00   ; - - - . . .
              adc       SI,00   ; - - - . . .
            _endguess           ; - - endguess
          _endif                ; - endif
        _endif                  ; endif
        pop     BP              ; get address of integer array
        mov     [BP],DI         ; store 64-bit integer
        mov     2[BP],CX        ; . . .
        mov     4[BP],BX        ; . . .
        mov     6[BP],SI        ; . . .
        pop     DI              ; restore DI
        pop     SI              ; restore SI
        pop     BP              ; restore BP
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        ret                     ; return
        endproc __Rnd2Int

;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;[]
;[] Bin2string  converts a binary integer into a string
;[]
;[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
;       void _Bin2String(
;               int near *bigint,       /* AX */
;               char near *bufptr,      /* DX */
;               int sigdigits )         /* BX */
;
        defpe   __Bin2string
        push    BP              ; save registers
        push    DI              ; ...
        push    SI              ; ...
        push    CX              ; ...
        push    BX              ; ...
        mov     DI,BX           ; get # of digits
        mov     BP,AX           ; get pointer to 64-bit integer
        mov     AX,6[BP]        ; get the integer
        mov     BX,4[BP]        ; ...
        mov     CX,2[BP]        ; ...
        mov     SI,[BP]         ; ...
        mov     BP,DX           ; get pointer to output buffer
        add     BP,DI           ; point to end of buffer
        mov     byte ptr [BP],0 ; put in null character
;
; input:
;       AX:BX:CX:SI - 64-bit integer
;       BP - pointer to buffer for digits
;       DI - digit count

        push    AX              ; save high word of quotient
        _loop                   ; loop
          pop   AX              ; - restore high word of quotient
          push  DI              ; - save # of digits left to go
          mov   DI,10000        ; - divisor is 10000
          sub   DX,DX           ; - zero high word
          or    AX,AX           ; - check high word
          jne   div1            ; - do all divides
          or    BX,BX           ; - check next word
          jne   div2            ; - skip first divide
          or    CX,CX           ; - check third word
          jne   div3            ; - skip first 2 divides
          or    SI,SI           ; - check low order word
          jne   div4            ; - go do last divide
          pop   DI              ; - restore # of digits
          push  AX              ; - save high word of quotient
          jmp   short div5      ; - result is 0
div1:     div   DI              ; - divide AX:BX:CX:SI by 10000
div2:     xchg  BX,AX           ; - ...
          div   DI              ; - ...
div3:     xchg  CX,AX           ; - ...
          div   DI              ; - ...
div4:     xchg  SI,AX           ; - ...
          div   DI              ; - ...

;   quotient is in BX:CX:SI:AX
;   remainder is in DX

          xchg  SI,AX           ; - move quotient to AX:BX:CX:SI
          xchg  CX,AX           ; - ...
          xchg  BX,AX           ; - ...
          pop   DI              ; - restore # of digits
          push  AX              ; - save high word of quotient
          mov   AX,DX           ; - get remainder
          mov   DL,100          ; - get divisor
          div   DL              ; - split remainder into 2 parts
          mov   DL,AH           ; - save low order part
          xor   AH,AH           ; - zero
          aam                   ; - split top part into 2 digits
          xchg  DX,AX           ; - DH, DL gets top 2 digits, AX gets low part
          xor   AH,AH           ; - zero
          aam                   ; - split low part into 2 digits
div5:     add   AX,3030h        ; - make ASCII digits
          add   DX,3030h        ; - ...
          sub   BP,4            ; - move back 4
          mov   3[BP],AL        ; - put low order digit in buffer
          dec   DI              ; - decrement digit count
          _quif e               ; - quit if done
          mov   2[BP],AH        ; - ...
          dec   DI              ; - decrement digit count
          _quif e               ; - quit if done
          mov   1[BP],DL        ; - ...
          dec   DI              ; - decrement digit count
          _quif e               ; - quit if done
          mov   [BP],DH         ; - put high order digit in buffer
          dec   DI              ; - decrement digit count
        _until  e               ; until done

        pop     AX              ; restore high word of quotient
        pop     BX              ; restore registers
        pop     CX              ; ...
        pop     SI              ; ...
        pop     DI              ; ...
        pop     BP              ; ...
        ret                     ; return
        endproc __Bin2string

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                  <>
;<>     64-bit integer powers of 10 table                            <>
;<>                                                                  <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
getpow10 proc   near            ; get address of powers of 10 table
        call    pow10end        ; call around the table
pow10table:                     ; powers of 10 table
        dw      00000h,00000h,00000h,00000h ; 0
        dw      00000h,00000h,00000h,00001h ; 10**00
        dw      00000h,00000h,00000h,0000ah ; 10**01
        dw      00000h,00000h,00000h,00064h ; 10**02
        dw      00000h,00000h,00000h,003e8h ; 10**03
        dw      00000h,00000h,00000h,02710h ; 10**04
        dw      00000h,00000h,00001h,086a0h ; 10**05
        dw      00000h,00000h,0000fh,04240h ; 10**06
        dw      00000h,00000h,00098h,09680h ; 10**07
        dw      00000h,00000h,005f5h,0e100h ; 10**08
        dw      00000h,00000h,03b9ah,0ca00h ; 10**09
        dw      00000h,00002h,0540bh,0e400h ; 10**10
        dw      00000h,00017h,04876h,0e800h ; 10**11
        dw      00000h,000e8h,0d4a5h,01000h ; 10**12
        dw      00000h,00918h,04e72h,0a000h ; 10**13
        dw      00000h,05af3h,0107ah,04000h ; 10**14
        dw      00003h,08d7eh,0a4c6h,08000h ; 10**15
        dw      00023h,086f2h,06fc1h,00000h ; 10**16
        dw      00163h,04578h,05d8ah,00000h ; 10**17
        dw      00de0h,0b6b3h,0a764h,00000h ; 10**18
        dw      08ac7h,02304h,089e8h,00000h ; 10**19
        dw      0ffffh,0ffffh,0ffffh,0ffffh ; MAX

pow10end proc   near
        pop     DI              ; get address of table
        ret                     ; return
        endproc pow10end
        endproc getpow10

        endmod
        end
