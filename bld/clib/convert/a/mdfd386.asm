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
;* Description:  Convert MS binary double to IEEE format, 32-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        mdfd386

                xdefp   "C",_dmsbintoieee
                defpe   _dmsbintoieee

ifdef __STACK__
                mov     eax,dword ptr +4H[esp]
                mov     edx,dword ptr +8H[esp]
else
                push    ecx
endif

; At this point:
; eax     - ptr to MBF source double
; edx     - ptr to IEEE dest double
; ecx     - spare register

; Check for and process MBF 0.0 first
                mov     ecx,+4h[eax]    ; load MBF double (hi)
                test    ecx,0ff000000h  ; MBF exponent = 0 ?
                jne     MBFNonZero      ; no, jump

; MBF 0.0, store IEEE 0.0
                xor     eax,eax         ; make 0
                mov     [edx],eax       ; store IEEE 0.0 (lo)
                mov     +4h[edx],eax    ; store IEEE 0.0 (hi)
ifndef __STACK__
                pop     ecx             ; clean up
endif
                ret                     ; return 0 (no overflow)

MBFNonZero:     mov     eax,[eax]       ; load MBF double (lo)

; At this point:
; ecx:eax - MBF source double
; edx     - ptr to IEEE dest double

                rol     ecx,9           ; rotate exponent & sign bit low
                shr     ecx,1           ; move sign bit before exponent
                rcr     cl,1            ; :
                adc     ecx,ecx         ; :
                ror     ecx,9           ; rotate exponent & sign bit back

; shift mantissa into place
                shrd    eax,ecx,2       ; shift mantissa
                sar     ecx,3           ; shift exponent & mantissa, save sign bit
                rcr     eax,1           ; shift mantissa
                jc      MBFRound        ; jump if rounding up
                and     ecx,8FFFFFFFh   ; mask out surplus sign bits
                add     ecx,37e00000h   ; convert MBF to IEEE exponent

IEEEStore:
                mov     [edx],eax       ; store IEEE double (lo)
                mov     +4h[edx],ecx    ; store IEEE double (hi)
                xor     eax,eax         ; 0
ifndef __STACK__
                pop     ecx             ; clean up
endif
                ret                     ; return 0 (no overflow)

; add rounding bit
MBFRound:       and     ecx,8FFFFFFFh   ; mask out surplus sign bits
                add     eax,1           ; add round bit
                adc     ecx,37e00000h   ; convert MBF to IEEE exponent
                jmp     IEEEStore       ; store result

                endproc _dmsbintoieee

                endmod
                end
