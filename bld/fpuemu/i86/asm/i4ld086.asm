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
;* Description:  convert 4-byte integer into long double
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    i4ld086, word

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __I4LD, __U4LD - convert 4-byte integer into long double
;<>
;<>   ifdef _BUILDING_MATHLIB
;<>     input:  DX:AX - long
;<>             SS:BX - pointer to long double operand 
;<>   else
;<>     input:  DX:AX - long
;<>             DS:BX - pointer to long double operand
;<>   endif
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

;       __I4LD - convert long into long double
;       __U4LD - convert unsigned long into long double

        xdefp   __I4LD
        xdefp   __U4LD

        defp    __I4LD
        or      DX,DX           ; if number is negative
        _if     s               ; then
          not   DX              ; - negate the value
          neg   AX              ; - ...
          sbb   DX,-1           ; - ...
          push  CX              ; - save CX
          mov   CX,0C01Eh       ; - set exponent
        _else                   ; else

        defp    __U4LD          ; - convert unsigned long to long double
          push  CX              ; - save CX
          mov   CX,0401Eh       ; - set exponent
        _endif                  ; endif
        or      DX,DX           ; if high order word is 0
        _if     e               ; then
          sub   CX,16           ; - adjust exponent
          xchg  AX,DX           ; - flip low order word to the top
        _endif                  ; endif
        cmp     DH,0            ; if high order byte is 0
        _if     e               ; then
          mov   DH,DL           ; - shift up 8 bits
          mov   DL,AH           ; - ...
          mov   AH,AL           ; - ...
          xor   AL,AL           ; - ...
          sub   CX,8            ; - adjust exponent
        _endif                  ; endif
        or      DH,DH           ; if high order byte is 0
        _if     e               ; then
          sub   CX,CX           ; - whole answer is 0
        _else                   ; else
          _if   ns              ; - if not already normalized
            _loop               ; - - loop (normalize result)
              dec   CX          ; - - - decrement exponent
              _shl  AX,1        ; - - - shift left 1 bit
              _rcl  DX,1        ; - - - ...
            _until  s           ; - - until normalized
          _endif                ; - endif
        _endif                  ; endif
ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
endif
        mov     8[BX],CX        ; store exponent
        mov     6[BX],DX        ; fraction
        mov     4[BX],AX        ; ...
        sub     AX,AX           ; rest is 0
        mov     2[BX],AX        ; ...
        mov     [BX],AX         ; ...
ifdef _BUILDING_MATHLIB
        pop     DS              ; restore DS
endif
        pop     CX              ; restore CX
        ret                     ; return
__U4LD  endp
__I4LD  endp


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
