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
 include mdef.inc
 .387
else
 include mdef.inc
 .8087
endif
include struct.inc
include math387.inc

        xref            IF@DCOS
        xref            IF@DSIN
        xref            IF@DTAN

        modstart        trigl87

    if __WASM__ ge 100
        xdefp   "C",_cosl       ; long double _cosl( long double x )
        xdefp   "C",_sinl       ; long double _sinl( long double x )
        xdefp   "C",_tanl       ; long double _tanl( long double x )
    else
        xdefp   <"C",_cosl>     ; long double _cosl( long double x )
        xdefp   <"C",_sinl>     ; long double _sinl( long double x )
        xdefp   <"C",_tanl>     ; long double _tanl( long double x )
    endif

;  input:       x - on the stack
;  output:      cos of x in st(0)
;
        defp    _cosl
ifdef __386__
        fld     tbyte ptr 4[ESP]; load argument x
        call    IF@DCOS         ; calculate cos(x)
        loadres                 ; load result
else
 if _MODEL and _BIG_CODE
  argx  equ     6
 else
  argx  equ     4
 endif
        prolog
        fld     tbyte ptr argx[BP]; load argument x
        call    IF@DCOS         ; calculate cos(x)
        epilog
endif
        ret_pop _POP_LD_        ; return
        endproc _cosl

;  input:       x - on the stack
;  output:      sin of x in st(0)
;
        defp    _sinl
ifdef __386__
        fld     tbyte ptr 4[ESP]; load argument x
        call    IF@DSIN         ; calculate sin(x)
        loadres                 ; load result
else
        prolog
        fld     tbyte ptr argx[BP]; load argument x
        call    IF@DSIN         ; calculate sin(x)
        epilog
endif
        ret_pop _POP_LD_        ; return
        endproc _sinl

;  input:       x - on the stack
;  output:      tan of x in st(0)
;
        defp    _tanl
ifdef __386__
        fld     tbyte ptr 4[ESP]; load argument x
        call    IF@DTAN         ; calculate tan(x)
        loadres                 ; load result
else
        prolog
        fld     tbyte ptr argx[BP]; load argument x
        call    IF@DTAN         ; calculate tan(x)
        epilog
endif
        ret_pop _POP_LD_        ; return
        endproc _tanl

        endmod
        end
