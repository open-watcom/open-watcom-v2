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

        modstart  sqrt286

        xdefp   __sqrtd

;
;      double __sqrtd( double AX BX CX DX );
;
        defp    __sqrtd
        push    SI                      ; save SI
        push    DS                      ; save DS
        sub     SP,10                   ; allocate space for long double
        mov     SI,SS                   ; set DS=SS
        mov     DS,SI                   ; ...
        mov     SI,SP                   ; point DS:SI to long double
        call    __EmuFDLD               ; convert double to long double
        mov     AX,SP                   ; point to long double
        call    __sqrt                  ; calculate square root
        mov     BX,SP                   ; point to long double
        call    __EmuLDFD               ; convert it to double
        add     SP,10                   ; remove long double from stack
        pop     DS                      ; restore DS
        pop     SI                      ; restore SI
        ret                             ; return
        endproc __sqrtd

include e86sqrt.inc
include xception.inc
include fstatus.inc

        xref    F8InvalidOp
        xref    F8OverFlow

include e86ldfd.inc
include e86fdld.inc

        endmod
        end
