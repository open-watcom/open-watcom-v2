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


;========================================================================
;==     Name:           PCA PCS                                        ==
;==     Operation:      Pointer plus long integer                      ==
;==     Inputs:         es:bx pointer                                  ==
;==                     ds:ax long int                                 ==
;==     Outputs:        es:bx has es:bx op ds:ax as  pointer           ==
;==     Volatile:       DX:AX and CX:BX                                ==
;==                                                                    ==
;==                                                                    ==
;========================================================================
include mdef.inc

        modstart        pca

        xdefp   __PCA
        xdefp   __PCS

        defp    __PCS
        neg     dx              ; negate the 32 bit integer
        neg     ax              ; ...
        sbb     dx,0            ; ...

        defp    __PCA
        push    cx              ; need a register
        add     bx,ax           ; add low part of integer into offset
        adc     dx,0            ; propagate carry into high part of integer
        mov     cl,4            ; adjust high part into segment position
        ror     dx,cl           ; ...
        and     dx,0f000h       ; ...
        mov     ax,es           ; and add to segment
        add     ax,dx           ; ... (new unnormalized pointer is now in ax:bx)
        mov     dx,bx           ; make a copy of the new offset
        shr     dx,cl           ; adjust into segment position
        add     ax,dx           ; add into segment
        and     bx,000fh        ; clear high 12 bits of new offset
        mov     es,ax           ; resulting in normalized pointer in es:bx
        pop     cx              ; restore a register
        ret                     ; ...
        endproc __PCA
        endproc __PCS

        endmod
        end
