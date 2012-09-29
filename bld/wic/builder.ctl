# WIC Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wic

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
    <CCCMD> dos386/wic.exe <OWRELROOT>/binw/wic.exe
    <CCCMD> os2386/wic.exe <OWRELROOT>/binp/wic.exe
    <CCCMD> nt386/wic.exe  <OWRELROOT>/binnt/wic.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
