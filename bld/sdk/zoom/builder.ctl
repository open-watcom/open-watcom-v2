# ZOOM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wzoom

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wzoom.exe <OWRELROOT>/binw/
    <CCCMD> nt386/wzoom.exe  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wzoom.exe  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wzoom.exe  <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
