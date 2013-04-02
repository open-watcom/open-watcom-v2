# WPROF Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wprof

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
    <CCCMD> dos386/wprof.exe     <OWRELROOT>/binw/
    <CCCMD> wini86/wprof.exe     <OWRELROOT>/binw/wprofw.exe
    <CCCMD> os2386pm/wprof.exe   <OWRELROOT>/binp/
    <CCCMD> os2386/wprof.exe     <OWRELROOT>/binp/wprofc.exe
    <CCCMD> nt386/wprof.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386c/wprof.exe     <OWRELROOT>/binnt/wprofc.exe
    <CCCMD> ntaxp/wprof.exe      <OWRELROOT>/axpnt/
    <CCCMD> linux386/wprof.exe   <OWRELROOT>/binl/wprof
    <CCCMD> linux386/wprof.sym   <OWRELROOT>/binl/
    <CCCMD> qnx386/wprof.qnx     <OWRELROOT>/qnx/binq/wprof
    <CCCMD> qnx386/wprof.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
