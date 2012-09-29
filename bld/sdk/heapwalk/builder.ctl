# HEAPWALKER Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=whepwlk

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wheapwlk.exe <OWRELROOT>/binw/wheapwlk.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
