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
;* Description:  Small function to load the helped DLL into the debuggee.
;*               See pgmexec.c for details.
;*
;*****************************************************************************


.386p

extrn           DosLoadModule : near

public          LoadHelperDLL_
public          EndLoadHelperDLL_

_TEXT segment byte public 'CODE' use32

assume cs:_TEXT

; Why use the indirect call? A direct call to DosLoadModule would be a near
; call and relative to caller's address. When this code is copied to the
; debuggee's address space, it wouldn't work. We get around that easily by
; getting DosLoadModule's absolute address and calling that. Note that the
; address is guaranteed to be same in both (all) processes due to the way
; DLLs are implemented on OS/2.
LoadHelperDLL_  proc    near
        mov     eax, offset DosLoadModule      ; get DosLoadModule address
        call    eax                            ; load WDSPLICE.DLL
        int     3                              ; breakpoint to get control back
EndLoadHelperDLL_ label byte
LoadHelperDLL_  endp

_TEXT           ends

                end
