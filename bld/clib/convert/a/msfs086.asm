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
;* Description:  Convert MS binary float to IEEE format, 16-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        msfs086

                xdefp   "C",_fmsbintoieee
                defpe   _fmsbintoieee

if _MODEL and _BIG_DATA
                push    ds          ; save register
                mov     ds,dx       ; load src float seg
                mov     dx,bx       ; dest float off
else
                push    bx          ; save register
endif

; At this point:
; ds:ax   - ptr to MBF source float
; dx      - ptr to IEEE dest float (off)
; cx      - ptr to IEEE dest float (seg) (large data model only)
; bx      - spare register

; Check for and process MBF 0 first
                mov     bx,ax       ; src float off
                mov     ax,+2H[bx]  ; load MBF float (hi)
                test    ah,ah       ; MBF exponent != 0 ?
                jne     MBFNonZero  ; yes, jump

; MBF 0, store IEEE 0
                mov     al,ah       ; 0
                mov     bx,dx       ; load dest float off
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     [bx],ax     ; store IEEE 0 (lo)
                mov     +2H[bx],ax  ; store IEEE 0 (hi)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

; General MBF case
MBFNonZero:     mov     bx,[bx]     ; load MBF float (lo)
                xchg    dx,bx       ; swap dest float off, MBF float (lo)
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif

; At this point:
; ax:dx         - MBF float
; ds:bx         - ptr to IEEE dest float
; cx            - spare register (large data model only)

                sub     ah,2        ; convert to IEEE exponent
                jc      MBFExp1     ; jump if MBF exponent 1
                je      MBFExp2     ; jump if MBF exponent 2
                add     al,al       ; shift sign bit into carry
                rcr     ax,1        ; rotate sign, exponent and mantissa
IEEEStore:
                mov     [bx],dx     ; store IEEE float (lo)
                mov     +2H[bx],ax  ; store IEEE float (hi)
                xor     ax,ax       ; return 0 (no overflow)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

; convert to IEEE denormal
MBFExp1:
                add     al,al       ; shift sign bit into carry
                rcr     ax,1        ; rotate sign, exponent and mantissa
                shr     al,1        ; correct mantissa for denormal
                rcr     dx,1        ; :
                shr     al,1        ; :
                rcr     dx,1        ; :
                adc     dx,0        ; add in round bit
                adc     al,0        ; :
                and     ah,80h      ; make IEEE exponent 0
                jmp     short IEEEStore

; convert to IEEE denormal
MBFExp2:        add     al,al       ; shift sign bit into carry
                rcr     ax,1        ; rotate sign, exponent and mantissa
                or      al,80h      ; set implied 1 from mantissa for shift
                shr     al,1        ; correct mantissa for denormal
                rcr     dx,1        ; :
                adc     dx,0        ; add in round bit
                adc     al,0        ; :
                                    ; If a carry occurs here then it must be a
                                    ; maximum denormal. All mantissa bits were
                                    ; 1. It then rounds up to become the
                                    ; minimum IEEE can represent. The carry
                                    ; lands in the least exponent bit which is
                                    ; correct.
                jmp     short IEEEStore

                endproc _fmsbintoieee

                endmod
                end
