# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>
set PROJNAME=rcexedmp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> os2386/exedmp.exe  <OWRELROOT>/binp/
    <CCCMD> nt386/exedmp.exe   <OWRELROOT>/binnt/

    <CCCMD> ntx64/exedmp.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
