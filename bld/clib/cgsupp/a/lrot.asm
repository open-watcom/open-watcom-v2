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


;=========================================================================
;==     Name:           LROT (Long ROTate)                              ==
;==                     unsigned long _lrotl(unsigned long,int);        ==
;==                     unsigned long _lrotr(unsigned long,int);        ==
;==     Inputs:         DX:AX   long integer to be rotated              ==
;==                     BX      integer shift count                     ==
;==     Outputs:        DX:AX                                           ==
;==     Volatile:       BX destroyed                                    ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        lrot

        xdefp   _lrotl_
        xdefp   _lrotr_

        defp    _lrotr_                 ; rotate argument to the right
        neg     BL                      ; negate the count
;
;       fall into _lrotl_ to rotate to the left
;
        defp    _lrotl_                 ; rotate argument to the left
        push    CX                      ; save CX
        mov     CX,BX                   ; get the shift count
        and     CL,31                   ; calc shift count mod 32
        cmp     CL,16                   ; if shift count >= 16
        _if     ae                      ; then
          xchg  DX,AX                   ; - exchange high and low words
          sub   CL,16                   ; - adjust shift count
        _endif                          ; endif
;
;       CL < 16
;
        rol     DX,CL                   ; rotate high word
        rol     AX,CL                   ; rotate low word
        mov     BX,0FFFFh               ; calc mask of bits to keep
        shl     BX,CL                   ; ...
        mov     CX,AX                   ; save low word
        and     AX,BX                   ; mask off unwanted bits
        xor     CX,AX                   ; calc. bits to move to high word
        xor     AX,DX                   ; move bits from high word to low word
        and     DX,BX                   ; mask off unwanted bits
        xor     AX,DX                   ; fix up low word
        or      DX,CX                   ; merge bits from low word to high
        pop     CX                      ; restore CX
        ret                             ; return
        endproc _lrotl_
        endproc _lrotr_

        endmod
        end
