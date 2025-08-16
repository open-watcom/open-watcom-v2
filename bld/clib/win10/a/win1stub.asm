;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Windows 1.0 stubs for non-existing Windows API entries. 
;*
;*****************************************************************************


_TEXT   segment word public 'CODE'

;
; Empty stub for IsDBCSLeadByte, which does not exist in Windows 1.x.
;
public WIN10ISDBCSLEADBYTE
WIN10ISDBCSLEADBYTE proc far
        xor ax,ax
        retf
WIN10ISDBCSLEADBYTE endp

;
; Empty stub for GetKBCodePage, which does not exist in any KEYBOARD driver in Windows 1.x.
;
public WIN10GETKBCODEPAGE
WIN10GETKBCODEPAGE proc far
        xor ax,ax
        retf
WIN10GETKBCODEPAGE endp

_TEXT   ends

end
