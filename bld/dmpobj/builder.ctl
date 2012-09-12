# dmpobj Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=dmpobj

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/dmpobj.exe    <OWRELROOT>/binw/dmpobj.exe
    <CCCMD> os2386/dmpobj.exe    <OWRELROOT>/binp/dmpobj.exe
    <CCCMD> nt386/dmpobj.exe     <OWRELROOT>/binnt/dmpobj.exe
    <CCCMD> linux386/dmpobj.exe  <OWRELROOT>/binl/dmpobj
    <CCCMD> ntaxp/dmpobj.exe     <OWRELROOT>/axpnt/dmpobj.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
