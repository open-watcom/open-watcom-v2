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

                modstart        fsms086

                xdefp   "C",_fieeetomsbin
                defpe   _fieeetomsbin

if _MODEL and _BIG_DATA
                push    ds          ; save register
                mov     ds,dx       ; load src float seg
                mov     dx,bx       ; dest float off
else
                push    bx          ; save register
endif

; At this point:
; ds:ax   - ptr to IEEE source float
; dx      - ptr to MBF dest float (off)
; cx      - ptr to MBF dest float (seg) (large data model only)
; bx      - spare register

; Check for and process IEEE 0 first
                mov     bx,ax       ; src float off
                mov     ax,+2H[bx]  ; load MBF float (hi)
                test    ax,7fe0h    ; IEEE exponent non zero or convertable
                                    ; denormal ?
                jne     IEEENonZero ; yes, jump

; IEEE 0 or IEEE unconvertable denormal, store MBF 0
                xor     ax,ax       ;
                mov     bx,dx       ; load dest float off
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     [bx],ax     ; store MBF 0 (lo)
                mov     +2H[bx],ax  ; store MBF 0 (hi)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

; At this point:
; ax            - IEEE source float (hi word)
; ds:bx         - ptr to IEEE source float
; dx            - ptr to MBF dest float (off)
; cx            - ptr to MBF dest float (seg) (large data model only)

IEEENonZero:    test    ax,7f80h    ; IEEE convertable denormal ?
                je      IEEEDenormal; yes, jump

                add     ax,ax       ; shift sign bit into carry
                rcr     al,1        ; rotate carry flag and mantissa into place
                add     ah,2        ; correct exponent
                jc      IEEEOverflow; jump if overflow
                mov     bx,[bx]     ; load IEEE float (lo)
                xchg    dx,bx       ; swap dest float off, IEEE float (lo)

MBFStore:
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     [bx],dx     ; store MBF float (lo)
                mov     +2H[bx],ax  ; store MBF float (hi)
                xor     ax,ax       ; return 0 (no overflow)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

IEEEDenormal:   mov     bx,[bx]     ; load IEEE float (lo)
                xchg    dx,bx       ; swap dest float off, IEEE float (lo)

; while implied 1 bit not seen increment count
DenormalLoop:
                inc     ah          ; count
                add     dx,dx       ; shift mantissa
                adc     al,al       ; :
                jno     DenormalLoop; loop while no implied 1
                add     ax,ax       ; shift sign bit into carry
                rcr     al,1        ; rotate carry flag and mantissa into place
                shr     ah,1        ; shift out implied 1
                xor     ah,3        ; exponent = 3 - count
                jmp     short MBFStore

IEEEOverflow:
if _MODEL and _BIG_DATA
                mov     ds,cx       ; load dest float seg
endif
                mov     bx,dx       ; load dest float off
                or      ax,0FF7Fh   ; set MBF exponent and mantissa to maximum
                                    ; but preserve MBF sign
                mov     +2H[bx],ax  ; store MBF float (hi)
                mov     al,ah       ; mantissa = 0FFFFh
                mov     [bx],ax     ; store MBF float (lo)
                neg     ax          ; return 1 (overflow)

if _MODEL and _BIG_DATA
                pop     ds          ; clean up
else
                pop     bx          ; clean up
endif
                ret                 ; return

                endproc _fieeetomsbin

                endmod
                end
