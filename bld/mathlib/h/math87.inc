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


ifdef   __386__
 _BP    equ     EBP
 _SP    equ     ESP
 _AX    equ     EAX
 _BX    equ     EBX
 _CX    equ     ECX
 _DX    equ     EDX
 _POP_LD_ equ   12      ; amount to pop from stack to remove long double
else
 _BP    equ     BP
 _SP    equ     SP
 _AX    equ     AX
 _BX    equ     BX
 _CX    equ     CX
 _DX    equ     DX
 _POP_LD_ equ   10      ; amount to pop from stack to remove long double
endif

loadres macro
 ifdef __STACK__
        sub     ESP,8           ; allocate space
        fstp    qword ptr [ESP] ; store result
        fwait                   ; wait
        pop     EAX             ; load result into EDX:EAX
        pop     EDX             ; ...
 endif
        endm

ret_pop macro   amount
 ifdef __386__
  ifdef __STACK__
        ret
  else
        ret     amount
  endif
 else
        ret     amount
 endif
        endm


chk387  macro
ifdef __386__
    cmp     byte ptr __8087,3       ; if 387 present
else
 if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        push    ds                      ; save DS
        mov     ax,DGROUP               ; get access to DGROUP
        mov     ds,ax                   ; . . .
        cmp     byte ptr __8087,3       ; if 387 present
        pop     ds                      ; restore ds
 else
        cmp     byte ptr __8087,3       ; if 387 present
 endif
endif
        endm

; The following must be in sync
;  with watcom/mathcode.h
;
FUNC_ACOS  equ  1
FUNC_ASIN  equ  2
FUNC_SQRT  equ  3
FUNC_EXP   equ  4
FUNC_COSH  equ  5
FUNC_SINH  equ  6
FUNC_POW   equ  7
FUNC_ACOSH equ  8
FUNC_LOG2  equ  9
FUNC_LOG   equ  10
FUNC_LOG10 equ  11
FUNC_ATANH equ  12
FUNC_ATAN2 equ  13
FUNC_IPOW  equ  14
FUNC_DPOWI equ  15
FUNC_COS   equ  16
FUNC_SIN   equ  17
FUNC_TAN   equ  18
FUNC_Y0    equ  19
FUNC_Y1    equ  20
FUNC_YN    equ  21
FUNC_MOD   equ  22
FUNC_COTAN equ  23

do_real_fdiv macro op1, op2, rev, popper
if rev
  if popper
     fdivrp     st(op1), st(op2)
  else
     fdivr      st(op1), st(op2)
  endif
else
  if popper
     fdivp      st(op1), st(op2)
  else
     fdiv       st(op1), st(op2)
  endif
endif
endm

ifdef PDIV

extern_chipbug macro
        extrn   __chipbug:byte
endm

chk_baddiv  macro
ifdef __386__
    test     byte ptr __chipbug,1       ; if bad divider
else
 if (_MODEL and (_BIG_DATA or _HUGE_DATA)) and ((_MODEL and _DS_PEGGED) eq 0)
        push    ds                      ; save DS
        mov     ax,DGROUP               ; get access to DGROUP
        mov     ds,ax                   ; . . .
        test    byte ptr __chipbug,1    ; if bad divider
        pop     ds                      ; restore ds
 else
        test    byte ptr __chipbug,1    ; if bad divider
 endif
endif
        endm

do_fdiv_raw macro op1,op2,rev,popper
        xref    __fdiv_fpr
        chk_baddiv
        _if e
            do_real_fdiv op1,op2,rev,popper
        _else
            if op1 ne 0
              fdiv_encoding = (op1 shl 3) or 04h or (rev shl 1) or popper
            else
              fdiv_encoding = (op2 shl 3)        or (rev shl 1) or popper
            endif
            ifdef __386__
              mov       eax,fdiv_encoding
            else
              mov       ax,fdiv_encoding
            endif
            call        __fdiv_fpr
        _endif
endm

do_fpatan macro
        xref    __fpatan_wrap
        chk_baddiv
        _if e
          fpatan
        _else
          call __fpatan_wrap
        _endif
endm

do_fptan macro
        xref    __fptan_chk
        chk_baddiv
        _if e
          fptan
        _else
          call __fptan_chk
        _endif
endm

do_fprem macro
        xref    __fprem_chk
        chk_baddiv
        _if e
          fprem
        _else
          call __fprem_chk
        _endif
endm

do_fprem1 macro
        xref    __fprem1_chk
        chk_baddiv
        _if e
          fprem1
        _else
          call __fprem1_chk
        _endif
endm


else

extern_chipbug macro
endm

do_fdiv_raw macro op1, op2, rev, popper
        do_real_fdiv op1,op2,rev,popper
endm

do_fpatan macro
        fpatan
endm

do_fptan macro
        fptan
endm

do_fprem macro
        fprem
endm

do_fprem1 macro
        fprem1
endm

endif

do_fdiv macro op1,op2
   do_fdiv_raw op1,op2,0,0
endm

do_fdivr macro op1,op2
   do_fdiv_raw op1,op2,1,0
endm

do_fdivp macro op1,op2
   do_fdiv_raw op1,op2,0,1
endm

do_fdivrp macro op1,op2
   do_fdiv_raw op1,op2,1,1
endm
