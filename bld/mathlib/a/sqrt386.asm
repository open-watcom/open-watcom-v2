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

        xref    __iFDLD
        xref    __iLDFD

        modstart  sqrt386

        xdefp   __sqrtd

;
;      double __sqrtd( double EDX EAX );
;

        defp    __sqrtd
        push    EDX                     ; make double parm addressable
        push    EAX                     ; ...
        mov     EAX,ESP                 ; get address of first parm
        sub     ESP,12                  ; allocate space for long double
        mov     EDX,ESP                 ; point EDX to long double
        call    __iFDLD                 ; convert double to long double
        mov     EAX,ESP                 ; point to long double
        call    __sqrt                  ; calculate square root
        mov     EAX,ESP                 ; point to long double
        mov     EDX,ESP                 ; get address of double
        add     EDX,12                  ; ...
        call    __iLDFD                 ; convert it to double
        add     ESP,12                  ; remove long double from stack
        pop     EAX                     ; pop double into return regs
        pop     EDX                     ; ...
        ret                             ; return
        endproc __sqrtd

include xception.inc
include fstatus.inc
include 386sqrt.inc

        endmod
        end
