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


; This module converts a string to long_double
;       void __ZBuf2LD( char *buf, long_double *value );
;
include mdef.inc
include struct.inc

        modstart bufld086

        xdefp   __ZBuf2LD

;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>                                                                   <>
;<>   __ZBuf2LD - convert buffer of significant digits into floating  <>
;<>   void __ZBuf2LD( char near *buf, long_double near *value )       <>
;<>                                                                   <>
;<>   input:  SS:AX - address of buffer of significant digits         <>
;<>           SS:DX - place to store value                            <>
;<>   output: SS:[DX]      - floating-point number                    <>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        defpe   __ZBuf2LD
        push    BP              ; save BP
        push    SI              ; save SI
        push    DI              ; save DI
        push    CX              ; save CX
        push    BX              ; save BX
        push    DX              ; save pointer to result
        mov     SI,AX           ; get address of buffer
        sub     DX,DX           ; set 80-bit integer to 0
        mov     DI,DX           ; ...
        mov     CX,DX           ; ...
        mov     BX,DX           ; ...
        mov     BP,DX           ; ...
        _loop                   ; loop (convert digits into 54-bit int)
          mov   AL,ss:[SI]      ; - get next digit
          cmp   AL,0            ; - quit if at end of buffer
          _quif e               ; - ...

;[]  multiply current value in DX:BX:CX:DI:BP by 10

          push  DX              ; - save current value
          push  BX              ; - ...
          push  CX              ; - ...
          push  DI              ; - ...
          mov   AX,BP           ; - ...

          _shl  BP,1            ; - multiply number by 4
          _rcl  DI,1            ; -   by shifting left 2 places
          _rcl  CX,1            ; - ...
          _rcl  BX,1            ; - ...
          _rcl  DX,1            ; - ...

          _shl  BP,1            ; - ...
          _rcl  DI,1            ; - ...
          _rcl  CX,1            ; - ...
          _rcl  BX,1            ; - ...
          _rcl  DX,1            ; - ...

          add   BP,AX           ; - add original value
          pop   AX              ; -  (this will make it times 5)
          adc   DI,AX           ; - ...
          pop   AX              ; - ...
          adc   CX,AX           ; - ...
          pop   AX              ; - ...
          adc   BX,AX           ; - ...
          pop   AX              ; - ...
          adc   DX,AX           ; - ...

          _shl  BP,1            ; - shift left to make it times 10
          _rcl  DI,1            ; - ...
          _rcl  CX,1            ; - ...
          _rcl  BX,1            ; - ...
          _rcl  DX,1            ; - ...

          mov   AL,ss:[SI]      ; - add in current digit
          and   AX,000Fh        ; - ...
          add   BP,AX           ; - ...
          adc   DI,0            ; - ...
          adc   CX,0            ; - ...
          adc   BX,0            ; - ...
          adc   DX,0            ; - ...

          inc   SI              ; - point to next digit in buffer
        _endloop                ; endloop
        mov     AX,DX           ; get high order word into AX
        mov     DX,DI           ; set up integer into AX:BX:CX:DX:BP

;[] Turn the integer in AX:BX:CX:DX:BP into a real number

        mov     DI,3FFFh+63+16  ; set exponent
        call    Norm            ; convert the 80 bit integer to a float
        pop     BP              ; restore pointer to result
        mov     8[BP],SI        ; store result
        mov     6[BP],AX        ; ...
        mov     4[BP],BX        ; ...
        mov     2[BP],CX        ; ...
        mov     0[BP],DX        ; ...
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DI              ; restore DI
        pop     SI              ; restore SI
        pop     BP              ; restore BP
        ret                     ; return to caller
        endproc __ZBuf2LD


; Norm normalizes an unsigned real in AX:BX:CX:DX:BP
; DI - contains initial exponent
; SI - will contain final exponent

Norm    proc    near            ; normalize floating point number
        mov     SI,AX           ; see if the integer is zero
        or      SI,BX           ; ...
        or      SI,CX           ; ...
        or      SI,DX           ; ...
        or      SI,BP           ; ...
        _if     ne              ; if number is non-zero
          _loop                 ; - loop
            or    AX,AX         ; - - quit if high word is non-zero
            _quif ne            ; - - ...
            mov   AX,BX         ; - - shift integer left by 16 bits
            mov   BX,CX         ; - - ...
            mov   CX,DX         ; - - ...
            mov   DX,BP         ; - - ...
            sub   BP,BP         ; - - ...
            sub   DI,16         ; - - exp <-- exp - 16
          _endloop              ; - endloop
          _loop                 ; - loop
            or    AX,AX         ; - - quit if number normalized
            _quif s             ; - - ...
            _shl  BP,1          ; - - shift integer left by 1 bit
            _rcl  DX,1          ; - - ...
            _rcl  CX,1          ; - - ...
            _rcl  BX,1          ; - - ...
            _rcl  AX,1          ; - - ...
            dec   DI            ; - - decrement exponent
          _endloop              ; - endloop
          _rcl  BP,1            ; - get lsb
          adc   DX,0            ; - and use it to round off the number
          adc   CX,0            ; - ...
          adc   BX,0            ; - ...
          adc   AX,0            ; - ...
          _if   c               ; - if carry
            mov   AH,80h        ; - - set high bit (all other bits are 0)
            inc   DI            ; - - bump exponent
          _endif                ; - endif
          mov   SI,DI           ; - get exponent
        _endif                  ; endif
        ret                     ; return
        endproc Norm

        endmod
        end
