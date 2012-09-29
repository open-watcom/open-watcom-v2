# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>
set PROJNAME=mkcdpg

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> nt386/mkcdpg.exe   <OWRELROOT>/binnt/mkcdpg.exe
    <CCCMD> ntaxp/mkcdpg.exe   <OWRELROOT>/axpnt/mkcdpg.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
