# WDE Builder Control file
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
    <CPCMD> <DEVDIR>/sdk/wde/wini86/wde.exe      <RELROOT>/binw/wde.exe
    <CPCMD> <DEVDIR>/sdk/wde/wini86/wde.sym      <RELROOT>/binw/wde.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/sdk/wde/nt386/wde.exe       <RELROOT>/binnt/wde.exe
    <CPCMD> <DEVDIR>/sdk/wde/nt386/wde.sym       <RELROOT>/binnt/wde.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/sdk/wde/ntaxp/wde.exe       <RELROOT>/axpnt/wde.exe
    <CPCMD> <DEVDIR>/sdk/wde/ntaxp/wde.sym       <RELROOT>/axpnt/wde.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
