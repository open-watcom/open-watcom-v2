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


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __FDC - floating double comparison
;<>     input:  EDX:EAX - operand 1
;<>             ECX:EBX - operand 2
;<>       if op1 > op2,  1 is returned in EAX
;<>       if op1 < op2, -1 is returned in EAX
;<>       if op1 = op2,  0 is returned in EAX
;<>
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
include mdef.inc
include struct.inc

        modstart        fdc386

        xdefp   __FDC

        defpe   __FDC
        push    EBP             ; save BP
        test    EDX,07ff00000h  ; check for zero
        _if     e               ; if it is then
          sub   EDX,EDX         ; - make whole damn thing a zero
        _endif                  ; endif
        test    ECX,07ff00000h  ; check op2 for zero
        _if     e               ; if it is then
          sub   ECX,ECX         ; - make whole damn thing a zero
        _endif                  ; endif
        mov     EBP,ECX         ; save op2 exponent
        xor     EBP,EDX         ; see about signs of the operands
        mov     EBP,0           ; clear result
        js      short cmpdone   ; quif arg1 & arg2 have diff signs
        _guess                  ; guess
          cmp   EDX,ECX         ; - compare high words of arg1, arg2
          _quif ne              ; - quif not equal
          cmp   EAX,EBX         ; - compare 2nd  words of arg1, arg2
        _endguess               ; endguess
        _if     ne              ; if arg1 <> arg2
          rcr   ECX,1           ; - save carry in CX
          xor   EDX,ECX         ; - sign of BX is sign of result

cmpdone:  _shl  EDX,1           ; - get sign of result into carry
          sbb   EBP,0           ; - BP gets sign of result
          _shl  EBP,1           ; - double BP
          inc   EBP             ; - make BP -1 or 1
        _endif                  ; endif
        mov     EAX,EBP         ; get result
        pop     EBP             ; restore BP
        ret                     ; return to caller
        endproc __FDC

        endmod
        end
