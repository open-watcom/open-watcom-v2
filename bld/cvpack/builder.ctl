# cvpack Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=cvpack

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/cvpack.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/cvpack.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/cvpack.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/cvpack.exe  <OWRELROOT>/binl/cvpack
    <CCCMD> ntaxp/cvpack.exe     <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
