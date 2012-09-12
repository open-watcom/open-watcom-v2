# whc Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=whc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/whc.exe      <OWRELROOT>/binw/
    <CCCMD> nt386/whc.exe       <OWRELROOT>/binnt/
    <CCCMD> os2386/whc.exe      <OWRELROOT>/binp/
    <CCCMD> linux386/whc.exe    <OWRELROOT>/binl/whc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
