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


;*
;*** WINEXT.INC   - windows extender include file
;***
;*


Start16BitLand MACRO
        push    esi
        push    edi
        push    ebp
        mov     ebp,esp         ; addressability to parms
ENDM

BackTo32BitLand MACRO popval
        ;
        ; assumes that bx contains data segment (loaded in StubStackRestore)
        ;
        mov     ds,bx
        mov     es,bx
        pop     ebp
        pop     edi
        pop     esi
        db      66h                             ; override to use EIP
    IFNB <popval>
        ret     (4*popval)                      ; return to 32-bit land
    ELSE
        ret                                     ; return to 32-bit land
    ENDIF
ENDM

StubStackSet MACRO
        push    ss ; 32 bit stack
        pop     es
        mov     ds:[_SaveSP],esp
        lss     sp,ds:[_EntryStackSave]
        push    ebp
ENDM

StubStackRestore MACRO
        pop     ecx
        mov     word ptr ds:[_EntryStackSave],sp
        mov     word ptr ds:[_EntryStackSave+2],ss
        mov     bx,ds:[_DataSelector]
        mov     ss,bx
        mov     esp,ecx
ENDM
