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

        modstart    i8ld386, dword

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __I8LD, __U8LD - convert 8-byte integer to long double
;<>
;<>   ifdef _BUILDING_MATHLIB
;<>     input:  EAX - pointer to 8-byte integer
;<>             EDX - pointer to long double operand 
;<>   else
;<>     input:  EDX:EAX - 8-byte integer
;<>             EBX - pointer to long double operand
;<>   endif
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __I8LD
        xdefp   __U8LD

;       __I8LD - convert signed 8-byte integer into long double
;       __U8LD - convert unsigned 8-byte integer into long double

        defp    __U8LD
        push    ECX              ; save DI
        mov     CL,1            ; unsigned value
        jmp     short cont1

        defp    __I8LD
        push    ECX             ; save ECX
        xor     CL,CL           ; signed value
cont1:
ifdef _BUILDING_MATHLIB
        push    EBX
        mov     EBX,EDX
        mov     EDX,4[EAX]      ; get 8-byte integer
        mov     EAX,[EAX]       ; ...
endif
        _guess
          or    CL,CL           ; if unsigned input
          mov   CX,403Eh        ; get exponent and sign
          _quif nz              ; then
          or    EDX,EDX         ; if number is negative
          _if s                 ; then
            neg EDX             ; - negate the value
            neg EAX             ; - ...
            sbb EDX,0           ; - ...
            or  CX,8000h        ; - turn on sign bit
          _endif                ; endif
        _endguess
        _guess
          or    EDX,EDX         ; if high order word is 0
          _quif ne
          sub   CL,32           ; - adjust exponent
          or    EDX,EAX         ; - shift operand left 32 bits
          mov   EAX,0           ; - ...
          _quif ne              ; - if last word was also 0
          mov   CX,AX           ; - set exponent to 0
        _admit
          _if ns                ; if not already normalized
            _loop               ; - loop (normalize result)
              dec   CX          ; - - decrement exponent
              shld  EDX,EAX,1   ; - - shift left 1 bit
              shl   EAX,1       ; - - ...
            _until s            ; - until normalized
          _endif                ; endif
        _endguess
        mov     8[EBX],CX        ; store exponent
        mov     4[EBX],EDX       ; fraction
        mov     [EBX],EAX        ; ...
ifdef _BUILDING_MATHLIB
        pop     EBX
endif
        pop     ECX             ; restore ECX
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
