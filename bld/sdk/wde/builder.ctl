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
    <CCCMD> wini86/wde.exe      <OWRELROOT>/binw/
    <CCCMD> wini86/wde.sym      <OWRELROOT>/binw/
    <CCCMD> nt386/wde.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386/wde.sym       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wde.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wde.sym       <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wde.exe       <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
