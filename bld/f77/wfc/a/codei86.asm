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

.8086

; CODEi86:     Pragma code bursts for built-in compiler pragmas
;
; - these routines are easier to code in assembler than in C

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

        name    codei86

.286p

_DATA   segment word public 'DATA'
        assume  CS:_DATA

module_start:
        dw      _Functions - module_start


; Space optimized pragmas

; es di - destination pointer
; ds si - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastEqOS
        db      "/* void __RTIStrBlastEq( es di, ds si, cx ) zaps di,si,cx */",0
        db      "#define __RTIStrBlastEqOS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOS_parms P_ESDI_DSSI_CX",0
        db      "#define __RTIStrBlastEqOS_saves HW_NotD_3( HW_DI, HW_SI, HW_CX )",0
        db      0
beginb  __RTIStrBlastEqOS
        rep   movsb
endb    __RTIStrBlastEqOS

; es di - destination pointer
; ax    - number of spaces to append
; ds si - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastNeOS
        db      "/* void __RTIStrBlastNe( es di, ax, ds si, cx ) zaps di,ax,si,cx */",0
        db      "#define __RTIStrBlastNeOS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOS_parms P_ESDI_AX_DSSI_CX",0
        db      "#define __RTIStrBlastNeOS_saves HW_NotD_4( HW_DI, HW_AX, HW_SI, HW_CX )",0
        db      0
beginb  __RTIStrBlastNeOS
        rep   movsb
        mov   cx, ax
        mov   ax, 0x2020
        rep   stosb
endb    __RTIStrBlastNeOS

; Time optimized pragmas

