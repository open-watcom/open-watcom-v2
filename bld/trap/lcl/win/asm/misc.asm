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


;****************************************************************************
;***                                                                      ***
;*** WINMISC.ASM - misc support routines for windows trap file            ***
;***                                                                      ***
;****************************************************************************

no87        ; no87 to turn off emulation fixups

.386p
.387

DGROUP group _DATA

;*
;*** data defined here
;*
_DATA segment word public 'DATA' use16
public _CopySize
_CopySize       dw      0               ; size for CopyMemory
_DATA ends

_TEXT segment word public 'CODE' use16

assume cs:_TEXT

assume ds:DGROUP
assume ss:DGROUP

;****************************************************************************
;***                                                                      ***
;*** _CopyMemory - use WDEBUG.386 to copy memory between an arbitrary     ***
;***               selector:offset pair.                                  ***
;***                                                                      ***
;****************************************************************************
_CopyMemory_ PROC
public _CopyMemory_

        ;*
        ;*** put destination offset into edx
        ;*
        shl     edx,16
        mov     dx,di

        ;*
        ;*** put source offset into ebx
        ;*
        xchg    bx,si           ; no bx=hi, si =lo
        shl     ebx,16
        mov     bx,si

        mov     si,ax           ; source selector
        mov     di,_CopySize

        mov     ax,0fa01h       ; copy the bytes
        int     02fh            ; returns bytes copied in ax
        ret

_CopyMemory_ ENDP

;**********************************
;****                           ***
;**** Read8087 - get 8087 state ***
;****                           ***
;**********************************
Read8087_ PROC
        public  Read8087_
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        fsave   [bx]
        frstor  [bx]
        wait
        pop     bx
        pop     ds
        ret
Read8087_ ENDP

;**********************************
;****                           ***
;**** Write8087 - change 8087   ***
;****                           ***
;**********************************
Write8087_ PROC
        public  Write8087_
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        fsave   [bx]
        frstor  [bx]
        wait
        pop     bx
        pop     ds
        ret
Write8087_ ENDP

;************************************
;****                             ***
;**** Read80387 - get 80387 state ***
;****                             ***
;************************************
Read80387_ PROC
        public  Read80387_
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        db      66H
        fnsave   ds:[bx]
        wait
        db      66H
        frstor  ds:[bx]
        wait
        pop     bx
        pop     ds
        ret
Read80387_ ENDP

;**********************************
;****                           ***
;**** Write80387 - change 80387 ***
;****                           ***
;**********************************
Write80387_ PROC
        public  Write80387_
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        db      66h
        frstor  ds:[bx]
        wait
        pop     bx
        pop     ds
        ret
Write80387_ endp

_TEXT ends
        end
