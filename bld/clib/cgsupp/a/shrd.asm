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
;==     Name:           SHRD                                            ==
;==     Operation:      shift right double (long)                       ==
;==     Inputs:         DX:AX   long integer to be shifted              ==
;==                     CX      integer shift count                     ==
;==     Outputs:        DX:AX   DX:AX >> CX                             ==
;==     Volatile:       CX destroyed                                    ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        shrd

        xdefp   __SHRD

        defp    __SHRD
        cmp     CL,16                   ; if shift count >= 16
        _if     ae                      ; then
          mov   AX,DX                   ; - shift right by 16
          sub   CL,16                   ; - adjust shift count
          sub   DX,DX                   ; - zero high word
          shr   AX,CL                   ; - shift the low word by (count-16)
          ret                           ; - return
        _endif                          ; endif
;
;       CL < 16
;
        push    SI                      ; save work reg
        shr     AX,CL                   ; shift bottom part
        ror     DX,CL                   ; rotate top part
        xor     AX,DX                   ; merge top bits of DX into AX
        mov     SI,0FFFFh               ; calc mask of bits to keep
        shr     SI,CL                   ; ...
        and     DX,SI                   ; mask off unwanted bits
        xor     AX,DX                   ; restore bottom bits of AX
        pop     SI                      ; restore SI
        ret                             ; return
        endproc __SHRD

        endmod
        end
