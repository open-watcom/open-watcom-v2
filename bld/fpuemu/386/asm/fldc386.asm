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
;* Description:  long double math library - comparison  __FLDC
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc

        modstart    fldc386, dword

endif

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;
;   __FLDC - long double comparison
;  
;       input:  EAX - pointer to operand 1
;               EDX - pointer to operand 2
;       output: AX - result
;  
;         if op1 > op2,  1 is returned in EAX
;         if op1 < op2, -1 is returned in EAX
;         if op1 = op2,  0 is returned in EAX
;         if either opnd is NaN, then 2 is returned in EAX
;  
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        xdefp   __FLDC

        defp    __FLDC
        push    EDI             ; save EDI
        push    ESI             ; save ESI
        _guess                  ; guess: op1 is not a NaN
          mov   SI,8[EAX]       ; - get sign and exponent of op1
          or    SI,8000H        ; - turn on sign bit (all 1's if a NaN)
          inc   SI              ; - will be 0 if a NaN
          _quif ne              ; - quit if not a NaN
          mov   ESI,4[EAX]      ; - get high order word of fraction
          _shl  ESI,1           ; - get rid of implied 1 bit
          or    ESI,[EAX]       ; - or in rest of fraction
          jne   short NaN       ; - non-zero fraction means its a NaN
        _endguess               ; endguess
        _guess                  ; guess: op2 is not a NaN
          mov   SI,8[EDX]       ; - get sign and exponent of op2
          or    SI,8000H        ; - turn on sign bit (all 1's if a NaN)
          inc   SI              ; - will be 0 if a NaN
          _quif ne              ; - quit if not a NaN
          mov   ESI,4[EDX]      ; - get high order word of fraction
          _shl  ESI,1           ; - get rid of implied 1 bit
          or    ESI,[EDX]       ; - or in rest of fraction
          jne   short NaN       ; - non-zero fraction means its a NaN
        _endguess               ; endguess
        mov     ESI,6[EAX]      ; get sign of op1
        mov     EDI,6[EDX]      ; get op2 exponent and sign
        xor     EDI,ESI         ; see about signs of the operands
        mov     EDI,0           ; clear result
        js      short chkfor0   ; quif arg1 & arg2 have diff signs
        _guess                  ; guess
          mov   SI,8[EAX]       ; - get exponent
          cmp   SI,8[EDX]       ; - compare them
          _quif ne              ; - quif not equal
          mov   EDI,4[EAX]      ; - get high part of fraction
          cmp   EDI,4[EDX]      ; - compare them
          _quif ne              ; - quif not equal
          mov   EDI,[EAX]       ; - get low part of fraction
          cmp   EDI,[EDX]       ; - compare them
        _endguess               ; endguess
        mov     EDI,0           ; clear result
        _if     ne              ; if arg1 <> arg2
          rcr   EAX,1           ; - save carry in EAX
          xor   ESI,EAX         ; - sign of ESI is sign of result

cmpdone:  _shl  ESI,1           ; - get sign of result into carry
          sbb   EDI,0           ; - EDI gets sign of result
          _shl  EDI,1           ; - double EDI
          inc   EDI             ; - make EDI -1 or 1
        _endif                  ; endif
        mov     EAX,EDI         ; get result
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return to caller

NaN:    mov     EAX,2           ; indicate NaN
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return to caller

chkfor0:or      EDI,[EAX]       ; see if both operands are 0
        or      EDI,[EDX]       ; ...
        or      EDI,4[EAX]      ; ...
        or      EDI,4[EDX]      ; ...
        mov     AX,8[EAX]       ; get exponnet
        shl     EAX,16          ; ...
        mov     AX,8[EDX]       ; ...
        and     EAX,7FFF7FFFh   ; get rid of signs
        or      EAX,EDI         ; check for 0
        mov     EDI,0           ;
        jne     cmpdone         ; if not zero
        pop     ESI             ; restore ESI
        pop     EDI             ; restore EDI
        ret                     ; return to caller

        endproc __FLDC


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
