# WTOUCH Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wtouch

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wtouch.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/wtouch.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/wtouch.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/wtouch.exe  <OWRELROOT>/binl/wtouch
    <CCCMD> ntaxp/wtouch.exe     <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
