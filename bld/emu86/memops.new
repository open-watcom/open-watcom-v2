;                 x0     x1     x2      x3     x4     x5     x6     x7
loadrtn: dw     ld_flt,ld_nul,ld_long,ld_nul,ld_dbl,ld_nul,ld_int,ld_nul
ld_flt:
        load    temp,float
        _lea    _AX,st
        _lea    _DX,temp
        mov     _BX,_AX
        ret

ld_long:
        load    temp,long
        _lea    _AX,st
        _lea    _DX,temp
        mov     _BX,_AX
        ret

ld_dbl:
        load    temp,double
        _lea    _AX,st
        _lea    _DX,temp
        mov     _BX,_AX
        ret

ld_int:
        load    temp,integer
        _lea    _AX,st
        _lea    _DX,temp
        mov     _BX,_AX
ld_nul: ret



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
m02:;   fiadd   dword ptr memop
m04:;   fadd    qword ptr memop
m06:;   fiadd   word ptr memop
s00:;   fadd    st,st(i)
s04:;   fadd    st(i),st
;===================================================================
        call    __FLDA
        donearith


m10:;   fmul    dword ptr memop
m12:;   fimul   dword ptr memop
m14:;   fmul    qword ptr memop
m16:;   fimul   word ptr memop
s10:;   fmul    st,st(i)
s14:;   fmul    st(i),st
;===================================================================
        call    __FLDM
        donearith

m20:;   fcom    dword ptr memop
m22:;   ficom   dword ptr memop
m24:;   fcom    qword ptr memop
m26:;   ficom   word ptr memop
s20:;   fcom    st(i)
s24:;   fcom    st(i)
s45:;   fucom   st(i)
;===================================================================
        call    __FLDC
        saveflg_done

m30:;   fcomp   dword ptr memop
m32:;   ficomp  dword ptr memop
m34:;   fcomp   qword ptr memop
m36:;   ficomp  word ptr memop
s30:;   fcomp   st(i)
s34:;   fcomp   st(i)
s55:;   fucomp  st(i)
;===================================================================
        call    __FLDC
        saveflg_popstk_done

m50:;   fsubr   dword ptr memop
m52:;   fisubr  dword ptr memop
m54:;   fsubr   qword ptr memop
m56:;   fisubr  word ptr memop
s50:;   fsubr   st,st(i)
s44:;   fsubr   st(i),st
;===================================================================
        xchg    _DX,_AX

m40:;   fsub    dword ptr memop
m42:;   fisub   dword ptr memop
m44:;   fsub    qword ptr memop
m46:;   fisub   word ptr memop
s40:;   fsub    st,st(i)
s54:;   fsub    st(i),st
;===================================================================
        call    __FLDS
        donearith

m70:;   fdivr   dword ptr memop
m72:;   fidivr  dword ptr memop
m74:;   fdivr   qword ptr memop
m76:;   fidivr  word ptr memop
s70:;   fdivr   st,st(i)
s64:;   fdivr   st(i),st
;===================================================================
        xchg    _DX,_AX

m60:;   fdiv    dword ptr memop
m62:;   fidiv   dword ptr memop
m64:;   fdiv    qword ptr memop
m66:;   fidiv   word ptr memop
s60:;   fdiv    st,st(i)
s74:;   fdiv    st(i),st
;===================================================================
        call    __FLDD
        donearith

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

m01:;   fld     dword ptr memop
;===================================================================
        pushstk
        load    st,float
        done

m03:;   fild    dword ptr memop
;===================================================================
        pushstk
        load    st,long
        done

m05:;   fld     qword ptr memop
;===================================================================
        pushstk
        load    st,double
        done

m07:;   fild    word ptr memop
;===================================================================
        pushstk
        load    st,integer
        done

m21:;   fst     dword ptr memop
;===================================================================
        store   float
        done

m23:;   fist    dword ptr memop
;===================================================================
        save_st                         ; 24-mar-90 FWC
        rndint                          ; 24-mar-90
        store   long
        restore_st                      ; 24-mar-90
        done

m25:;   fst     qword ptr memop
;===================================================================
        store   double
        done

m27:;   fist    word ptr memop
;===================================================================
        save_st                         ; 24-mar-90 FWC
        rndint                          ; 24-mar-90
        store   integer
        restore_st                      ; 24-mar-90
        done

m31:;   fstp    dword ptr memop
;===================================================================
        store   float
        popstk_done

m33:;   fistp   dword ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   long
        popstk_done

m35:;   fstp    qword ptr memop
;===================================================================
        store   double
        popstk_done

m37:;   fistp   word ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   integer
        popstk_done

m41:;   fldenv  memop
;===================================================================
        rest87  ENV_SIZE
        done

m45:;   frstor  memop
;===================================================================
        rest87  NDP_SIZE
        done

m47:;   fbld    tbyte ptr memop
;===================================================================
        nyi     fbld
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

m53:;   fld     tbyte ptr memop
;===================================================================
        pushstk
        load    st,treal
        done

m57:;   fild    qword ptr memop
;===================================================================
        pushstk
        load    st,dlong                ; 24-mar-90 changed ulong to dlong
        done

m61:;   fstenv  memop
;===================================================================
        save87  ENV_SIZE
        done

m65:;   fsave   memop
;===================================================================
        save87  NDP_SIZE
        jmp     emu_finit

m67:;   fbstp   tbyte ptr memop
;===================================================================
        nyi     fbstp
        popstk_done

m71:;   fstcw   word ptr memop
;===================================================================
        getword ax,control
        saveword ax
        done

m73:;   fstp    tbyte ptr memop
;===================================================================
        store   treal
        popstk_done

m75:;   fstsw   word ptr memop
;===================================================================
        getword ax,status
        saveword ax
        done

m77:;   fistp   qword ptr memop
;===================================================================
        rndint                          ; 24-mar-90
        store   dlong                   ; 24-mar-90 changed ulong to dlong
        popstk_done

