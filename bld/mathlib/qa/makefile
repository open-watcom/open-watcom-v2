#
# Math libraray test program makefile (see mathtest.c)
#

# code  platform            invocation
# ====  ========            ==========
# DOS   16-bit DOS          dos_fpic
#                           dos_fpcc
#                           dos_fpih
#                           dos_fpch
#                           dos_fpil
#                           dos_fpcl
#                           dos_fpim
#                           dos_fpcm
#                           dos_fpis
#                           dos_fpcs
# PLS   PharLap             run386 pls_fpir
#                           run386 pls_fpcr
#                           run386 pls_fpis
#                           run386 pls_fpcs
# RSI   Rational Systems    rsi_fpir
#                           rsi_fpcr
#                           rsi_fpis
#                           rsi_fpcs
# CW    Causeway            cw_fpir
#                           cw_fpcr
#                           cw_fpis
#                           cw_fpcs
# OS2   OS/2 16-bit         os2_fpi
#                           os2_fpc
# OS3   OS/2 32-bit         os3_fpir
#                           os3_fpcr
#                           os3_fpis
#                           os3_fpcs
# WIN   Windows 16-bit      create icons ; click
# W32   Windows 32-bit      create icon; click
# WNT   Windows NT          wnt_fpir
#                           wnt_fpcr
#                           wnt_fpis
#                           wnt_fpcs
# LNX   Linux 386           lnx_fpir
#                           lnx_fpcr
#                           lnx_fpis
#                           lnx_fpcs

!include cproj.mif
!include deftarg.mif

.erase
.extensions:
.extensions: .exe .rex .exp .obj .c .asm

.before
!ifdef __UNIX__
    @set WATCOM=$(%RELROOT)
    @set INCLUDE=
    @set DOS_INCLUDE=$(%WATCOM)/h
    @set NT_INCLUDE=$(%WATCOM)/h
    @set OS2_INCLUDE=$(%WATCOM)/h
    @set QNX_INCLUDE=$(%WATCOM)/qh
    @set LINUX_INCLUDE=$(%WATCOM)/lh
    @set WINDOWS_INCLUDE=$(%WATCOM)/h/win:$(%WATCOM)/h
    @set PATH=$(%WATCOM)/binl:$(%WATCOM)/binw:$(%PATH)
!else
    @set WATCOM=$(%RELROOT)
    @set INCLUDE=
    @set DOS_INCLUDE=$(%WATCOM)\h
    @set NT_INCLUDE=$(%WATCOM)\h
    @set OS2_INCLUDE=$(%WATCOM)\h
    @set QNX_INCLUDE=$(%WATCOM)\qh
    @set LINUX_INCLUDE=$(%WATCOM)\lh
    @set WINDOWS_INCLUDE=$(%WATCOM)\h\win;$(%WATCOM)\h
    @set PATH=$(%WATCOM)\binnt;$(%WATCOM)\binw;$(%PATH)
!endif

mathtest = ../c/mathtest.c

comp_opts = -za99 -ox -d2 -zq -fo=$^*.obj
link_opts = NAME $^* OPTION quiet, map DEBUG all

exes = &
    dos_fpis.exe dos_fpcs.exe &
    dos_fpim.exe dos_fpcm.exe &
    dos_fpic.exe dos_fpcc.exe &
    dos_fpil.exe dos_fpcl.exe &
    dos_fpih.exe dos_fpch.exe &
    pls_fpir.exp pls_fpcr.exp &
    pls_fpis.exp pls_fpcs.exp &
    rsi_fpir.exe rsi_fpcr.exe &
    rsi_fpis.exe rsi_fpcs.exe &
    cw_fpir.exe cw_fpcr.exe &
    cw_fpis.exe cw_fpcs.exe &
    os2_fpi.exe os2_fpc.exe &
    os3_fpir.exe os3_fpcr.exe &
    os3_fpis.exe os3_fpcs.exe &
    win_fpi.exe win_fpc.exe &
    w32_fpir.exe w32_fpcr.exe &
    w32_fpis.exe w32_fpcs.exe &
    wnt_fpir.exe wnt_fpcr.exe &
    wnt_fpis.exe wnt_fpcs.exe &
    lnx_fpir.exe lnx_fpcr.exe &
    lnx_fpis.exe lnx_fpcs.exe

