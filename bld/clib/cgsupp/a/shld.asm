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
;==     Name:           SHLD                                            ==
;==     Operation:      shift left double (long)                        ==
;==     Inputs:         DX:AX   long integer to be shifted              ==
;==                     CX      integer shift count                     ==
;==     Outputs:        DX:AX   DX:AX << CX                             ==
;==     Volatile:       CX destroyed                                    ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        shld

        xdefp   __SHLD

        defp    __SHLD
        cmp     CL,16                   ; if shift count >= 16
        _if     ae                      ; then
          mov   DX,AX                   ; - shift left by 16
          sub   CL,16                   ; - adjust shift count
          sub   AX,AX                   ; - zero low word
          shl   DX,CL                   ; - shift the top word by (count-16)
          ret                           ; - return
        _endif                          ; endif
;
;       CL < 16
;
        push    SI                      ; save work reg
        shl     DX,CL                   ; shift top part
        rol     AX,CL                   ; rotate bottom part
        xor     DX,AX                   ; merge bottom bits of AX into DX
        mov     SI,0FFFFh               ; calc mask of bits to keep
        shl     SI,CL                   ; ...
        and     AX,SI                   ; mask off unwanted bits
        xor     DX,AX                   ; restore top bits of DX
        pop     SI                      ; restore SI
        ret                             ; return
        endproc __SHLD

        endmod
        end
