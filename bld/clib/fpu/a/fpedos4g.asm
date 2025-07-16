;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  FPE interrupt handler hook for DOS/4G style DOS extenders.
;*                (32-bit DOS code only)
;*
;*****************************************************************************


.8087
.386p

include struct.inc
include mdef.inc
include extender.inc
include fpeint.inc
include int21.inc


        xrefp           __FPE2Handler_  ; handle exceptions w/o OUT's

        modstart fpedos4g

        datasegment

        extrn   "C",__PMAddr    : dword
        extrn   "C",__PMSeg     : word

        enddata

        xdefp __DOS4G_hook_init_
defp    __DOS4G_hook_init_
        push    DS                      ; save DS
        mov     AH,35h                  ; get interrupt handler for
        mov     AL,FPE_INT              ; for INT 2 (INT 10 on NEC)
        int21h                          ; ...
        mov     __PMAddr,EBX            ; save protected-mode interrupt vector
        mov     __PMSeg,ES              ; ...
        mov     AH,25h                  ; set new FPE handler
        mov     AL,FPE_INT              ; get interrupt number
        mov     DX,CS                   ; set DS:EDX pointing to handler
        mov     DS,DX                   ; ...
        lea     EDX,__FPE2Handler_      ; ...
        int21h                          ; set it
        pop     DS                      ; restore DS
        ret                             ; return
endproc __DOS4G_hook_init_

;
;/* Disconnect */
;

        xdefp __DOS4G_hook_fini_
defp    __DOS4G_hook_fini_
        push    DS                      ; save DS
        mov     AH,25h                  ; set old FPE handler
        mov     AL,FPE_INT              ; get interrupt number
        mov     EDX,__PMAddr            ; set DS:EDX pointing to old handler
        mov     DS,__PMSeg              ; ...
        int21h                          ; restore previous interrupt handler
        pop     DS                      ; restore DS
        ret                             ; return
endproc __DOS4G_hook_fini_

        endmod
        end
