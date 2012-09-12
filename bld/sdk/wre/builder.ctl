# WRE Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wre

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wre.exe         <OWRELROOT>/binw/wre.exe
    <CPCMD> wini86/wre.sym         <OWRELROOT>/binw/wre.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wre.exe          <OWRELROOT>/binnt/wre.exe
    <CPCMD> nt386/wre.sym          <OWRELROOT>/binnt/wre.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wre.exe          <OWRELROOT>/axpnt/wre.exe
    <CPCMD> ntaxp/wre.sym          <OWRELROOT>/axpnt/wre.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
