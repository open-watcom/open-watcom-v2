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


; Structured macros for MASM.

        __label = 0
        __depth = 0

_setname macro  name,value
        name = value
        endm

_guess  macro   name
        __depth = __depth + 1
        __label = __label + 1
        _set    __astk,%__depth,__label,
        __label = __label + 1
        _set    __estk,%__depth,__label,
        ifnb    <name>
;;        name = __label
          _setname name,__label
        endif
        endm

_quif   macro   cc,name
        ifnb    <name>
          _set  __elbl,,name,
          _j    cc,_l,%__elbl
        else
          _set  __albl,,__astk,%__depth
          _j    cc,_l,%__albl
        endif
        endm

_quit   macro   name
        _quif   ,name
        endm

_admit  macro
        _set    __albl,,__astk,%__depth
        __label = __label + 1
        _set    __astk,%__depth,__label,
        _set    __elbl,,__estk,%__depth
        _jmp    _l,%__elbl
        _label  _l,%__albl
        endm

_endguess macro
        _set    __albl,,__astk,%__depth
        _label  _l,%__albl
        _set    __elbl,,__estk,%__depth
        _label  _l,%__elbl
        __depth = __depth - 1
        endm

_loop   macro   name
        _guess  name
        _set    __albl,,__astk,%__depth
        _label  _m,%__albl
        endm

_until  macro   cc
        _set    __albl,,__astk,%__depth
        _jn     cc,_m,%__albl
        _endguess
        endm

_endloop macro
        _set    __albl,,__astk,%__depth
        _jmp    _m,%__albl
        _endguess
        endm

_if     macro   cc
        _guess  ;;
        _set    __albl,,__astk,%__depth
        _jn     cc,_l,%__albl
        endm

_else   macro
        _admit
        endm

_endif  macro
        _endguess
        endm

_set    macro   base1,ext1,base2,ext2
        base1&ext1 = base2&ext2
        endm

_label  macro   base,ext
base&ext:
        endm

_jmp    macro   base,ext
        jmp     short base&ext
        endm

_j      macro   cc,base,ext
        j&cc    short base&ext
        endm

_jn     macro   cc,base,ext
        _jn&cc  base&ext
        endm

_jnna   macro   label
        ja      short label
        endm

_jnnae  macro   label
        jae     short label
        endm

_jnnb   macro   label
        jb      short label
        endm

_jnnbe  macro   label
        jbe     short label
        endm

_jnnc   macro   label
        jc      short label
        endm

_jnne   macro   label
        je      short label
        endm

_jnng   macro   label
        jg      short label
        endm

_jnnge  macro   label
        jge     short label
        endm

_jnnl   macro   label
        jl      short label
        endm

_jnnle  macro   label
        jle     short label
        endm

_jnno   macro   label
        jo      short label
        endm

_jnnp   macro   label
        jp      short label
        endm

_jnns   macro   label
        js      short label
        endm

_jnnz   macro   label
        jz      short label
        endm

_jnpe   macro   label
        jpo     short label
        endm

_jnpo   macro   label
        jpe     short label
        endm

_jna    macro   label
        jna     short label
        endm

_jnae   macro   label
        jnae    short label
        endm

_jnb    macro   label
        jnb     short label
        endm

_jnbe   macro   label
        jnbe    short label
        endm

_jnc    macro   label
        jnc     short label
        endm

_jne    macro   label
        jne     short label
        endm

_jng    macro   label
        jng     short label
        endm

_jnge   macro   label
        jnge    short label
        endm

_jnl    macro   label
        jnl     short label
        endm

_jnle   macro   label
        jnle    short label
        endm

_jno    macro   label
        jno     short label
        endm

_jnp    macro   label
        jnp     short label
        endm

_jns    macro   label
        jns     short label
        endm

_jnz    macro   label
        jnz     short label
        endm

_jpe    macro   label
        jpe     short label
        endm

_jpo    macro   label
        jpo     short label
        endm

_shl    macro   reg,count
        add     reg,reg
        endm

_rcl    macro   reg,count
        adc     reg,reg
        endm
