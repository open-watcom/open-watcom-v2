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
;* Description:  16-bit chain interrupt handler function.
;*
;*****************************************************************************


include mdef.inc
include struct.inc

        name    chainint

_TEXT   segment word public 'CODE'
_TEXT   ends


_TEXT   segment

_chain_intr     proc far
        public  "C",_chain_intr
        mov     cx,ax                   ; get offset
        mov     ax,dx                   ; get segment
        mov     sp,bp                   ; reset SP to point to saved registers
        xchg    cx,20[bp]               ; restore cx, & put in offset
        xchg    ax,22[bp]               ; restore ax, & put in segment
        mov     bx,28[bp]               ; restore flags
        and     bx,0FCFFh               ; except for IF and TF
        push    bx                      ; :
        popf                            ; :
        pop     dx                      ; pop off dummy fs entry
        pop     dx                      ; pop off dummy gs entry
        pop     es                      ; restore segment registers
        pop     ds                      ;
        pop     di                      ; . . .
        pop     si                      ; . . .
        pop     bp                      ; . . .
        pop     bx                      ; restore registers
        pop     bx                      ; . . .
        pop     dx                      ; . . .
        ret                             ; return to previous interrupt handler
_chain_intr     endp

_TEXT   ends
        end
