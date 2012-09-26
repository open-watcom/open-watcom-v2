# WR Builder Control file
# =======================

set PROJDIR=<CWD>
set PROJNAME=wr

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wr.dll        <OWRELROOT>/binw/wr.dll
    <CPCMD> wini86/wr.sym        <OWRELROOT>/binw/wr.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wr.dll         <OWRELROOT>/binnt/wr.dll
    <CPCMD> nt386/wr.sym         <OWRELROOT>/binnt/wr.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wr.dll         <OWRELROOT>/axpnt/wr.dll
    <CPCMD> ntaxp/wr.sym         <OWRELROOT>/axpnt/wr.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
