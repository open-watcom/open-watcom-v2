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
;                               from double to signed/unsigned int64
;
include mdef.inc
include struct.inc

        modstart        fdi8386

        xref    __U8LS
        xref    __U8RS

; Convert double precision float to unsigned 64-bit integer with truncation
; Input: [EDX, EAX] = 64-bit float
; Output: [EDX, EAX] = 64-bit integer

        xdefp   __FDU8
        defp    __FDU8
        push    ECX             ; save ECX
        mov     CX,3ffh+64      ; maximum number 2^64-1
        call    __FDU           ; convert float to unsigned __int64
        pop     ECX             ; restore ECX
        ret                     ; return
        endproc __FDU8

; Convert double precision float to signed 64-bit integer with truncation
; Input: [EDX, EAX] = 64-bit float
; Output: [EDX, EAX] = 64-bit integer

        xdefp   __FDI8
        defp    __FDI8
        push    ECX             ; save ECX
        mov     CX,3ffh+63      ; maximum number 2^63-1
        call    __FDI           ; convert float to signed __int64
        pop     ECX             ; restore ECX
        ret                     ; return
        endproc __FDI8

__FDI   proc    near
__FDU:                          ; what? they both do the same thing
        or      EDX,EDX         ; check sign bit
        jns     short __FDAbs   ; treat as unsigned if positive
        call    __FDAbs         ; otherwise convert number
        neg     EDX             ; negate the result
        neg     EAX             ;
        sbb     EDX,0           ;
        ret                     ; return
        endproc __FDI

__FDAbs proc near
        or      EAX,EAX         ; check if number 0
        jne     short notzero   ;
        or      EDX,EDX         ; check if number 0
        jne     short notzero   ;
        ret
notzero:
        push    EBX             ; save EBX
        _shl    EDX,1           ; shift mantissa over
        rol     EDX,11          ; get exponent to bottom
        mov     BX,DX           ; copy and isolate
        and     BX,07ffh        ; exponent
        cmp     BX,3ffh         ; quit if number < 1.0
        jb      short dbluflow  ; ...
        stc                     ; set carry for implied bit
        rcr     EDX,1           ; put implied '1' bit in
        shr     EDX,11          ; remove exponent and extra bit
        cmp     BX,CX           ; check if exponent exceeds maximum
        jae     short dblmax    ; return maximum value if so
        sub     BX,3ffh+52      ; calculate amount to shift (+ve -> left)
        jae     short dblm_left ; jump if left shift/no shift
        neg     BX              ; make positive
        call    __U8RS          ; shift mantissa right
        pop     EBX             ; restore EBX
        ret                     ; return

dblm_left:
        _if     ne              ; done if exponent exactly 55
          call    __U8LS        ; - shift mantissa left
        _endif                  ; endif
        pop     EBX             ; restore EBX
        ret                     ; return

; CX = 3ffh+64  for unsigned
;      3ffh+63  for signed
dblmax:
        mov     EAX,0FFFFFFFFh  ; return maximum value
        mov     EDX,EAX         ;
        sub     CX,3fFh+64      ; subtract bias + 64, results in 0 or -1
        neg     CX              ; get shift count
        mov     BX,CX           ; set shift count
        call    __U8RS          ; shift mantissa right 1 bit for signed
        pop     EBX             ; restore EBX
        ret                     ; return

dbluflow:
        sub     EAX,EAX         ; ensure entire number 0
        sub     EDX,EDX         ;
        pop     EBX             ; restore EBX
        ret                     ; return

        endproc __FDAbs

; Convert double precision float to unsigned 64-bit integer with rounding
; Input: [EDX, EAX] = 64-bit float

;       xdefp   __RDU8
;       defp    __RDU8
; not implemented
;       endproc __RDU8

; Convert double precision float to signed 64-bit integer with rounding
; Input: [EDX, EAX] = 64-bit float

;       xdefp   __RDI8
;       defp    __RDI8
; not implemented
;       endproc __RDI8

        endmod
        end
