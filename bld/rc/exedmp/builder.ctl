# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/exedmp.exe  <OWRELROOT>/binp/exedmp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/exedmp.exe   <OWRELROOT>/binnt/exedmp.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
