# ZOOM Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    <CPCMD> <DEVDIR>/sdk/zoom/wini86/wzoom.exe <RELROOT>/binw/wzoom.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/sdk/zoom/nt386/wzoom.exe  <RELROOT>/binnt/wzoom.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/sdk/zoom/ntaxp/wzoom.exe  <RELROOT>/axpnt/wzoom.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
