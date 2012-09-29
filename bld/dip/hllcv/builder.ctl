# IBM HLL/CodeView DIP Builder Control file
# =========================================

set PROJDIR=<CWD>
set PROJNAME=hllcv

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK .<WATCOMBOOT>. .1. ]
    [ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK .<WATCOMBOOT>. .. ]
    [ INCLUDE <OWROOT>/build/defdylib.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/hllcv.dip      <OWRELROOT>/binw/
#    <CCCMD> dos386/hllcv.sym      <OWRELROOT>/binw/hllcv.dsy

    <CCCMD> wini86/hllcv.dll      <OWRELROOT>/binw/
#    <CCCMD> wini86/hllcv.sym      <OWRELROOT>/binw/

#    <CCCMD> os2i86/hllcv.dll      <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/hllcv.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/hllcv.d32      <OWRELROOT>/binp/
    <CCCMD> os2386/hllcv.sym      <OWRELROOT>/binp/

    <CCCMD> nt386/hllcv.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386/hllcv.sym       <OWRELROOT>/binnt/

    <CCCMD> linux386/hllcv.dip    <OWRELROOT>/binl/
    <CCCMD> linux386/hllcv.sym    <OWRELROOT>/binl/

    <CCCMD> ntaxp/hllcv.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/hllcv.sym       <OWRELROOT>/axpnt/

    <CCCMD> qnx386/hllcv.dip      <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/hllcv.sym      <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
