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
;* Description:  Convert MS binary float to IEEE format, 32-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        msfs386

                xdefp   "C",_fmsbintoieee
                defpe   _fmsbintoieee

ifdef __STACK__
                mov     eax,dword ptr +4H[esp]
                mov     edx,dword ptr +8H[esp]
endif

; At this point:
; eax     - ptr to MBF source float
; edx     - ptr to IEEE dest float

                mov     eax,[eax]       ; load MBF float
                                        ; MBF exponent = 0 ?
                test    eax,0ff000000h
                jne     MBFNonZero      ; yes, jump

                xor     eax,eax         ; IEEE 0.0
                mov     [edx],eax       ; store IEEE float
                ret                     ; return 0 (no overflow)

; At this point:
; eax     - MBF source float
; edx     - ptr to IEEE dest float

MBFNonZero:     rol     eax,9           ; rotate exponent for better analysis
                shr     eax,1           ; shift    out
                rcr     al,1            ; move sign bit
                adc     eax,eax         ; shift back
                sub     al,2            ; convert exponent
                jc      MBFExp1         ; jump if MBF exponent 1
                je      MBFExp2         ; jump if MBF exponent 2
                ror     eax,9           ; rotate so IEEE float
IEEEStore:
                mov     [edx],eax       ; store IEEE float
                xor     eax,eax         ; return 0 (no overflow)
                ret                     ; :

MBFExp1:        and     al,1            ; zero exponent except implied 1
                ror     eax,9           ; rotate so IEEE float
                sar     eax,2           ; convert to IEEE denormal
                adc     eax,0           ; add in round bit
                and     eax,80FFFFFFh   ; zero surplus sign bits
                jmp     short IEEEStore ; continue

MBFExp2:        or      al,1            ; set implied 1
                ror     eax,9           ; rotate so IEEE float
                sar     eax,1           ; convert to IEEE denormal
                adc     eax,0           ; add in round bit
                and     eax,80FFFFFFh   ; zero surplus sign bit
                jmp     short IEEEStore ; continue

                endproc _fmsbintoieee

                endmod
                end
