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

.386p

DGROUP group _DATA

;*
;*** data defined here
;*
_DATA segment word public 'DATA' use16
public _CopySize
_CopySize       dw      0               ; size for CopyMemory
_DATA ends

_TEXT segment word public 'CODE' use16

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

_TEXT   ends

        end
