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


;
include mdef.inc
include struct.inc

.8087
        modstart        i4fd87

        xref            __8087  ; indicate that NDP instructions are present

        xdefp   __I4FD
        xdefp   __U4FD
        xdefp   __FDU4

;
; __I4FD      convert signed 32-bit integer in DX:AX into double float
; __U4FD      convert unsigned 32-bit integer in DX:AX into double float
;     Input:  DX:AX       - 32-bit integer
;     Output: AX BX CX DX - double precision representation of integer
;

        defpe   __I4FD
        or      DX,DX           ; if number is negative
        _if     s               ; then
          not   DX              ; - negate number
          neg   AX              ; - . . .
          sbb   DX,-1           ; - . . .
        _endif                  ; endif

;       convert unsigned 32-bit integer to double

        defpe   __U4FD
        push    BP
        sub     SP,4
        push    DX
        push    AX
        mov     BP,SP
        fild    dword ptr 0[BP]
        fstp    qword ptr 0[BP]
        fwait
        pop     DX
        pop     CX
        pop     BX
        pop     AX
        pop     BP
        ret                     ; return
        endproc __I4FD
        endproc __U4FD


        defpe   __FDU4
        push    BP
        push    AX
        push    BX
        push    CX
        push    DX
        mov     BP,SP
        fld     qword ptr 0[bp]
        fistp   dword ptr 4[BP]
        add     SP,4
        fwait
        pop     AX
        pop     DX
        pop     BP
        ret
        endproc __FDU4

        endmod
        end
