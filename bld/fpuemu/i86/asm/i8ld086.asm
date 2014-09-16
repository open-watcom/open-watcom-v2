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
;* Description:  convert 8-byte integer into long double
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    i8ld086, word

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __I8LD, __U8LD - convert 8-byte integer into long double
;<>
;<>   ifdef _BUILDING_MATHLIB
;<>     input:  SS:AX - pointer to 8-byte integer
;<>             SS:DX - pointer to long double operand 
;<>   else
;<>     input:  CX:BX:DX:AX - 8-byte integer
;<>             DS:SI - pointer to long double operand
;<>   endif
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __I8LD
        xdefp   __U8LD

;       __I8LD - convert 8-byte integer into long double
;       __U8LD - convert unsigned 8-byte integer into long double

        defp    __U8LD
        push    DI              ; save DI
        mov     DI,1            ; unsigned value
        jmp     short cont1

        defp    __I8LD
        push    DI              ; save DI
        xor     DI,DI           ; signed value
cont1:
ifdef _BUILDING_MATHLIB
        push    BP
        push    CX
        push    BX
        push    AX
        push    DX
        mov     BP,AX
        mov     CX,6[BP]        ; get 8-byte integer
        mov     BX,4[BP]        ; ...
        mov     DX,2[BP]        ; ...
        mov     AX,[BP]         ; ...
endif
        _guess
          or    DI,DI           ; if unsigned input
          mov   DI,403Eh        ; get exponent and sign
          _quif nz              ; then
          or    CX,CX           ; if number is negative
          _if s                 ; then
            not CX              ; - negate the value
            not BX              ; - ...
            not DX              ; - ...
            neg AX              ; - ...
            sbb DX,-1           ; - ...
            sbb BX,-1           ; - ...
            sbb CX,-1           ; - ...
            or  DI,8000h        ; - turn on sign bit
          _endif                ; endif
        _endguess
        _guess
          or    CX,CX           ; if high order word is 0
          _quif ne
          sub   DI,16           ; - adjust exponent
          or    CX,BX           ; - shift operand left 16 bits
          mov   BX,DX           ; - ...
          mov   DX,AX           ; - ...
          mov   AX,0            ; - ...
          _quif ne              ; - if next word was also 0
          sub   DI,16           ; - adjust exponent
          or    CX,BX           ; - shift operand left 16 bits again
          mov   BX,DX           ; - ...
          mov   DX,AX           ; - ...
          _quif ne              ; - if next word was also 0
          sub   DI,16           ; - adjust exponent
          or    CX,BX           ; - shift operand left 16 bits again
          mov   BX,DX           ; - ...
          _quif ne              ; - if last word was also 0
          mov   DI,AX           ; - set exponent to 0
        _admit
          cmp   CH,0            ; if high order byte is 0
          _if e                 ; then
            sub DI,8            ; - adjust exponent
            or  CH,CL           ; - shift up 8 bits
            mov CL,BH           ; - ...
            mov BH,BL           ; - ...
            mov BL,DH           ; - ...
            mov DH,DL           ; - ...
            mov DL,AH           ; - ...
            mov AH,AL           ; - ...
            mov AL,0            ; - ...
          _endif                ; endif
          _if ns                ; if not already normalized
            _loop               ; - loop (normalize result)
              dec   DI          ; - - decrement exponent
              _shl  AX,1        ; - - shift left 1 bit
              _rcl  DX,1        ; - - ...
              _rcl  BX,1        ; - - ...
              _rcl  CX,1        ; - - ...
            _until s            ; - until normalized
          _endif                ; endif
        _endguess
ifdef _BUILDING_MATHLIB
        pop     BP
        push    BP
        mov     8[BP],DI        ; store exponent
        mov     6[BP],CX        ; fraction
        mov     4[BP],BX        ; ...
        mov     2[BP],DX        ; ...
        mov     [BP],AX         ; ...
        pop     DX
        pop     AX
        pop     BX
        pop     CX
        pop     BP
else
        mov     8[SI],DI        ; store exponent
        mov     6[SI],CX        ; fraction
        mov     4[SI],BX        ; ...
        mov     2[SI],DX        ; ...
        mov     [SI],AX         ; ...
endif
        pop     DI              ; restore DI
        ret                     ; return
__I8LD  endp
__U8LD  endp


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
