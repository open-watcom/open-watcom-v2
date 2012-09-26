# WDE Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wde

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wde.exe      <OWRELROOT>/binw/wde.exe
    <CPCMD> wini86/wde.sym      <OWRELROOT>/binw/wde.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wde.exe       <OWRELROOT>/binnt/wde.exe
    <CPCMD> nt386/wde.sym       <OWRELROOT>/binnt/wde.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wde.exe       <OWRELROOT>/axpnt/wde.exe
    <CPCMD> ntaxp/wde.sym       <OWRELROOT>/axpnt/wde.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
