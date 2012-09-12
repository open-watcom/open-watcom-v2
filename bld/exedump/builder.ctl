# WDUMP Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wdump

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/wdump.exe    <OWRELROOT>/binw/wdump.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wdump.exe    <OWRELROOT>/binp/wdump.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wdump.exe     <OWRELROOT>/binnt/wdump.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wdump.exe     <OWRELROOT>/axpnt/wdump.exe

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wdump.exe    <OWRELROOT>/qnx/wdump

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wdump.exe  <OWRELROOT>/binl/wdump

[ BLOCK . . ]
#============
cdsay <PROJDIR>
