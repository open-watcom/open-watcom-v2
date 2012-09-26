# SPY Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wspy

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wspy.exe    <OWRELROOT>/binw/wspy.exe
    <CPCMD> wini86/wspyhk.dll  <OWRELROOT>/binw/wspyhk.dll

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wspy.exe     <OWRELROOT>/binnt/wspy.exe
    <CPCMD> nt386/ntspyhk.dll  <OWRELROOT>/binnt/ntspyhk.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wspy.exe     <OWRELROOT>/axpnt/wspy.exe
    <CPCMD> ntaxp/ntspyhk.dll  <OWRELROOT>/axpnt/ntspyhk.dll

[ BLOCK . . ]
#============
cdsay <PROJDIR>
