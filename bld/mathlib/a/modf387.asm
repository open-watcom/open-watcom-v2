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
include math387.inc

        xref            __CHP
        xref            __8087  ; indicate that NDP instructions are present

        modstart        modf387

    if __WASM__ ge 100
        xdefp   "C",modf        ; double modf( double x, double *iptr )
    else
        xdefp   <"C",modf>      ; double modf( double x, double *iptr )
    endif

;  input:       x - on the stack
;               iptr on the stack
;  output:      signed fractional part of x in EDX:EAX
;               *iptr gets the integer part of x
;
        defp    modf
ifdef __386__
        push    EAX             ; save EAX
        fld     qword ptr 8[ESP]; load x
        fld     st(0)           ; duplicate it
        call    __CHP           ; calculate integer part
        fsub    st(1),st(0)     ; subtract integer part from x
        mov     EAX,16[ESP]     ; get iptr
        fstp    qword ptr [EAX] ; store integer part
        pop     EAX             ; restore EAX
        loadres                 ; load result
        fwait                   ; wait
        ret_pop 12              ; return
else
        if _MODEL and _BIG_CODE
         argx    equ     6
        else
         argx    equ     4
        endif
        iptr    equ     argx+8

        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        push    BX              ; save BX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        lds     BX,iptr[BP]     ; get segment and offset of iptr
        popamt  equ     8+4
else
        mov     BX,iptr[BP]     ; get iptr
        popamt  equ     8+2
endif
        fld     qword ptr argx[BP]; load argument
        fld     st(0)           ; dup x
        docall  __CHP           ; calculate integer part
        fsub    st(1),st(0)     ; subtract integer part from x
        fstp    qword ptr [bx]  ; store integer part
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        fwait                   ; wait
        pop     BP              ; restore BP
        ret     popamt          ; return
endif
        endproc modf

        endmod
        end
