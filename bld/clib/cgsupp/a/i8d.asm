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


;========================================================================
;==     Name:           I8D                                            ==
;==     Operation:      Signed 8 byte divide                           ==
;==     Inputs:         EDX;EAX  Dividend                              ==
;==                     ECX;EBX  Divisor                               ==
;==     Outputs:        EDX;EAX  Quotient                              ==
;==                     ECX;EBX  Remainder (same sign as dividend)     ==
;==     Volatile:       none                                           ==
;==                                     same sign as dividend for      ==
;==                                     consistency with 8086 idiv     ==
;==                                     and so (a/b)*b + a%b == a      ==
;==                                     to get a 64-bit version for 386==
;========================================================================
include mdef.inc
include struct.inc

        modstart        i8d

        xdefp   __I8D

        defpe   __I8D
        or      edx,edx         ; check sign of dividend
        js      divneg          ; handle case where dividend < 0
        or      ecx,ecx         ; check sign of divisor
        js      notU8D          ; easy case if it is also positive

        ; dividend >= 0, divisor >= 0
        docall  __U8D           ; - ...
        ret                     ; - ...

        ; dividend >= 0, divisor < 0
notU8D: neg     ecx             ; take positive value of divisor
        neg     ebx             ; ...
        sbb     ecx,0           ; ...
        docall  __U8D           ; do unsigned division
        neg     edx             ; negate quotient
        neg     eax             ; ...
        sbb     edx,0           ; ...
        ret                     ; and return

divneg:                         ; dividend is negative
        neg     edx             ; take absolute value of dividend
        neg     eax             ; ...
        sbb     edx,0           ; ...
        or      ecx,ecx         ; check sign of divisor
        jns     negres          ; negative result if divisor > 0

        ; dividend < 0, divisor < 0
        neg     ecx             ; negate divisor too
        neg     ebx             ; ...
        sbb     ecx,0           ; ...
        docall  __U8D           ; and do unsigned division
        neg     ecx             ; negate remainder
        neg     ebx             ; ...
        sbb     ecx,0           ; ...
        ret                     ; and return

        ; dividend < 0, divisor >= 0
negres: docall  __U8D           ; do unsigned division
        neg     ecx             ; negate remainder
        neg     ebx             ; ...
        sbb     ecx,0           ; ...
        neg     edx             ; negate quotient
        neg     eax             ; ...
        sbb     edx,0           ; ...
        ret                     ; and return

        endproc __I8D

;========================================================================
;==     Name:           U8D                                            ==
;==     Operation:      Unsigned 8 byte divide                         ==
;==     Inputs:         EDX;EAX  Dividend                              ==
;==                     ECX;EBX  Divisor                               ==
;==     Outputs:        EDX;EAX  Quotient                              ==
;==                     ECX;EBX  Remainder                             ==
;==     Volatile:       none                                           ==
;========================================================================

        xdefp   __U8D

        defpe   __U8D
        or      ecx,ecx         ; check for easy case
        jne     noteasy         ; easy if divisor is 16 bit
        dec     ebx             ; decrement divisor
        _if     ne              ; if not dividing by 1
          inc   ebx             ; - put divisor back
          cmp   ebx,edx         ; - if quotient will be >= 64K
          _if   be              ; - then
;
;       12-aug-88, added thanks to Eric Christensen from Fox Software
;       divisor < 64K, dividend >= 64K, quotient will be >= 64K
;
;       *note* this sequence is used in ltoa's #pragmas; any bug fixes
;              should be reflected in ltoa's code bursts
;
            mov   ecx,eax       ; - - save low word of dividend
            mov   eax,edx       ; - - get high word of dividend
            sub   edx,edx       ; - - zero high part
            div   ebx           ; - - divide bx into high part of dividend
            xchg  eax,ecx       ; - - swap high part of quot,low word of dvdnd
          _endif                ; - endif
          div   ebx             ; - calculate low part
          mov   ebx,edx         ; - get remainder
          mov   edx,ecx         ; - get high part of quotient
          sub   ecx,ecx         ; - zero high part of remainder
        _endif                  ; endif
        ret                     ; return


