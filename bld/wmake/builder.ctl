# WMake Builder Control file
# ==========================

set PROJNAME=wmake

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wmk.exe    <OWRELROOT>/binw/wmaker.exe
    <CCCMD> dos386/wmk.exe    <OWRELROOT>/binw/wmake.exe
    <CCCMD> os2386/wmk.exe    <OWRELROOT>/binp/wmake.exe
    <CCCMD> os2386/wmk.sym    <OWRELROOT>/binp/wmake.sym
    <CCCMD> nt386/wmk.exe     <OWRELROOT>/binnt/wmake.exe
    <CCCMD> nt386/wmk.sym     <OWRELROOT>/binnt/wmake.sym
    <CCCMD> linux386/wmk.exe  <OWRELROOT>/binl/wmake
    <CCCMD> linux386/wmk.sym  <OWRELROOT>/binl/wmake.sym
    <CCCMD> ntaxp/wmk.exe     <OWRELROOT>/axpnt/wmake.exe

    <CCCMD> qnxi86/wmk.exe    <OWRELROOT>/qnx/binq/wmake

    <CCCMD> ntx64/wmk.exe     <OWRELROOT>/binnt64/wmake.exe
    <CCCMD> linuxx64/wmk.exe  <OWRELROOT>/binl64/wmake
    <CCCMD> linuxarm/wmk.exe  <OWRELROOT>/arml/wmake

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
