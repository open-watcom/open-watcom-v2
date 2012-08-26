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


; Holes in the opcode space
;===================================================================
s02:
s12:
s22:
s32:
s42:
s62:
s72:
s03:
s13:
s23:
s33:
s53:
s63:
s73:
s65:
s75:
s26:
s57:
s67:
s77:
sxxx:
        illop
        done

s21:    jmptab   s210, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx
;===================================================================

s210:;  fnop
;===================================================================
        done

s00:;   fadd    st,st(i)
;===================================================================
        _add    st,st(i),st
        donearith

s10:;   fmul    st,st(i)
;===================================================================
        _mul    st,st(i),st
        donearith

s20:;   fcom    st(i)
;===================================================================
s24:;
        _cmp    st,st(i)
        saveflg_done

s30:;   fcomp   st(i)
;===================================================================
s34:;   redundant
        _cmp    st,st(i)
        saveflg_popstk_done

s40:;   fsub    st,st(i)
;===================================================================
        _sub    st,st(i),st
        donearith

s50:;   fsubr   st,st(i)
;===================================================================
        _sub    st(i),st,st
        donearith

s60:;   fdiv    st,st(i)
;===================================================================
        _div    st,st(i),st
        donearith

s70:;   fdivr   st,st(i)
;===================================================================
        _div    st(i),st,st
        donearith

s01:;   fld     st(i)
;===================================================================
        pushstk
        load    st,st(i)
        done

s11:;   fxch    st(i)
;===================================================================
s15:;   redundant
s17:;   redundant
        _xchg   st,st(i)
        done

s41:    jmptab   s410, s411, sxxx, sxxx, s414, s415, sxxx, sxxx
;===================================================================

s410:;  fchs
;===================================================================
        _neg    st
        done

s411:;  fabs
;===================================================================
        _abs    st
        done

s414:;  ftst
;===================================================================
        ldcons  zero
        _cmp    st,temp
        saveflg_done

s415:;  fxam
;===================================================================
        st_top  fxam
        done

s51:    jmptab   s510, s511, s512, s513, s514, s515, s516, sxxx
;===================================================================

s510:;  fld1
;===================================================================
        stcons  03FFFh,08000h,00000h,00000h,00000h      ; one
        done

s511:;  fldl2t
;===================================================================
        stcons  04000h,0D49Ah,0784Bh,0CD1Bh,08AFEh      ; log2ten
        done

s512:;  fldl2e
;===================================================================
        stcons  03FFFh,0B8AAh,03B29h,05C17h,0F0BCh      ; log2e
        done

s513:;  fldpi
;===================================================================
        stcons  04000h,0C90Fh,0DAA2h,02168h,0C235h      ; pi
        done

s514:;  fldlg2
;===================================================================
        stcons  03FFDh,09A20h,09A84h,0FBCFh,0F799h      ; log2
        done

s515:;  fldln2
;===================================================================
        stcons  03FFEh,0B172h,017F7h,0D1CFh,079ACh      ; ln2
        done

s516:;  fldz
;===================================================================
        stcons  00000h,00000h,00000h,00000h,00000h      ; zero
        tag_st0 t_zero
        done

s61:    jmptab   s610, s611, s612, s613, s614, s615, s616, s617
;===================================================================

s610:;  f2xm1
;===================================================================
        st_top  f2xm1
        donearith

s611:;  fyl2x
;===================================================================
        st_top2 fyl2x
        donearith

s612:;  fptan
;===================================================================
        st_top  tan
        stcons  03FFFh,08000h,00000h,00000h,00000h      ; one
        done

; s613 thru s615 are really processor specific

s616:;  fdecstp
;===================================================================
        decstp
        done

s617:;  fincstp
;===================================================================
        incstp
        done


s71:    jmptab   s710, s711, s712, s713, s714, s715, s716, s717
;===================================================================

s711:;  fly2xp1
;===================================================================
        st_top2 fyl2xp1
        donearith

s712:;  fsqrt
;===================================================================
        st_top  sqrt
        donearith

s713:;  fsincos
;===================================================================
        save_st
        st_top  sin
        pushstk
        restore_st
        st_top  cos
        done

; s714 & s715 are really processor specific

s716:;  fsin
;===================================================================
        st_top  sin
        donearith

s717:;  fcos
;===================================================================
        st_top  cos
        donearith

s52:    jmptab   sxxx, s521, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx
;===================================================================

s521:;  fucompp
;===================================================================
        load    temp,st
        popstk
        _cmp    temp,st
        saveflg_popstk_done

s43:    jmptab   s430, s431, s432, s433, s434, sxxx, sxxx, sxxx
;===================================================================

s430:;  feni
;===================================================================
        done

s431:;  fdisi
;===================================================================
        done

s432:;  fclex
        _fclex
        done

s433:;  finit
;===================================================================
emu_finit:
        _finit
        done

s434:;  fsetpm
;===================================================================
        done

s04:;   fadd    st(i),st
;===================================================================
        _add    st(i),st,st(i)
        donearith

s14:;   fmul    st(i),st
;===================================================================
        _mul    st(i),st,st(i)
        donearith

s44:;   fsubr   st(i),st
;===================================================================
        _sub    st,st(i),st(i)
        donearith

s54:;   fsub    st(i),st
;===================================================================
        _sub    st(i),st,st(i)
        donearith

s64:;   fdivr   st(i),st
;===================================================================
        _div    st,st(i),st(i)
        donearith

s74:;   fdiv    st(i),st
;===================================================================
        _div    st(i),st,st(i)
        donearith

s05:;   ffree   st(i)
;===================================================================
s07:;   redundant
        tag_sti t_empty
        done

s25:;   fst     st(i)
;===================================================================
s27:;   redundant
        store   st(i)
        tag_sti t_valid
        done

s45:;   fucom   st(i)
;===================================================================
        _cmp    st,st(i)
        saveflg_done

s55:;   fucomp  st(i)
;===================================================================
        _cmp    st,st(i)
        saveflg_popstk_done

s35:;   fstp    st(i)
;===================================================================
s31:;   redundant
s37:;   redundant
        store   st(i)
        tag_sti t_valid
        popstk_done

s06:;   faddp   st(i),st
;===================================================================
        _add    st(i),st,st(i)
        popstk
        donearith

s16:;   fmulp   st(i),st
;===================================================================
        _mul    st(i),st,st(i)
        popstk
        donearith

s36:    jmptab   sxxx, s361, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx
;===================================================================

s361:;  fcompp
;===================================================================
        load    temp,st
        popstk
        _cmp    temp,st
        saveflg_popstk_done

s46:;   fsubrp  st(i),st
;===================================================================
        _sub    st,st(i),st(i)
        popstk
        donearith

s56:;   fsubp   st(i),st
;===================================================================
        _sub    st(i),st,st(i)
        popstk
        donearith

s66:;   fdivrp  st(i),st
;===================================================================
        _div    st,st(i),st(i)
        popstk
        donearith

s76:;   fdivp   st(i),st
;===================================================================
        _div    st(i),st,st(i)
        popstk
        donearith

s47:    jmptab   s470, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx, sxxx
;===================================================================

; s470 is really processor specific
