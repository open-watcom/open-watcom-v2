;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


; CODE386:     Pragma code bursts for built-in compiler pragmas
;
; - these routines are easier to code in assembler than in C
;

.386p

include struct.inc


beginb  macro   name
_&name&_name:
        db      "&name&",0
public  _&name
_&name:
        db      E_&name - _&name - 1
endm

endb    macro   name
E_&name:
endm

defsb    macro   name
_&name&_defs:
endm

        name    code386

_DATA   segment word public 'DATA'
        assume  CS:_DATA

module_start:
        dw      _Functions - module_start


; Space optimized pragmas (take an awful beating on a pentium)

; edi  - destination pointer
; esi  - source pointer
; ecx  - number of characters to move
defsb   __RTIStrBlastEqOS
        db      "/* void __RTIStrBlastEq( edi, esi, ecx ) zaps */",0
        db      "#define __RTIStrBlastEqOS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOS_parms P_EDI_ESI_ECX",0
        db      "#define __RTIStrBlastEqOS_saves HW_NotD_1( HW_EMPTY )",0
        db      0
beginb  __RTIStrBlastEqOS
        rep   movsb
endb    __RTIStrBlastEqOS

; edi  - destination pointer
; eax  - number of spaces to append
; esi  - source pointer
; ecx  - number of characters to move
defsb   __RTIStrBlastNeOS
        db      "/* void __RTIStrBlastNe( edi, eax, esi, ecx ) zaps */",0
        db      "#define __RTIStrBlastNeOS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOS_parms P_EDI_EAX_ESI_ECX",0
        db      "#define __RTIStrBlastNeOS_saves HW_NotD_1( HW_EMPTY )",0
        db      0
beginb  __RTIStrBlastNeOS
        rep   movsb
        mov   ecx, eax
        mov   eax, 0x20202020
        rep   stosb
endb    __RTIStrBlastNeOS

; Time optimized pragmas

; edi  - destination pointer
; esi  - source pointer
; ecx  - number of 4 character tuples to move (strlen >> 2)
; eax  - number of characters left over after initial 4-tuple move (strlen & 3)
defsb   __RTIStrBlastEqOT
        db      "/* void __RTIStrBlastEq( edi, esi, ecx, eax ) zaps */",0
        db      "#define __RTIStrBlastEqOT_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOT_parms P_EDI_ESI_ECX_EAX",0
        db      "#define __RTIStrBlastEqOT_saves HW_NotD_1( HW_EMPTY )",0
        db      0
beginb  __RTIStrBlastEqOT
        rep   movsd
        mov   ecx, eax
        rep   movsb
endb    __RTIStrBlastEqOT

; edi  - destination pointer
; eax  - number of spaces to append
; esi  - source pointer
; ecx  - number of characters to move
defsb   __RTIStrBlastNeOT
        db      "/* void __RTIStrBlastNe( edi, eax, esi, ecx ) zaps ecx */",0
        db      "#define __RTIStrBlastNeOT_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOT_parms P_EDI_EAX_ESI_ECX",0
        db      "#define __RTIStrBlastNeOT_saves HW_NotD_1( HW_ECX )",0
        db      0
beginb  __RTIStrBlastNeOT
        mov   ecx, eax
        shr   ecx, 2
        rep   movsd
        mov   ecx, eax
        and   ecx, 3
        rep   movsb
        mov   ecx, edx
        shr   ecx, 2
        mov   eax, 0x20202020
        rep   stosd
        mov   ecx, edx
        and   ecx, 3
        rep   stosb
endb    __RTIStrBlastNeOT



func    macro   name
        dw      _&name&_defs - module_start
        dw      _&name&_name - module_start
        dw      _&name - module_start
        endm

        public  _Functions

_Functions:
        func    __RTIStrBlastEqOS
        func    __RTIStrBlastNeOS
        func    __RTIStrBlastEqOT
        func    __RTIStrBlastNeOT

        dw      0,0,0

_DATA   ends

        end
