# WSTUB Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wstub

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (all build os_dos os_win cpu_i86 cpu_386 "") <2*> ]
    <CPCMD> dosi86/wstub.exe    <OWRELROOT>/binw/wstub.exe
    <CPCMD> dosi86/wstubq.exe   <OWRELROOT>/binw/wstubq.exe
    <CPCMD> wstub.c             <OWRELROOT>/src/wstub.c

[ BLOCK . . ]
#============
cdsay <PROJDIR>
