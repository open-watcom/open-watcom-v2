# WRE Builder Control file
# ========================

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
    <CPCMD> <DEVDIR>/sdk/wre/wini86/wre.exe         <RELROOT>/binw/wre.exe
    <CPCMD> <DEVDIR>/sdk/wre/wini86/wre.sym         <RELROOT>/binw/wre.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/sdk/wre/nt386/wre.exe          <RELROOT>/binnt/wre.exe
    <CPCMD> <DEVDIR>/sdk/wre/nt386/wre.sym          <RELROOT>/binnt/wre.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/sdk/wre/ntaxp/wre.exe          <RELROOT>/axpnt/wre.exe
    <CPCMD> <DEVDIR>/sdk/wre/ntaxp/wre.sym          <RELROOT>/axpnt/wre.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
