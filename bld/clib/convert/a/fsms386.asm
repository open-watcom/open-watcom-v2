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
;* Description:  Convert IEEE float to MS binary format, 32-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        fsms386

                xdefp   "C",_fieeetomsbin
                defpe   _fieeetomsbin

ifdef __STACK__
                mov     eax,dword ptr +4H[esp]
                mov     edx,dword ptr +8H[esp]
endif

; At this point:
; eax     - ptr to IEEE source float
; edx     - ptr to MBF dest float

                mov     eax,[eax]       ; load IEEE float
                test    eax,7fe00000h   ; IEEE exponent != 0 or convertable
                                        ; denormal ?
                jne     IEEENonZero     ; :

; IEEE Zero or IEEE unconvertable denormal, store MBF Zero
                xor     eax,eax         ; 0
                mov     [edx],eax       ; store MBF 0.0F
                ret                     ; return 0 (no overflow)

; At this point:
; eax     - IEEE source float
; edx     - ptr to MBF dest float

IEEENonZero:    rol     eax,9           ; rotate for exponent analysis
                test    al,al           ; IEEE convertable denormal ?
                je      IEEEDenormal    ; yes, jump
                add     al,2            ; MBF exponent = IEEE exponent + 2
                jc      IEEEOverflow    ; jump if overflow
                shr     eax,1           ; rotate sign bit and exponent
                adc     al,al           ; :
                adc     eax,eax         ; :
                ror     eax,9           ; rotate so MBF float

MBFStore:       mov     [edx],eax       ; store MBF float
                xor     eax,eax         ; 0
                ret                     ; return 0 (no overflow)

; One of leading 2 bits of mantissa is a 1
IEEEDenormal:
ifndef __STACK__
                push    ecx             ; save register
endif
                mov     ecx,eax         ; save sign bit and exponent
                and     ah,0FEh         ; eliminate sign bit
DenormalLoop:   inc     ecx             ; increment count
                add     eax,eax         ; shift mantissa
                jnc     DenormalLoop    ; loop while no implied 1
                xor     ecx,3h          ; invert count (new exponent)
                shr     ecx,1           ; rotate exponent and sign bit
                adc     cl,cl           ; :
                adc     ecx,ecx         ; :
                shrd    eax,ecx,9       ; combine mantissa (eax) and
                                        ; exponent& sign bit (ecx)
ifndef __STACK__
                pop     ecx             ; restore register
endif
                jmp     MBFStore        ; continue

IEEEOverflow:   rol     eax,15          ; rotate sign bit into place
                or      eax,0FF7FFFFFh  ; set MBF exponent and mantissa to
                                        ; maximum but preserve MBF sign
                mov     [edx],eax       ; store MBF float
                and     eax,1           ; 1
                ret                     ; return 1 (overflow)

                endproc _fieeetomsbin

                endmod
                end
