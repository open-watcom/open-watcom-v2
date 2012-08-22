# WDE Builder Control file
# ========================

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
    <CPCMD> wini86/wde.exe      <OWRELROOT>/binw/wde.exe
    <CPCMD> wini86/wde.sym      <OWRELROOT>/binw/wde.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wde.exe       <OWRELROOT>/binnt/wde.exe
    <CPCMD> nt386/wde.sym       <OWRELROOT>/binnt/wde.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wde.exe       <OWRELROOT>/axpnt/wde.exe
    <CPCMD> ntaxp/wde.sym       <OWRELROOT>/axpnt/wde.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
