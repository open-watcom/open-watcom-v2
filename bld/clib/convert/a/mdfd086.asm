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
;* Description:  Convert MS binary double to IEEE format, 16-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        mdfd086

                xdefp   "C",_dmsbintoieee
                defpe   _dmsbintoieee

if _MODEL and _BIG_DATA
                push    ds          ; save register
                mov     ds,dx       ; load src float seg
                mov     dx,bx       ; dest float off
else
                push    bx          ; save register
endif

; At this point:
; ds:ax   - ptr to MBF source double
; dx      - ptr to IEEE dest double (off)
; cx      - ptr to IEEE dest float (seg) (large data model only)
; bx      - spare register

; Check for and process MBF 0.0 first
                mov     bx,ax       ; src float off
                mov     ax,+6H[bx]  ; load MBF double (hi)
                test    ah,ah       ; MBF exponent = 0 ?
                jne     MBFNonZero  ; no, jump

; MBF 0.0, store IEEE 0.0
                mov     al,ah       ; make 0
                mov     bx,dx       ; load dest float off
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     [bx],ax     ; store IEEE 0.0 (lo)
                mov     +2H[bx],ax  ; store :
                mov     +4H[bx],ax  ; store :
                mov     +6H[bx],ax  ; store IEEE 0.0 (hi)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret

; General MBF case, load rest of double
MBFNonZero:
                push    di          ; save registers
                push    si          ; :
                mov     di,[bx]     ; load MBF double (lo)
                mov     si,+2H[bx]  ; load MBF double :
                mov     bx,+4H[bx]  ; load MBF double :
                xchg    dx,bx       ; swap dest double off, MBF double
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif

; At this point:
; ax:dx:si:di   - MBF double
; ds:bx         - ptr to IEEE dest double
; cx            - spare register (large data model only)

                add     al,al       ; move sign bit before exponent
                rcr     ax,1        ; :

; shift mantissa into place
                sar     ax,1        ; shift exponent, save sign bit
                rcr     dx,1        ; 3rd mantissa shift
                rcr     si,1        ; 2nd mantissa shift
                rcr     di,1        ; 1st mantissa shift
                sar     ax,1        ; shift exponent, save sign bit
                rcr     dx,1        ; 3rd mantissa shift
                rcr     si,1        ; 2nd mantissa shift
                rcr     di,1        ; 1st mantissa shift
                sar     ax,1        ; shift exponent, save sign bit
                rcr     dx,1        ; 3rd mantissa shift
                rcr     si,1        ; 2nd mantissa shift
                rcr     di,1        ; 1st mantissa shift
                jc      MBFRound    ; jump if rounding up
                and     ah,8fH      ; mask out surplus sign bits
                add     ax,37e0H    ; convert MBF to IEEE exponent

; store result
IEEEStore:
                mov     [bx],di     ; store IEEE double (lo)
                mov     +2H[bx],si  ; store IEEE double :
                mov     +4H[bx],dx  ; store IEEE double :
                mov     +6H[bx],ax  ; store IEEE double (hi)
                xor     ax,ax       ; 0
                pop     si          ; clean up
                pop     di          ; :
if _MODEL and _BIG_DATA
                pop     ds          ; :
else
                pop     bx          ; :
endif
                ret                 ; return 0 (no overflow)

; add rounding bit
MBFRound:       and     ah,8fH      ; mask out surplus sign bits
                add     di,1        ; add round bit
                adc     si,0        ; :
                adc     dx,0        ; :
                adc     ax,37e0H    ; convert MBF to IEEE exponent
                jmp     IEEEStore   ; store result

                endproc _dmsbintoieee

                endmod
                end
