# WTOUCH Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wtouch

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/wtouch.exe    <OWRELROOT>/binw/wtouch.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wtouch.exe    <OWRELROOT>/binp/wtouch.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wtouch.exe     <OWRELROOT>/binnt/wtouch.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wtouch.exe  <OWRELROOT>/binl/wtouch

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wtouch.exe     <OWRELROOT>/axpnt/wtouch.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
