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


;
; CPLXDIV       : complex divide
;

.8087

include struct.inc
include mdef.inc

        modstart        cplxdiv


; Complex division
;    (a,b)/(c,d) = ((a,b)*(c,-d))/(c**2+d**2)

        xdefp   CplxDivide
defp    CplxDivide
        fld     st(0)                   ; compute c*c + d*d
        fmul    st(0),st(0)             ; ...
        fld     st(2)                   ; ...
        fmul    st(0),st(0)             ; ...
        faddp   st(1),st(0)             ; ...
        fdiv    st(3),st(0)             ; compute a/(c*c+d*d)
        fdivp   st(4),st(0)             ; compute b/(c*c+d*d)
        fld     st(2)                   ; compute ad/(c*c+d*d)
        fmul    st(0),st(2)             ; ...
        fxch    st(4)                   ; ...
        fmul    st(2),st(0)             ; compute bd/(c*c+d*d)
        fmul    st(0),st(1)             ; compute bc/(c*c+d*d)
        fsubrp  st(4),st(0)             ; compute imaginary part
        fmulp   st(2),st(0)             ; compute ac/(c*c+d*d)
        faddp   st(1),st(0)             ; compute real part
        ret
endproc CplxDivide

        endmod
        end
