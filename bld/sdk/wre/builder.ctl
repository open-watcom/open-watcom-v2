# WRE Builder Control file
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
    <CPCMD> wini86/wre.exe         <OWRELROOT>/binw/wre.exe
    <CPCMD> wini86/wre.sym         <OWRELROOT>/binw/wre.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wre.exe          <OWRELROOT>/binnt/wre.exe
    <CPCMD> nt386/wre.sym          <OWRELROOT>/binnt/wre.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wre.exe          <OWRELROOT>/axpnt/wre.exe
    <CPCMD> ntaxp/wre.sym          <OWRELROOT>/axpnt/wre.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
