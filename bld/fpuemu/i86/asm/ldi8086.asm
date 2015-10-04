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
;* Description:  convert long double into 8-byte integer
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    ldi8086, word

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;  
;   __LDI8 - convert long double into 8-byte integer with truncation
;            it is assumed that the long double has already been rounded
;            to an integer by calling __frndint.
;  
;ifdef _BUILDING_MATHLIB
;       input:  SS:AX - pointer to long double operand 
;               SS:DX - pointer to 8-byte integer result
;else
;       input:  DS:AX - pointer to long double operand
;       output: AX:BX:CX:DX - 8-byte integer
;endif
;  
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __LDI8

        defp    __LDI8
        push    SI
ifdef _BUILDING_MATHLIB
        push    BX              ; save BX
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
        push    DX
endif
        mov     BX,AX           ; setup parm
        _guess
          mov   AX,8[BX]        ; get exponent
          and   AX,7FFFh        ; isolate exponent
          cmp   AX,3FFFh        ; if number < 1.0
          _quif ae              ; then
          sub   AX,AX           ; - result is 0
          cwd                   ; - ...
          mov   BX,AX           ; - ...
          mov   CX,AX           ; - ...
        _admit
          cmp   AX,403Eh        ; if number too large
          _quif b               ; then
          mov   AX,8000h        ; - return largest number
          sub   BX,BX           ; - ...
          mov   CX,BX           ; - ...
          mov   DX,BX           ; - ...
        _admit
          push  8[BX]           ; get sign of the value
          sub   AX,403Eh        ; calculate # of bits to shift
          neg   AX              ; ...
          mov   SI,AX           ; get shift count
          mov   AX,6[BX]        ; get fraction
          mov   CX,2[BX]        ; ...
          mov   DX,[BX]         ; get fraction
          mov   BX,4[BX]        ; ...
          _loop                 ; loop
            cmp SI,16           ; - quit if < 16 bits to shift
            _quif l             ; - ...
            mov DX,CX           ; - shift right 16
            mov CX,BX           ; - ...
            mov BX,AX           ; - ...
            sub AX,AX           ; - ...
            sub SI,16           ; - adjust shift count
          _endloop              ; endloop
          cmp   SI,8            ; if >= 8 bits to shift
          _if ge                ; then
            mov DL,DH           ; - shift right 8 bits
            mov DH,CL           ; - ...
            mov CL,CH           ; - ...
            mov CH,BL           ; - ...
            mov BL,BH           ; - ...
            mov BH,AL           ; - ...
            mov AL,AH           ; - ...
            xor AH,AH           ; - ...
            sub SI,8            ; - adjust shift count
          _endif                ; endif
          cmp   SI,0            ; if some bits to shift
          _if ne                ; then
            _loop               ; - loop (bit shift)
              shr  AX,1         ; - - shift right 1 bit
              rcr  BX,1         ; - - ...
              rcr  CX,1         ; - - ...
              rcr  DX,1         ; - - ...
              dec  SI           ; - - decrement shift count
            _until e            ; - until done
          _endif                ; endif
;
          pop   SI              ; get sign of the value
          or    SI,SI           ; if negative
          _if s                 ; then
            not AX              ; - negate the value
            not BX              ; - ...
            not CX              ; - ...
            neg DX              ; - ...
            sbb CX,-1           ; - ...
            sbb BX,-1           ; - ...
            sbb AX,-1           ; - ...
          _endif                ; endif
        _endguess
ifdef _BUILDING_MATHLIB
        mov     SI,BX
        pop     BX
        mov     6[BX],AX
        mov     4[BX],SI
        mov     2[BX],CX
        mov     [BX],DX
        pop     BX              ; restore BX
        pop     DS              ; restore DS
endif
        pop     SI
        ret                     ; return to caller

        endproc __LDI8


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
