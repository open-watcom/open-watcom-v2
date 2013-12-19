# WDISASM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wdis

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
#    <CCCMD> osi386/wdis.exe   <OWRELROOT>/binw/
#    <CCCMD> osi386/wdis.sym   <OWRELROOT>/binw/
    <CCCMD> dos386/wdis.exe   <OWRELROOT>/binw/
    <CCCMD> dos386/wdis.sym   <OWRELROOT>/binw/
    <CCCMD> os2386/wdis.exe   <OWRELROOT>/binp/
    <CCCMD> os2386/wdis.sym   <OWRELROOT>/binp/
    <CCCMD> nt386/wdis.exe    <OWRELROOT>/binnt/
    <CCCMD> nt386/wdis.sym    <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wdis.exe    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wdis.sym    <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wdis.exe   <OWRELROOT>/qnx/binq/wdis
    <CCCMD> qnx386/wdis.sym   <OWRELROOT>/qnx/sym/
    <CCCMD> linux386/wdis.exe <OWRELROOT>/binl/wdis
    <CCCMD> linux386/wdis.sym <OWRELROOT>/binl/

    <CCCMD> ntx64/wdis.exe    <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wdis.exe <OWRELROOT>/binl64/wdis

[ BLOCK . . ]
#============
cdsay <PROJDIR>
