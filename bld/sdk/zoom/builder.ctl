# ZOOM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wzoom

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wzoom.exe <OWRELROOT>/binw/wzoom.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wzoom.exe  <OWRELROOT>/binnt/wzoom.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wzoom.exe  <OWRELROOT>/axpnt/wzoom.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
