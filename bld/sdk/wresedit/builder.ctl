# WRESEDIT Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> waccel/wini86/wacc.dll     <RELROOT>/binw/wacc.dll
    <CPCMD> waccel/wini86/wacc.sym     <RELROOT>/binw/wacc.sym
    <CPCMD> wmenu/wini86/wmenu.dll     <RELROOT>/binw/wmenu.dll
    <CPCMD> wmenu/wini86/wmenu.sym     <RELROOT>/binw/wmenu.sym
    <CPCMD> wstring/wini86/wstring.dll <RELROOT>/binw/wstring.dll
    <CPCMD> wstring/wini86/wstring.sym <RELROOT>/binw/wstring.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> waccel/nt386/wacc.dll      <RELROOT>/binnt/wacc.dll
    <CPCMD> waccel/nt386/wacc.sym     <RELROOT>/binnt/wacc.sym
    <CPCMD> wmenu/nt386/wmenu.dll      <RELROOT>/binnt/wmenu.dll
    <CPCMD> wmenu/nt386/wmenu.sym      <RELROOT>/binnt/wmenu.sym
    <CPCMD> wstring/nt386/wstring.dll  <RELROOT>/binnt/wstring.dll
    <CPCMD> wstring/nt386/wstring.sym  <RELROOT>/binnt/wstring.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> waccel/ntaxp/wacc.dll      <RELROOT>/axpnt/wacc.dll
    <CPCMD> waccel/ntaxp/wacc.sym      <RELROOT>/axpnt/wacc.sym
    <CPCMD> wmenu/ntaxp/wmenu.dll      <RELROOT>/axpnt/wmenu.dll
    <CPCMD> wmenu/ntaxp/wmenu.sym      <RELROOT>/axpnt/wmenu.sym
    <CPCMD> wstring/ntaxp/wstring.dll  <RELROOT>/axpnt/wstring.dll
    <CPCMD> wstring/ntaxp/wstring.sym  <RELROOT>/axpnt/wstring.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