noteasy:                        ; have to work to do division
;
;       check for divisor > dividend
;
        _guess                  ; guess: divisor > dividend
          cmp   ecx,edx         ; - quit if divisor <= dividend
          _quif b               ; - . . .
          _if   e               ; - if high parts are the same
            cmp   ebx,eax       ; - - compare the lower order words
            _if   be            ; - - if divisor <= dividend
              sub   eax,ebx     ; - - - calulate remainder
              mov   ebx,eax     ; - - - ...
              sub   ecx,ecx     ; - - - ...
              sub   edx,edx     ; - - - quotient = 1
              mov   eax,1       ; - - - ...
              ret               ; - - - return
            _endif              ; - - endif
          _endif                ; - endif
          sub   ecx,ecx         ; - set divisor = 0 (this will be quotient)
          sub   ebx,ebx         ; - ...
          xchg  eax,ebx         ; - return remainder = dividend
          xchg  edx,ecx         ; - and quotient = 0
          ret                   ; - return
        _endguess               ; endguess
        push    ebp              ; save work registers
        push    esi              ; ...
        push    edi              ; ...
        sub     esi,esi           ; zero quotient
        mov     edi,esi           ; ...
        mov     ebp,esi           ; and shift count
moveup:                         ; loop until divisor > dividend
          _shl    ebx,1         ; - divisor *= 2
          _rcl    ecx,1         ; - ...
          jc      backup        ; - know its bigger if carry out
          inc     ebp           ; - increment shift count
          cmp     ecx,edx       ; - check if its bigger yet
          jb      moveup        ; - no, keep going
          ja      divlup        ; - if below, know we're done
          cmp     ebx,eax       ; - check low parts (high parts equal)
          jbe     moveup        ; until divisor > dividend
divlup:                         ; division loop
        clc                     ; clear carry for rotate below
        _loop                   ; loop
          _loop                 ; - loop
            _rcl  esi,1         ; - - shift bit into quotient
            _rcl  edi,1         ; - - . . .
            dec   ebp           ; - - quif( -- shift < 0 ) NB carry not changed
            js    donediv       ; - - ...
backup:                         ; - - entry to remove last shift
            rcr   ecx,1         ; - - divisor /= 2 (NB also used by 'backup')
            rcr   ebx,1         ; - - ...
            sub   eax,ebx       ; - - dividend -= divisor
            sbb   edx,ecx       ; - - c=1 iff it won't go
            cmc                 ; - - c=1 iff it will go
          _until  nc            ; - until it won't go
          _loop                 ; - loop
            _shl  esi,1         ; - - shift 0 into quotient
            _rcl  edi,1         ; - - . . .
            dec   ebp           ; - - going to add, check if done
            js    toomuch       ; - - if done, we subtracted to much
            shr   ecx,1         ; - - divisor /= 2
            rcr   ebx,1         ; - - ...
            add   eax,ebx       ; - - dividend += divisor
            adc   edx,ecx       ; - - c = 1 iff bit of quotient should be 1
          _until  c             ; - until divisor will go into dividend
        _endloop                ; endloop
toomuch:                        ; we subtracted too much
        add     eax,ebx         ; dividend += divisor
        adc     edx,ecx         ; ...
donediv:                        ; now quotient in di;si, remainder in dx;ax
        mov     ebx,eax         ; move remainder to cx;bx
        mov     ecx,edx         ; ...
        mov     eax,esi         ; move quotient to dx;ax
        mov     edx,edi         ; ...
        pop     edi             ; restore registers
        pop     esi             ; ...
        pop     ebp             ; ...
        ret                     ; and return
        endproc __U8D

        endmod
        end
