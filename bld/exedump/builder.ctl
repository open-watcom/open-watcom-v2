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
    <CCCMD> dosi86/wdump.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/wdump.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/wdump.exe     <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wdump.exe     <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wdump.exe    <OWRELROOT>/qnx/wdump
    <CCCMD> linux386/wdump.exe  <OWRELROOT>/binl/wdump

    <CCCMD> ntx64/wdump.exe     <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wdump.exe  <OWRELROOT>/binl64/wdump
    <CCCMD> linuxarm/wdump.exe  <OWRELROOT>/arml/wdump

[ BLOCK . . ]
#============
cdsay <PROJDIR>
