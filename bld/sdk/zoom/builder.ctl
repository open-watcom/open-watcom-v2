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
    <CCCMD> wini86/wzoom.exe <OWRELROOT>/binw/wzoom.exe
    <CCCMD> nt386/wzoom.exe  <OWRELROOT>/binnt/wzoom.exe
    <CCCMD> ntaxp/wzoom.exe  <OWRELROOT>/axpnt/wzoom.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
