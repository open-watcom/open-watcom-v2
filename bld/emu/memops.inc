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

m11:
m13:
m43:
m63:
m15:
m55:
m17:
        illop
        done

m00:;   fadd    dword ptr memop
;===================================================================
        load    temp,float
        _add    st,temp,st
        donearith

m10:;   fmul    dword ptr memop
;===================================================================
        load    temp,float
        _mul    st,temp,st
        donearith

m20:;   fcom    dword ptr memop
;===================================================================
        load    temp,float
        _cmp    st,temp
        saveflg_done

m30:;   fcomp   dword ptr memop
;===================================================================
        load    temp,float
        _cmp    st,temp
        saveflg_popstk_done

m40:;   fsub    dword ptr memop
;===================================================================
        load    temp,float
        _sub    st,temp,st
        donearith

m50:;   fsubr   dword ptr memop
;===================================================================
        load    temp,float
        _sub    temp,st,st
        donearith

m60:;   fdiv    dword ptr memop
;===================================================================
        load    temp,float
        _div    st,temp,st
        donearith

m70:;   fdivr   dword ptr memop
;===================================================================
        load    temp,float
        _div    temp,st,st
        donearith

m01:;   fld     dword ptr memop
;===================================================================
        pushstk
        load    st,float
        done

m21:;   fst     dword ptr memop
;===================================================================
        store   float
        done

m31:;   fstp    dword ptr memop
;===================================================================
        store   float
        popstk_done

m41:;   fldenv  memop
;===================================================================
        rest87  ENV_SIZE
;;;     fish_st
        done

m51:;   fldcw   word ptr memop
;===================================================================
        loadword ax
        putword ax,control
        and     ax,CW_PC                ; isolate precision control
        _guess                          ; guess: extended
          cmp   ax,PC_EXTENDED          ; - quit if not extended
          _quif ne                      ; - ...
          setp_extended                 ; - ...
        _admit                          ; guess: double
          cmp   ax,PC_DOUBLE            ; - quit if not double
          _quif ne                      ; - ...
          setp_double
        _admit                          ; admit: single
          setp_single
        _endguess                       ; endguess
        done

m61:;   fstenv  memop
;===================================================================
        save87  ENV_SIZE
        done

m71:;   fstcw   word ptr memop
;===================================================================
        getword ax,control
        saveword ax
        done

m02:;   fiadd   dword ptr memop
;===================================================================
        load    temp,long
        _add    st,temp,st
        donearith

m12:;   fimul   dword ptr memop
;===================================================================
        load    temp,long
        _mul    st,temp,st
        donearith

m22:;   ficom   dword ptr memop
;===================================================================
        load    temp,long
        _cmp    st,temp
        saveflg_done

m32:;   ficomp  dword ptr memop
;===================================================================
        load    temp,long
        _cmp    st,temp
        saveflg_popstk_done

m42:;   fisub   dword ptr memop
;===================================================================
        load    temp,long
        _sub    st,temp,st
        donearith

m52:;   fisubr  dword ptr memop
;===================================================================
        load    temp,long
        _sub    temp,st,st
        donearith

m62:;   fidiv   dword ptr memop
;===================================================================
        load    temp,long
        _div    st,temp,st
        donearith

m72:;   fidivr  dword ptr memop
;===================================================================
        load    temp,long
        _div    temp,st,st
        donearith

m03:;   fild    dword ptr memop
;===================================================================
        pushstk
        load    st,long
        done

m23:;   fist    dword ptr memop
;===================================================================
        save_st                         ; 24-mar-90 FWC
        rndint                          ; 24-mar-90
        store   long
        restore_st                      ; 24-mar-90
        done

m33:;   fistp   dword ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   long
        popstk_done

m04:;   fadd    qword ptr memop
;===================================================================
        load    temp,double
        _add    st,temp,st
        donearith

m14:;   fmul    qword ptr memop
;===================================================================
        load    temp,double
        _mul    st,temp,st
        donearith

m24:;   fcom    qword ptr memop
;===================================================================
        load    temp,double
        _cmp    st,temp
        saveflg_done

m34:;   fcomp   qword ptr memop
;===================================================================
        load    temp,double
        _cmp    st,temp
        saveflg_popstk_done

m44:;   fsub    qword ptr memop
;===================================================================
        load    temp,double
        _sub    st,temp,st
        donearith

m54:;   fsubr   qword ptr memop
;===================================================================
        load    temp,double
        _sub    temp,st,st
        donearith

m64:;   fdiv    qword ptr memop
;===================================================================
        load    temp,double
        _div    st,temp,st
        donearith

m74:;   fdivr   qword ptr memop
;===================================================================
        load    temp,double
        _div    temp,st,st
        donearith

m53:;   fld     tbyte ptr memop
;===================================================================
        pushstk
        load    st,treal
        done

m05:;   fld     qword ptr memop
;===================================================================
        pushstk
        load    st,double
        done

m25:;   fst     qword ptr memop
;===================================================================
        store   double
        done

m73:;   fstp    tbyte ptr memop
;===================================================================
        store   treal
        popstk_done

m35:;   fstp    qword ptr memop
;===================================================================
        store   double
        popstk_done

m45:;   frstor  memop
;===================================================================
        rest87  NDP_SIZE
;;;     fish_st
        done

m65:;   fsave   memop
;===================================================================
        save87  NDP_SIZE
        jmp     emu_finit

m75:;   fstsw   word ptr memop
;===================================================================
        getword ax,status
        saveword ax
        done

m06:;   fiadd   word ptr memop
;===================================================================
        load    temp,integer
        _add    st,temp,st
        donearith

m16:;   fimul   word ptr memop
;===================================================================
        load    temp,integer
        _mul    st,temp,st
        donearith

m26:;   ficom   word ptr memop
;===================================================================
        load    temp,integer
        _cmp    st,temp
        saveflg_done

m36:;   ficomp  word ptr memop
;===================================================================
        load    temp,integer
        _cmp    st,temp
        saveflg_popstk_done

m46:;   fisub   word ptr memop
;===================================================================
        load    temp,integer
        _sub    st,temp,st
        donearith

m56:;   fisubr  word ptr memop
;===================================================================
        load    temp,integer
        _sub    temp,st,st
        donearith

m66:;   fidiv   word ptr memop
;===================================================================
        load    temp,integer
        _div    st,temp,st
        donearith

m76:;   fidivr  word ptr memop
;===================================================================
        load    temp,integer
        _div    temp,st,st
        donearith

m07:;   fild    word ptr memop
;===================================================================
        pushstk
        load    st,integer
        done

m27:;   fist    word ptr memop
;===================================================================
        save_st                         ; 24-mar-90 FWC
        rndint                          ; 24-mar-90
        store   integer
        restore_st                      ; 24-mar-90
        done

m37:;   fistp   word ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   integer
        popstk_done

m47:;   fbld    tbyte ptr memop
;===================================================================
        nyi     fbld
        done

m57:;   fild    qword ptr memop
;===================================================================
        pushstk
        load    st,dlong                ; 24-mar-90 changed ulong to dlong
        done

m67:;   fbstp   tbyte ptr memop
;===================================================================
        nyi     fbstp
        popstk_done

m77:;   fistp   qword ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   dlong                   ; 24-mar-90 changed ulong to dlong
        popstk_done

