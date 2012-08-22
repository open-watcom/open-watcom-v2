# ZOOM Builder Control file
# ============================

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
    <CPCMD> wini86/wzoom.exe <OWRELROOT>/binw/wzoom.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wzoom.exe  <OWRELROOT>/binnt/wzoom.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wzoom.exe  <OWRELROOT>/axpnt/wzoom.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
