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

        xref            __8087  ; indicate that NDP instructions are present

        modstart        acos87

        xref            IF@DATAN2
        xref            __@DSQRT

        xdefp           "C",acos        ; double acos( double x )

ifdef __386__
    datasegment
endif
piby2   dt      1.57079632679489661923
ifdef __386__
    enddata
endif

;
;       acos(x) = pi/2 - atan( x / sqrt( 1 - x*x ) );
;

        public  IF@DACOS        ; double acos( double x )
        public  IF@ACOS         ; double acos( double x )
        defp    IF@DACOS
        defp    IF@ACOS
        push    ax
ifndef __386__
        prolog
        sub     sp,2            ; for status word
endif
        fld     st(0)           ; duplicate x
        fmul    st(0),st        ; calc. x * x
        fld1                    ; 1.0
        fsubrp  st(1),st        ; 1.0 - x * x
        ftst                    ; compare with 0.0
ifdef __386__
        fstsw   ax              ;
else
        fstsw   word ptr -2[bp]
        fwait
        mov     ax,word ptr -2[bp]
endif
        sahf                    ; if (1.0 - x * x) == 0.0
        _if     e               ; then
          fcompp                ; - compare x with 0.0 (still on stack)
ifdef __386__
          fstsw ax              ;
else
          fstsw word ptr -2[bp]
          fwait
          mov   ax,word ptr -2[bp]
endif
          sahf                  ; - if x > 0
          _if   be              ; - then
            fldz                ; - - result is 0.0
          _else                 ; - else
            fldpi               ; - - result is pi
          _endif                ; - endif
        _else                   ; else
          mov   AL,FUNC_ACOS    ; - indicate ACOS function
          call  __@DSQRT        ; - calculate sqrt( 1.0 - x * x )
          cmp   AL,0            ; - if no error
          _if   e               ; - then
            fxch st(1)          ; - - flip arguments
;;;;        fpatan              ; - - atan( x / sqrt( 1.0 - x * x ) )
            call  IF@DATAN2     ; - - atan2( x, sqrt( 1.0 - x * x ) );
            fld tbyte ptr piby2 ; - - load pi/2
            fsubrp  st(1),st(0) ; - - compute arcos(x)=pi/2-arcsin(x)
          _endif                ; - endif
        _endif                  ; endif
ifndef __386__
        mov     sp,bp
        epilog
endif
        pop     ax
        ret                     ; return
        endproc IF@ACOS
        endproc IF@DACOS

;  input:       x - on the stack
;  output:      arccos of x in st(0)
;
        defp    acos
ifdef __386__
        fld     qword ptr 4[ESP]; load argument x
        call    IF@DACOS        ; calculate acos(x)
        loadres                 ; load result
else
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
        prolog
        fld     qword ptr argx[BP]; load argument x
        lcall   IF@DACOS        ; calculate acos(x)
        epilog
endif
        ret_pop 8               ; return
        endproc acos

        endmod
        end
