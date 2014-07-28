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


ifdef __386__
 .387
else
 .8087
endif
include mdef.inc
include struct.inc
include math87.inc

        xrefp           __8087  ; indicate that NDP instructions are present

        modstart        asin87

        xrefp           IF@DATAN2
        xrefp           __@DSQRT

        xdefp   "C",asin        ; double asin( double x )
;
;       asin(x) = atan( x / sqrt( 1 - x*x ) );
;

        public  IF@DASIN        ; double asin( double x )
        public  IF@ASIN         ; double asin( double x )
        defp    IF@DASIN
        defp    IF@ASIN
ifndef __386__
        prolog
endif
        fld     st(0)           ; duplicate x
        fmul    st(0),st        ; calc. x * x
        fld1                    ; 1.0
        fsubrp  st(1),st        ; 1.0 - x * x
        mov     AL,FP_FUNC_ASIN ; indicate ASIN function
        call    __@DSQRT        ; calculate sqrt( 1.0 - x * x )
        cmp     AL,0            ; if no error
        _if     e               ; then
          fxch    st(1)         ; - flip arguments around
;;;        fpatan               ; - atan( x / sqrt( 1.0 - x * x ) )
          call    IF@DATAN2     ; - atan2( x, sqrt( 1.0 - x * x ) );
        _endif                  ; endif
ifndef __386__
        epilog
endif
        ret                     ; return
        endproc IF@ASIN
        endproc IF@DASIN


;  input:       x - on the stack
;  output:      arcsin of x in st(0)
;
        defp    asin
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DASIN        ; calculate asin(x)
        loadres                 ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        prolog
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DASIN        ; calculate asin(x)
        epilog
endif
        ret_pop 8               ; return
        endproc asin

        endmod
        end
