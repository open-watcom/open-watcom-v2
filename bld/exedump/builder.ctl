# WDUMP Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wdump

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wdump.exe    <OWRELROOT>/binw/wdump.exe
    <CCCMD> os2386/wdump.exe    <OWRELROOT>/binp/wdump.exe
    <CCCMD> nt386/wdump.exe     <OWRELROOT>/binnt/wdump.exe
    <CCCMD> ntaxp/wdump.exe     <OWRELROOT>/axpnt/wdump.exe
    <CCCMD> qnx386/wdump.exe    <OWRELROOT>/qnx/wdump
    <CCCMD> linux386/wdump.exe  <OWRELROOT>/binl/wdump

[ BLOCK . . ]
#============
cdsay <PROJDIR>
