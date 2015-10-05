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
;* Description:  convert long double into 4-byte integer
;*
;*****************************************************************************


ifdef _BUILDING_MATHLIB

include mdef.inc
include struct.inc
include shiftmac.inc

        modstart    ldi4086, word

endif

        xdefp   __LDI4
        xdefp   __LDU4

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;  
;   __LDI4 - convert long double into 4-byte integer
;            it is assumed that the long double has already been rounded
;            to an integer by calling __frndint.
;  
;ifdef _BUILDING_MATHLIB
;       input:  SS:AX - pointer to operand
;else
;       input:  DS:BX - pointer to operand
;endif
;       output: DX:AX - 4-byte integer
;  
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

__dw_rshift     proc    near

        _loop                   ; loop
          cmp CL,16             ; - quit if < 16 bits to shift
          _quif l               ; - ...
          dw_rshift_16          ; - shift right 16 bits
          sub CL,16             ; - adjust shift count
        _endloop                ; endloop
        cmp CL,8                ; if >= 8 bits to shift
        _if ge                  ; then
          dw_rshift_8           ; - shift right 8 bits
          sub CL,8              ; - adjust shift count
        _endif                  ; endif
        or  CL,CL               ; if some bits to shift
        _if nz                  ; then
          _loop                 ; - loop
            dw_rshift_1         ; - - shift right 1 bit
            dec  CL             ; - - decrement shift count
          _until z              ; - loop if some bits to shift
        _endif                  ; endif
        ret                     ; return

__dw_rshift     endp

        defp    __LDI4
        defp    __LDU4

ifdef _BUILDING_MATHLIB
        push    DS              ; save DS
        push    BX              ; save BX
        push    SS              ; fpc code assumes parms are relative to SS
        pop     DS              ; ...
        mov     BX,AX           ; setup parm
endif
        _guess
          mov   AX,8[BX]        ; get exponent
          and   AX,7FFFh        ; isolate exponent
          cmp   AX,3FFFh        ; if number < 1.0
          _quif ae              ; then
          sub   AX,AX           ; - result is 0
          cwd                   ; - ...
        _admit                  ; endif
          cmp   AX,401Eh        ; if number too large
          _quif b               ; then
          mov   DX,8000h        ; - return largest number
          sub   AX,AX           ; - ...
        _admit                  ; endif
          push  CX              ; save CX
          sub   AX,401Eh        ; calculate # of bits to shift
          neg   AX              ; ...
          mov   CL,AL           ; get shift count
          mov   DX,6[BX]        ; get fraction
          mov   AX,4[BX]        ; ...
          call __dw_rshift      ; shift fraction right
          mov   CX,8[BX]        ; get sign of the value
          or    CX,CX           ; if negative
          _if   s               ; then
            not DX              ; - negate the value
            neg AX              ; - ...
            sbb DX,-1           ; - ...
          _endif                ; endif
          pop   CX              ; restore CX
        _endguess
ifdef _BUILDING_MATHLIB
        pop     BX              ; restore BX
        pop     DS              ; restore DS
endif
        ret                     ; return to caller

        endproc __LDU4
        endproc __LDI4


ifdef _BUILDING_MATHLIB

        endmod

        endf    equ end
else
        endf    equ <>

endif

endf