all : .symbolic $(exes)

dos_fpis.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -ms -fpi -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpcs.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -ms -fpc -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpim.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mm -fpi -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpcm.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mm -fpc -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpic.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mc -fpi -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpcc.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mc -fpc -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpil.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -ml -fpi -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpcl.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -ml -fpc -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpih.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mh -fpi -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

dos_fpch.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -mh -fpc -bt=dos
    wlink SYSTEM dos $(link_opts) FILE $^*.obj

pls_fpir.exp : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3r
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*.obj

pls_fpcr.exp : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3r
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*.obj

pls_fpis.exp : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3s
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*.obj

pls_fpcs.exp : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3s
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*.obj

rsi_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3r
    wlink SYSTEM dos4g $(link_opts) FILE $^*.obj

rsi_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3r
    wlink SYSTEM dos4g $(link_opts) FILE $^*.obj

rsi_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3s
    wlink SYSTEM dos4g $(link_opts) FILE $^*.obj

rsi_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3s
    wlink SYSTEM dos4g $(link_opts) FILE $^*.obj

cw_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3r
    wlink SYSTEM causeway $(link_opts) FILE $^*.obj

cw_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3r
    wlink SYSTEM causeway $(link_opts) FILE $^*.obj

cw_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=dos -3s
    wlink SYSTEM causeway $(link_opts) FILE $^*.obj

cw_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=dos -3s
    wlink SYSTEM causeway $(link_opts) FILE $^*.obj

os2_fpi.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -fpi -bt=os2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*.obj

os2_fpc.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -fpc -bt=os2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*.obj

os3_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=os2 -3r
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*.obj

os3_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=os2 -3r
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*.obj

os3_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=os2 -3s
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*.obj

os3_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=os2 -3s
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*.obj

win_fpi.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -fpi -bw -zw -bt=windows
    wlink SYSTEM windows $(link_opts) FILE $^*.obj

win_fpc.exe : $(mathtest)
    $(cc_i86) $[@ $(comp_opts) -fpc -bw -zw -bt=windows
    wlink SYSTEM windows $(link_opts) FILE $^*.obj

w32_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bw -zw -bt=windows -3r
    wlink SYSTEM win386 $(link_opts) FILE $^*.obj
    wbind -q -s $(lang_root)/binw/win386.ext -n $^*
    rm $^*.rex

w32_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bw -zw -bt=windows -3r
    wlink SYSTEM win386 $(link_opts) FILE $^*.obj
    wbind -q -s $(lang_root)/binw/win386.ext -n $^*
    rm $^*.rex

w32_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bw -zw -bt=windows -3s
    wlink SYSTEM win386 $(link_opts) FILE $^*.obj
    wbind -q -s $(lang_root)/binw/win386.ext -n $^*
    rm $^*.rex

w32_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bw -zw -bt=windows -3s
    wlink SYSTEM win386 $(link_opts) FILE $^*.obj
    wbind -q -s $(lang_root)/binw/win386.ext -n $^*
    rm $^*.rex

wnt_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=nt -3r
    wlink SYSTEM nt $(link_opts) FILE $^*.obj

wnt_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=nt -3r
    wlink SYSTEM nt $(link_opts) FILE $^*.obj

wnt_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=nt -3s
    wlink SYSTEM nt $(link_opts) FILE $^*.obj

wnt_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=nt -3s
    wlink SYSTEM nt $(link_opts) FILE $^*.obj

lnx_fpir.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=linux -3r
    wlink SYSTEM linux $(link_opts) FILE $^*.obj NAME $@

lnx_fpcr.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=linux -3r
    wlink SYSTEM linux $(link_opts) FILE $^*.obj NAME $@

lnx_fpis.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpi -bt=linux -3s
    wlink SYSTEM linux $(link_opts) FILE $^*.obj NAME $@

lnx_fpcs.exe : $(mathtest)
    $(cc_386) $[@ $(comp_opts) -fpc -bt=linux -3s
    wlink SYSTEM linux $(link_opts) FILE $^*.obj NAME $@
