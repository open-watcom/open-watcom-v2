# WDE Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wde

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wde.exe      <OWRELROOT>/binw/wde.exe
    <CCCMD> wini86/wde.sym      <OWRELROOT>/binw/wde.sym
    <CCCMD> nt386/wde.exe       <OWRELROOT>/binnt/wde.exe
    <CCCMD> nt386/wde.sym       <OWRELROOT>/binnt/wde.sym
    <CCCMD> ntaxp/wde.exe       <OWRELROOT>/axpnt/wde.exe
    <CCCMD> ntaxp/wde.sym       <OWRELROOT>/axpnt/wde.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
