# WDUMP Builder Control file
# ==========================

set PROJNAME=wdump

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wdump.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/wdump.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/wdump.exe     <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wdump.exe     <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wdump.exe    <OWRELROOT>/qnx/wdump
    <CCCMD> linux386/wdump.exe  <OWRELROOT>/binl/wdump
    <CCCMD> rdos386/wdump.exe   <OWRELROOT>/rdos/

    <CCCMD> ntx64/wdump.exe     <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wdump.exe  <OWRELROOT>/binl64/wdump
    <CCCMD> linuxarm/wdump.exe  <OWRELROOT>/arml/wdump
    <CCCMD> osxx64/wdump.exe    <OWRELROOT>/osx64/wdump

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
