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
;* Description:  Convert IEEE double to MS binary format, 32-bit variant.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

                modstart        fdmd386

                xdefp   "C",_dieeetomsbin
                defpe   _dieeetomsbin

ifdef __STACK__
                mov     eax,dword ptr +4H[esp]  ; load src double ptr
                mov     edx,dword ptr +8H[esp]  ; load dst double ptt
else
                push    ecx             ; save register
endif

; At this point:
; eax     - ptr to IEEE source double
; edx     - ptr to MBF dest double
; ecx     - spare register

; Check for IEEE Underflow first
                mov     ecx,+4h[eax]    ; load IEEE double (hi)
                rol     ecx,1           ; rotate sign bit away for comparisons
                cmp     ecx,6fe00000H   ; exponent < 1023 - 128 ?
                jae     IEEENoUnderflow ; yes, jump

; IEEE Underflow, store MBF 0.0
                xor     eax,eax         ; make 0
                mov     [edx],eax       ; store MBF 0.0 (lo)
                mov     +4h[edx],eax    ; store MBF 0.0 (hi)
ifndef __STACK__
                pop     ecx             ; clean up
endif
                ret                     ; return 0 (no overflow)

; Check for IEEE Overflow
IEEENoUnderflow:
                cmp     ecx,8fc00000H   ; exponent >= 1023 + 127 ?
                jae     IEEEOverflow    ; yes, jump

; General IEEE case, load rest of double
                mov     eax,[eax]       ; load IEEE double (lo)
                ror     ecx,1           ; rotate sign bit back into place

; At this point:
; ecx:eax - IEEE source double
; edx     - ptr to MBF dest double

                push    ecx             ; save sign bit

; shift exponent & mantissa into place
                shld    ecx,eax,3       ; shift exponent and mantissa
                shl     eax,3           ; :
                mov     [edx],eax       ; save mantissa
                rol     ecx,9           ; convert IEEE exponent to MBF
                shr     ecx,1           ; :
                adc     cl,cl           ; :
                add     cl,82h          ; correct MBF exponent
                pop     eax             ; restore sign bit
                add     eax,eax         ; shift sign bit into carry
                adc     ecx,ecx         ; add in sign bit
                ror     ecx,9           ; MBF double (hi)
                mov     +4h[edx],ecx    ; store MBF double (hi)
                xor     eax,eax         ; 0
ifndef __STACK__
                pop     ecx             ; clean up
endif
                ret                     ; return 0 (no overflow)

; IEEE Overflow, store maximum MBF, preserving sign
IEEEOverflow:   or      ecx,0FFFFFFFFEh ; set MBF exponent and mantissa to maximum
                mov     eax,ecx         ; :
                ror     ecx,9           ; rotate sign bit into place for MBF
                sar     eax,1           ; now -1
                mov     +4h[edx],ecx    ; store IEEE double (hi)
                mov     [edx],eax       ; store IEEE double (lo)
                neg     eax             ; 1
ifndef __STACK__
                pop     ecx             ; clean up
endif
                ret                     ; return 1 (overflow)

                endproc _dieeetomsbin

                endmod
                end
