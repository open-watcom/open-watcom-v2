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
    <CCCMD> dosi86/wtouch.exe    <OWRELROOT>/binw/wtouch.exe
    <CCCMD> os2386/wtouch.exe    <OWRELROOT>/binp/wtouch.exe
    <CCCMD> nt386/wtouch.exe     <OWRELROOT>/binnt/wtouch.exe
    <CCCMD> linux386/wtouch.exe  <OWRELROOT>/binl/wtouch
    <CCCMD> ntaxp/wtouch.exe     <OWRELROOT>/axpnt/wtouch.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
