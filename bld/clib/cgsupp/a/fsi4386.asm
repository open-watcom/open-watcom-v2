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


;========================================================================
;==     Name:           FSI4, FSU4, FSI2, FSU2, FSI1, FSU1             ==
;==     Operation:      Convert single precision to integer            ==
;==     Inputs:         EAX     single precision floating point        ==
;==     Outputs:        EAX            integer value                   ==
;==     Volatile:       none                                           ==
;==                                                                    ==
;==                                                                    ==
;==                                     handle -1.0 -> 0xffffffff      ==
;========================================================================
include mdef.inc
include struct.inc

        modstart        fsi4386

        xdefp   __FSI4
        xdefp   __FSU4
        xdefp   __FSI2
        xdefp   __FSU2
        xdefp   __FSI1
        xdefp   __FSU1

        defpe   __FSI4
        push    ECX             ; save cx
        mov     cl,7fh+31       ; maximum number 2^31-1
        call    __FSI           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return (overflow already handled
        endproc __FSI4

        defpe   __FSU4
        push    ECX             ; save cx
        mov     cl,7fh+32       ; maximum number 2^32-1
        call    __FSU           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return if no overflow
        endproc __FSU4

        defpe   __FSI2
        push    ECX             ; save cx
        mov     cl,7fh+15       ; maximum number 2^15-1
        call    __FSI           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return (overflow already handled
        endproc __FSI2

        defpe   __FSU2
        push    ECX             ; save cx
        mov     cl,7fh+16       ; maximum number 2^16-1
        call    __FSU           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return if no overflow
        endproc __FSU2

        defpe   __FSI1
        push    ECX             ; save cx
        mov     cl,7fh+7        ; maximum number 2^7-1
        call    __FSI           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return (overflow already handled
        endproc __FSI1


        defpe   __FSU1
        push    ECX             ; save cx
        mov     cl,7fh+8        ; maximum number 2^8-1
        call    __FSU           ; convert to integer
        pop     ECX             ; restore cx
        ret                     ; return if no overflow
        endproc __FSU1

__FSI   proc    near
__FSU:
        or      EAX,EAX         ; check sign bit
        jns     short __FSAbs   ; treat as unsigned if positive
        call    __FSAbs         ; otherwise convert number
        neg     EAX             ; negate the result
        ret                     ; and return
        endproc __FSI

; 18-nov-87 AFS (for WATCOM C)
;__FSU  proc near
;       jmp
;       or      dx,dx           ; check sign bit
;       jns     __FSAbs         ; just convert if positive
;       sub     ax,ax           ; return 0 if negative
;       sub     dx,dx           ; ...
;       ret
;       endproc __FSU

;========================================================================
;==     Name:           FSAbs_                                         ==
;==     Inputs:         EAX   float                                    ==
;==                     CL    maximum exponent excess $7f              ==
;==     Outputs:        EAX   integer, absolute value of float         ==
;==                           if exponent >= maximum then 2^max - 1    ==
;==                             returned                               ==
;========================================================================

__FSAbs proc near
        or      EAX,EAX         ; check if number 0
        je      short retzero   ; if so, just return it
        _shl    EAX,1           ; shift mantissa over
        rol     EAX,8           ; get exponent to bottom
        cmp     AL,7fh          ; quit if number < 1.0          15-apr-91
        jb      short uflow     ; ...
        mov     CH,AL           ; save exponent
        stc                     ; set carry for implied bit
        rcr     EAX,1           ; put implied '1' bit in
        shr     EAX,8           ; remove exponent
        cmp     CH,CL           ; check if exponent exceeds maximum
        jae     short retmax    ; return maximum value if so
        sub     CH,7fh+23       ; calculate amount to shift (+ve -> left)
        jae     short m_left    ; jump if left shift/no shift
        xchg    CH,CL           ; get shift count
        neg     CL              ; make positive
        shr     EAX,CL          ; shift mantissa
        ret                     ; return with number

m_left:
        _if     ne              ; done if exponent exactly 23
          mov     CL,CH         ; - get shift count
          shl     EAX,CL        ; - shift number left
        _endif                  ; endif
        ret                     ; return

retmax: mov     EAX,0FFFFFFFFh  ; return maximum value
        sub     CL,7Fh+32       ; subtract bias + 32
        neg     CL              ; get shift count
        shr     EAX,CL          ; compute value
        ret                     ; return

uflow:
retzero:sub     EAX,EAX         ; ensure entire number 0
        ret                     ; return
        endproc __FSAbs

        endmod
        end
