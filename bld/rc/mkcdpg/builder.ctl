# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/mkcdpg.exe   <OWRELROOT>/binnt/mkcdpg.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/mkcdpg.exe   <OWRELROOT>/axpnt/mkcdpg.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
