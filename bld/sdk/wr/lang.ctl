# WR Builder Control file
# =======================

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
    <CPCMD> <DEVDIR>/sdk/wr/wini86/wr.dll        <RELROOT>/binw/wr.dll
    <CPCMD> <DEVDIR>/sdk/wr/wini86/wr.sym        <RELROOT>/binw/wr.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/sdk/wr/nt386/wr.dll         <RELROOT>/binnt/wr.dll
    <CPCMD> <DEVDIR>/sdk/wr/nt386/wr.sym         <RELROOT>/binnt/wr.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/sdk/wr/ntaxp/wr.dll         <RELROOT>/axpnt/wr.dll
    <CPCMD> <DEVDIR>/sdk/wr/ntaxp/wr.sym         <RELROOT>/axpnt/wr.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
