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

        modstart        exp87

.8087

        xdefp   __exp87         ; calc exp(fac1)


        defp    __exp87
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; push argument
        push    BX              ; . . .
        push    CX              ; . . .
        push    DX              ; . . .
        fld     qword ptr -8[BP]; load argument
        call    _exp            ; calulate e to the x
        fstp    qword ptr -8[BP]; store result
        fwait                   ; wait
        pop     DX              ; load result
        pop     CX              ; . . .
        pop     BX              ; . . .
        pop     AX              ; . . .
        pop     BP              ; restore BP
        ret                     ; return
        endproc __exp87


;       calculate exp(st(0)) using 8087
_exp    proc    near
        fldl2e                  ; load log2(e)
        fmulp   st(1),st(0)     ; calculate argument * log2(e)

;;      xdef    x2y8087         ; calculate x ** y using 8087
x2y8087:                        ; compute x to the y
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
; + 20-oct-86  DJP
; if exponent ( y ) < -32767 return zero. Done so fist won't overflow.
        mov     AX,8001h        ; load -32767 in fac1
        push    AX              ; . . .
        fild    word ptr -2[BP] ; . . .
        fcom    st(1)           ; compare -32767 to exponent
        fstsw   -2[BP]          ; get result of compare
        fwait                   ; . . .
        pop     AX              ; . . .
        sahf                    ; load flags
        fstp    st(0)           ; clean stack
        _if     a               ; if -32767 greater than exponent
          fstp  st(0)           ; - clean stack
          fldz                  ; - return zero
          pop   BP              ; - restore BP
          ret                   ; - return
        _endif                  ; endif
;- 20-oct-86  DJP
        fld     st(0)           ; duplicate fac1
        call    __floor87       ; get integer part of fac1
;+ 19-mar-86 FWC
;       this code is here to cause an overflow interrupt if the argument
;       to fscale is out of range.
        push    AX              ; allocate word on stack
        fist    word ptr -2[BP] ; store argument as 16-bit integer
        fwait                   ; wait
        pop     AX              ; clean up stack
        cmp     AX,8000h        ; if argument out of range
        _if     e               ; then
          dec   AX              ; - set to 32767
          push  AX              ; - push it
          fstp  st(0)           ; - throw away argument
          fstp  st(0)           ; - again
          fild  word ptr -2[BP] ; - load 32767
          fld   st(0)           ; - duplicate it
          pop   AX              ; - clean up stack
        _endif                  ; - endif
;- 19-mar-86 FWC
        fld     st(0)           ; duplicate it
        fld1                    ; load 1.0
        fscale                  ; calc. 2 ** integer part(fac1)
        fstp    st(1)           ; copy 2**integer up
        fxch    st(1)           ; st(0) := floor(arg), st(1) := 2**ip(arg)
        fld     st(2)           ; get copy of argument
        fsubrp  st(1),st(0)     ; fraction := arg - floor(arg)
        ftst                    ; test fractional part
        push    AX              ; allocate space for result
        fstsw   -2[BP]          ; get condition
        fwait                   ; wait
        pop     AX              ; . . .
        sahf                    ; set flags
        _if     ne              ; if fraction <> 0
          fld1                  ; - load -1
          fchs                  ; - . . .
          fxch  st(1)           ; - exchange fraction and -1
          fscale                ; - divide fraction by 2
          fstp  st(1)           ; - move fraction up over -1
          f2xm1                 ; - calculate x ** fraction - 1
          fld1                  ; - load 1
          faddp st(1),st(0)     ; - add 1 to result
          fmul  st(0),st(0)     ; - square the result
          fmulp st(1),st(0)     ; - 2**fraction * 2**integer
        _else                   ; else
          fstp  st(0)           ; - get rid of fraction
        _endif                  ; endif
        fstp    st(1)           ; copy answer up into fac1
        pop     BP              ; restore BP
        ret                     ; return
        endproc _exp


__floor87 proc  near
        push    BP              ; save BP
        mov     BP,SP           ; get address of storage space
        push    AX              ; allocate space
        fstcw   -2[BP]          ; get current control word
        fwait                   ; wait for it
        mov     AX,-2[BP]       ; get control word
        and     AX,0F3FFh       ; mask out old rounding-control bits
        or      AX,0400h        ; set round down towards -ve infinity
        push    AX              ; . . .
        fldcw   -4[BP]          ; load new control word
        frndint                 ; round st(0) to an integer
        fldcw   -2[BP]          ; set control word back
        fwait                   ; wait
        mov     SP,BP           ; clean up the stack
        pop     BP              ; restore BP
        ret                     ; return
        endproc __floor87


        endmod
        end
