# WSTUB Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wstub

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wstub.exe    <OWRELROOT>/binw/wstub.exe
    <CCCMD> dosi86/wstubq.exe   <OWRELROOT>/binw/wstubq.exe
    <CCCMD> wstub.c             <OWRELROOT>/src/wstub.c

[ BLOCK . . ]
#============
cdsay <PROJDIR>
