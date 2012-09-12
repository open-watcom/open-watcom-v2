# DRWATCOM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=drwatcom

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> win/wini86/drwatcom.exe  <OWRELROOT>/binw/drwatcom.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/nt386/drwatcom.exe    <OWRELROOT>/binnt/drwatcom.exe
    <CPCMD> nt/win95/drwatcom.exe    <OWRELROOT>/binnt/_drwin95.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> nt/ntaxp/drwatcom.exe    <OWRELROOT>/axpnt/drwatcom.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
