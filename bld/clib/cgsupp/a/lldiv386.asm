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
;* Description:  Signed 64-bit division for 386.
;*
;*****************************************************************************


;========================================================================
;==     Name:           LLDIV                                          ==
;==     Operation:      Signed 8 byte divide                           ==
;==     Inputs:         EAX;EDX Dividend                               ==
;==                     ECX;EBX Divisor                                ==
;==                     SS:ESI  pointer to result structure            ==
;==     Volatile:       none                                           ==
;========================================================================

include mdef.inc
include struct.inc

ifdef _PROFILE
include p5prof.inc
endif
        modstart        lldiv386

        xref    __I8D

;        defpe   imaxdiv
;        xdefp   "C",imaxdiv
        defpe   lldiv
        xdefp   "C",lldiv
    ifdef _PROFILE
        P5Prolog
    endif
    ifdef __STACK__
        push    EBX             ; save EBX
        mov     EAX,08[ESP]     ; get numerator
        mov     EDX,12[ESP]
        mov     EBX,16[ESP]     ; get denominator
        mov     ECX,20[ESP]
    endif
        docall  __I8D           ; call long division routine

        mov     [ESI],EAX       ; store quotient
        mov     4[ESI],EDX
        mov     8[ESI],EBX      ; store remainder
        mov     12[ESI],ECX
    ifdef __STACK__
        pop     EBX             ; restore EBX
    endif
    ifdef _PROFILE
        P5Epilog
    endif
        ret                     ; return
        endproc lldiv
;        endproc imaxdiv

        endmod
        end
