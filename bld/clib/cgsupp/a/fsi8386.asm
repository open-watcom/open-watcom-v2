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


;
;                               from float to signed/unsigned int64
;
include mdef.inc
include struct.inc

        modstart        fsi8386

        xref    __U8LS
        xref    __U8RS

; Convert single precision float to unsigned 64-bit integer with truncation
; Input: [EAX] = 32-bit float
; Output: [EDX, EAX] = 64-bit integer

        xdefp   __FSU8
        defp    __FSU8
        push    ECX             ; save ECX
        mov     CL,7fh+64       ; maximum number 2^64-1
        call    __FSU           ; convert float to unsigned __int64
        pop     ECX             ; restore ECX
        ret                     ; return
        endproc __FSU8

; Convert single precision float to signed 64-bit integer with truncation
; Input: [EAX] = 32-bit float
; Output: [EDX, EAX] = 64-bit integer

        xdefp   __FSI8
        defp    __FSI8
        push    ECX             ; save ECX
        mov     CL,7fh+63       ; maximum number 2^63-1
        call    __FSI           ; convert float to signed __int64
        pop     ECX             ; restore ECX
        ret                     ; return
        endproc __FSI8

__FSI   proc    near
__FSU:                          ; what? they both do the same thing
        or      EAX,EAX         ; check sign bit
        jns     short __FSAbs   ; treat as unsigned if positive
        call    __FSAbs         ; otherwise convert number
        neg     EDX             ; negate the result
        neg     EAX             ;
        sbb     EDX,0           ;
        ret                     ; return
        endproc __FSI

__FSAbs proc near
        or      EAX,EAX         ; check if number 0
        je      short fltzero   ; if so, just return 0
        _shl    EAX,1           ; shift mantissa over
        rol     EAX,8           ; get exponent to bottom
        cmp     AL,7fh          ; quit if number < 1.0
        jb      short fltuflow  ; ...
        push    EBX             ; save EBX
        mov     CH,AL           ; save exponent
        stc                     ; set carry for implied bit
        rcr     EAX,1           ; put implied '1' bit in
        shr     EAX,8           ; remove exponent and extra bit
        mov     EDX,EAX         ; put into 64-bit hi part
        xor     EAX,EAX         ; zero 64-bit lo part
        cmp     CH,CL           ; check if exponent exceeds maximum
        jae     short fltmax    ; return maximum value if so
        sub     CH,7fh+55       ; calculate amount to shift (+ve -> left)
        jae     short fltm_left ; jump if left shift/no shift
        xchg    CH,CL           ; get shift count
        neg     CL              ; make positive
        mov     BX,CX
        call    __U8RS          ; shift mantissa right
        pop     EBX             ; restore EBX
        ret                     ; return

fltm_left:
        _if     ne              ; done if exponent exactly 55
          mov     BL,CH         ; - get shift count
          call    __U8LS        ; - shift mantissa left
        _endif                  ; endif
        pop     EBX             ; restore EBX
        ret                     ; return

; CL = 7fh+64  for unsigned
;      7fh+63  for signed
fltmax:
        mov     EAX,0FFFFFFFFh  ; return maximum value
        mov     EDX,EAX         ;
        sub     CL,7fh+64       ; subtract bias + 64, results in 0 or -1
        neg     CL              ; get shift count
        mov     BL,CL           ; set shift count
        call    __U8RS          ; shift mantissa right 1 bit for signed
        pop     EBX             ; restore EBX
        ret                     ; return

fltuflow:
        sub     EAX,EAX         ; ensure entire number 0
fltzero:
        sub     EDX,EDX         ;
        ret                     ; return
        endproc __FSAbs

; Convert single precision float to unsigned 64-bit integer with rounding
; Input: [EAX] = 32-bit float

;       xdefp   __RSU8
;       defp    __RSU8
; not implemented
;       endproc __RSU8

; Convert single precision float to signed 64-bit integer with rounding
; Input: [EAX] = 32-bit float

;       xdefp   __RSI8
;       defp    __RSI8
; not implemented
;       endproc __RSI8

        endmod
        end
