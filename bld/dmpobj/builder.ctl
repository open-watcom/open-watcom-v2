# dmpobj Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=dmpobj

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/dmpobj.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/dmpobj.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/dmpobj.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/dmpobj.exe  <OWRELROOT>/binl/dmpobj
    <CCCMD> ntaxp/dmpobj.exe     <OWRELROOT>/axpnt/

    <CCCMD> ntx64/dmpobj.exe     <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