; es di - destination pointer
; ds si - source pointer
; cx    - number of 2 character tuples to move (strlen >> 21
; ax    - number of characters left over after initial 2-tuple move (strlen & 1)
defsb   __RTIStrBlastEqOT
        db      "/* void __RTIStrBlastEq( es di, ds si, cx, ax ) zaps cx,si,di */",0
        db      "#define __RTIStrBlastEqOT_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOT_parms P_ESDI_DSSI_CX_AX",0
        db      "#define __RTIStrBlastEqOT_saves HW_NotD_3( HW_CX, HW_SI, HW_DI )",0
        db      0
beginb  __RTIStrBlastEqOT
        rep   movsw
        mov   cx,ax
        rep   movsb
endb    __RTIStrBlastEqOT

; es di - destination pointer
; ax    - number of spaces to append
; ds si - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastNeOT
        db      "/* void __RTIStrBlastNe( es di, ax, ds si, cx ) zaps di,ax,si,cx */",0
        db      "#define __RTIStrBlastNeOT_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOT_parms P_ESDI_DX_DSSI_AX",0
        db      "#define __RTIStrBlastNeOT_saves HW_NotD_4( HW_DI, HW_AX, HW_SI, HW_CX )",0
        db      0
beginb  __RTIStrBlastNeOT
        mov   cx,ax
        shr   cx,1
        rep   movsw
        adc   cx,0
        rep   movsb
        mov   cx,dx
        mov   ax,0x2020
        shr   cx,1
        rep   stosw
        adc   cx,0
        rep   stosb
endb    __RTIStrBlastNeOT

; Windows pragmas (can't use DS as an argument since DS is pegged)

; Space optimized pragmas

; es di - destination pointer
; si bx - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastEqOSWin
        db      "/* void __RTIStrBlastEq( es di, si bx, cx ) zaps di,si,cx */",0
        db      "#define __RTIStrBlastEqOSWin_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOSWin_parms P_ESDI_SIBX_CX",0
        db      "#define __RTIStrBlastEqOSWin_saves HW_NotD_3( HW_DI, HW_SI, HW_CX )",0
        db      0
beginb  __RTIStrBlastEqOSWin
        push  ds
        mov   ds,si
        mov   si,bx
        rep   movsb
        pop   ds
endb    __RTIStrBlastEqOSWin

; es di - destination pointer
; ax    - number of spaces to append
; si bx - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastNeOSWin
        db      "/* void __RTIStrBlastNe( es di, ax, si bx, cx ) zaps di,ax,si,cx */",0
        db      "#define __RTIStrBlastNeOSWin_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOSWin_parms P_ESDI_AX_SIBX_CX",0
        db      "#define __RTIStrBlastNeOSWin_saves HW_NotD_4( HW_DI, HW_AX, HW_SI, HW_CX )",0
        db      0
beginb  __RTIStrBlastNeOSWin
        push  ds
        mov   ds,si
        mov   si,bx
        rep   movsb
        mov   cx, ax
        mov   ax, 0x2020
        rep   stosb
        pop   ds
endb    __RTIStrBlastNeOSWin

; Time optimized pragmas

; es di - destination pointer
; si bx - source pointer
; cx    - number of 2 character tuples to move (strlen >> 21
; ax    - number of characters left over after initial 2-tuple move (strlen & 1)
defsb   __RTIStrBlastEqOTWin
        db      "/* void __RTIStrBlastEq( es di, si bx, cx, ax ) zaps cx,si,di */",0
        db      "#define __RTIStrBlastEqOTWin_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOTWin_parms P_ESDI_SIBX_CX_AX",0
        db      "#define __RTIStrBlastEqOTWin_saves HW_NotD_3( HW_CX, HW_SI, HW_DI )",0
        db      0
beginb  __RTIStrBlastEqOTWin
        push  ds
        mov   ds,si
        mov   si,bx
        rep   movsw
        mov   cx,ax
        rep   movsb
        pop   ds
endb    __RTIStrBlastEqOTWin

; es di - destination pointer
; ax    - number of spaces to append
; si bx - source pointer
; cx    - number of characters to move
defsb   __RTIStrBlastNeOTWin
        db      "/* void __RTIStrBlastNe( es di, dx, si bx, ax ) zaps di,dx,si,ax,cx */",0
        db      "#define __RTIStrBlastNeOTWin_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOTWin_parms P_ESDI_DX_SIBX_AX",0
        db      "#define __RTIStrBlastNeOTWin_saves HW_NotD_5( HW_DI, HW_DX, HW_SI, HW_AX, HW_CX )",0
        db      0
beginb  __RTIStrBlastNeOTWin
        push  ds
        mov   ds,si
        mov   si,bx
        mov   cx,ax
        shr   cx,1
        rep   movsw
        adc   cx,0
        rep   movsb
        mov   cx,dx
        mov   ax,0x2020
        shr   cx,1
        rep   stosw
        adc   cx,0
        rep   stosb
        pop   ds
endb    __RTIStrBlastNeOTWin

; Small memory pragmas.

; Space optimized pragmas

; di   - destination pointer
; si   - source pointer
; cx   - number of characters to move
defsb   __RTIStrBlastEqOSS
        db      "/* void __RTIStrBlastEq( di, si, cx ) zaps di,si,cx */",0
        db      "#define __RTIStrBlastEqOSS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOSS_parms P_DI_SI_CX",0
        db      "#define __RTIStrBlastEqOSS_saves HW_NotD_1( HW_ES )",0
        db      0
beginb  __RTIStrBlastEqOSS
        push  ds
        pop   es
        rep   movsb
endb    __RTIStrBlastEqOSS

; di   - destination pointer
; ax   - number of spaces to append
; si   - source pointer
; cx   - number of characters to move
defsb   __RTIStrBlastNeOSS
        db      "/* void __RTIStrBlastNe( di, ax, si, cx ) zaps di,ax,si,cx */",0
        db      "#define __RTIStrBlastNeOSS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOSS_parms P_DI_AX_SI_CX",0
        db      "#define __RTIStrBlastNeOSS_saves HW_NotD_1( HW_ES )",0
        db      0
beginb  __RTIStrBlastNeOSS
        push  ds
        pop   es
        rep   movsb
        mov   cx, ax
        mov   ax, 0x2020
        rep   stosb
endb    __RTIStrBlastNeOSS

; Time optimized pragmas

; di   - destination pointer
; si   - source pointer
; cx   - number of 2 character tuples to move (strlen >> 21
; ax   - number of characters left over after initial 2-tuple move (strlen & 1)
defsb   __RTIStrBlastEqOTS
        db      "/* void __RTIStrBlastEq( di, si, cx, ax ) zaps di,si,cx */",0
        db      "#define __RTIStrBlastEqOTS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastEqOTS_parms P_DI_SI_CX_AX",0
        db      "#define __RTIStrBlastEqOTS_saves HW_NotD_1( HW_ES )",0
        db      0
beginb  __RTIStrBlastEqOTS
        push  ds
        pop   es
        rep   movsw
        mov   cx, ax
        rep   movsb
endb    __RTIStrBlastEqOTS

; di   - destination pointer
; ax   - number of spaces to append
; si   - source pointer
; cx   - number of characters to move
defsb   __RTIStrBlastNeOTS
        db      "/* void __RTIStrBlastNe( di, dx, si, ax ) zaps cx,es */",0
        db      "#define __RTIStrBlastNeOTS_ret   HW_D( HW_EMPTY )",0
        db      "#define __RTIStrBlastNeOTS_parms P_DI_DX_SI_AX",0
        db      "#define __RTIStrBlastNeOTS_saves HW_NotD_2( HW_CX, HW_ES )",0
        db      0
beginb  __RTIStrBlastNeOTS
        push  ds
        pop   es
        mov   cx, ax
        shr   cx, 1
        rep   movsw
        adc   cx, 0
        rep   movsb
        mov   cx, dx
        mov   ax, 0x2020
        shr   cx, 1
        rep   stosw
        adc   cx, 0
        rep   stosb
endb    __RTIStrBlastNeOTS


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
        func    __RTIStrBlastEqOSWin
        func    __RTIStrBlastNeOSWin
        func    __RTIStrBlastEqOTWin
        func    __RTIStrBlastNeOTWin
        func    __RTIStrBlastEqOSS
        func    __RTIStrBlastNeOSS
        func    __RTIStrBlastEqOTS
        func    __RTIStrBlastNeOTS

        dw      0,0,0
_DATA   ends

        end
