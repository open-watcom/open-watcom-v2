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
;*** WINMISC.ASM - misc. routines                                         ***
;***                                                                      ***
;****************************************************************************
.386p
.387

DGROUP group _DATA

        include winext.inc

extrn   _DataSelector:WORD
extrn   _Has87:WORD
extrn   _DataSelectorSize:DWORD

_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment word public 'CODE' use16
assume cs:_TEXT
;****************************************************************************
;***                                                                      ***
;*** GetDataSelectorSize - get the size of the application DS             ***
;***                                                                      ***
;****************************************************************************
PUBLIC  GetDataSelectorSize_
GetDataSelectorSize_ proc
        push    eax
        push    ebx
        movzx   ebx,ds:_DataSelector
        lsl     eax,ebx                         ; segment limit of 32-bit area
        inc     eax
        mov     ds:_DataSelectorSize,eax        ; save size (used by dpmi fns)
        pop     ebx
        pop     eax
        ret
GetDataSelectorSize_ endp
_TEXT ends
        end
