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

        extern_chipbug
        xrefp           __8087  ; indicate that NDP instructions are present

        modstart        fmod87

        xdefp   "C",fmod        ; calc fmod(x,y)

ifndef __386__
if _MODEL and _BIG_CODE
argx    equ     6
else
argx    equ     4
endif
endif

        public  IF@DFMOD                ; double fmod( double x, double y )
        public  IF@FMOD                 ; double fmod( double x, double y )
        defp    IF@DFMOD
        defp    IF@FMOD
ifndef __386__
        push    BP                      ; save BP
        mov     BP,SP                   ; get access to stack
endif
        _loop                           ; loop
          do_fprem                      ; - calculate remainder
ifdef __386__
          fstsw AX                      ; - get status
else
          push  AX                      ; - allocate space for status
          fstsw word ptr -2[BP]         ; - get status
          fwait                         ; - wait
          pop   AX                      ; - ...
endif
          sahf                          ; - set flags
        _until  np                      ; until modulus calculated
        fstp    st(1)                   ; copy answer up
ifndef __386__
        pop     BP                      ; restore BP
endif
        ret                             ; return
        endproc IF@FMOD
        endproc IF@DFMOD
;
;       double fmod( double x, double y )
;
        defp    fmod
ifdef __386__
        fld     qword ptr 4+8[ESP]      ; load argument y
        fld     qword ptr 4[ESP]        ; load argument x
        call    IF@DFMOD                ; calculate fmod(x,y)
        loadres                         ; load result
else
        push    BP                      ; save BP
        mov     BP,SP                   ; get access to stack
        fld     qword ptr argx+8[BP]    ; load y
        fld     qword ptr argx[BP]      ; load x
        lcall   IF@DFMOD                ; calculate fmod(x,y)
        pop     BP                      ; restore BP
endif
        ret_pop 16                      ; return
        endproc fmod


        endmod
        end
