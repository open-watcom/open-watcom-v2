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


include mdef.inc
include struct.inc

                modstart        fdmd086

                xdefp   "C",_dieeetomsbin
                defpe   _dieeetomsbin

if _MODEL and _BIG_DATA
                push    ds          ; save register
                mov     ds,dx       ; load src float seg
                mov     dx,bx       ; dest float off
else
                push    bx          ; save register
endif

; At this point:
; ds:ax   - ptr to IEEE source double
; dx      - ptr to MBF dest double (off)
; cx      - ptr to MBF dest float (seg) (large data model only)
; bx      - spare register

; Check for IEEE Underflow first (including zero)
                mov     bx,ax       ; src float off
                mov     ax,+6H[bx]  ; load IEEE double (hi)
                rol     ax,1        ; rotate sign bit away for comparisons
                cmp     ax,6fe0H    ; exponent < 1023 - 128 ?
                                    ; yes, jump
                jae     short IEEENoUnderflow

; IEEE Underflow, store MBF 0.0
                xor     ax,ax       ; make 0
                mov     bx,dx       ; load dest float off
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     [bx],ax     ; store MBF 0.0 (lo)
                mov     +2H[bx],ax  ; store :
                mov     +4H[bx],ax  ; store :
                mov     +6H[bx],ax  ; store MBF 0.0 (hi)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return 0 (no overflow)

; Check for IEEE Overflow
IEEENoUnderflow:
                cmp     ax,8fc0H    ; exponent >= 1023 + 127 ?
                jae     short IEEEOverflow

; General IEEE case, load rest of double
                push    di          ; save registers
                push    si          ; :
                mov     di,[bx]     ; load IEEE double (lo)
                mov     si,+2H[bx]  ; load IEEE double :
                mov     bx,+4H[bx]  ; load IEEE double :
                xchg    dx,bx       ; swap dest double off, IEEE double (hi)
                ror     ax,1        ; rotate sign bit back into place
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif

; At this point:
; ax:dx:si:di   - IEEE double
; ds:bx         - ptr to MBF dest double
; cx            - spare register (large data model only)

if _MODEL and _BIG_DATA
                mov     cx,ax       ; save sign bit
else
                push    ax          ; save sign bit
endif

; shift exponent & mantissa into place
                add     di,di       ; shift exponent & mantissa
                adc     si,si       ; :
                adc     dx,dx       ; :
                adc     ax,ax       ; :

                add     di,di       ; shift exponent & mantissa
                adc     si,si       ; :
                adc     dx,dx       ; :
                adc     ax,ax       ; :

                add     di,di       ; shift exponent & mantissa
                adc     si,si       ; :
                adc     dx,dx       ; :
                adc     ax,ax       ; :

; store result
                mov     [bx],di     ; store MBF double (lo)
                mov     +2H[bx],si  ; store MBF double :
                mov     +4H[bx],dx  ; store MBF double :

; correct exponent and add sign-bit
                add     ax,ax       ; shift exponent into place
if _MODEL and _BIG_DATA
                add     ah,82H      ; convert chopped IEEE to MBF exponent
                add     ch,ch       ; shift sign bit into carry
else
                pop     dx          ; restore sign bit
                add     ah,82H      ; convert chopped IEEE to MBF exponent
                add     dh,dh       ; shift sign bit into carry
endif
                rcr     al,1        ; rotate sign bit into position
                mov     +6H[bx],ax  ; store MBF double (hi)
                xor     ax,ax       ; return 0 (no overflow)
                pop     si          ; clean up
                pop     di          ; :
if _MODEL and _BIG_DATA
                pop     ds          ; :
else
                pop     bx          ; :
endif
                ret                 ; return

; IEEE Overflow, store maximum MBF, preserving sign
IEEEOverflow:
                ror     al,1        ; rotate sign bit into place for MBF
                mov     bx,dx       ; load dest float off
                or      ax,0FF7Fh   ; set MBF exponent and mantissa to maximum
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     +6H[bx],ax  ; store MBF double (hi)
                mov     al,ah       ; mantissa = 0FFFFh
                mov     [bx],ax     ; store MBF double (lo)
                mov     +2H[bx],ax  ; store :
                mov     +4H[bx],ax  ; store :
                neg     ax          ; return 1 (overflow)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

                endproc _dieeetomsbin

                endmod
                end
