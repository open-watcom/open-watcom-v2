# CauseWay Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=CauseWay

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> cw32/dos386/cwdll.lib   <OWRELROOT>/lib386/dos/
    <CCCMD> inc/cwdll.h             <OWRELROOT>/h/

    <CCCMD> cw32/dosi86/cwstub.exe  <OWRELROOT>/binw/
    <CCCMD> cw32/dosi86/cwdstub.exe <OWRELROOT>/binw/
    <CCCMD> cw32/dosi86/cwstub.exe  <OWRELROOT>/binl/
    <CCCMD> cw32/dosi86/cwdstub.exe <OWRELROOT>/binl/

    <CCCMD> cwc/dosi86/cwc.exe      <OWRELROOT>/binw/
    <CCCMD> cwc/dosi86/cwstub.exe   <OWRELROOT>/binw/
    <CCCMD> cwc/nt386/cwc.exe       <OWRELROOT>/binnt/
    <CCCMD> cwc/nt386/cwstub.exe    <OWRELROOT>/binnt/
    <CCCMD> cwc/os2386/cwc.exe      <OWRELROOT>/binp/
    <CCCMD> cwc/os2386/cwstub.exe   <OWRELROOT>/binp/
    <CCCMD> cwc/linux386/cwc.exe    <OWRELROOT>/binl/cwc
    <CCCMD> cwc/linux386/cwstub.exe <OWRELROOT>/binl/

    <CCCMD> cwc/ntx64/cwc.exe       <OWRELROOT>/binnt64/
    <CCCMD> cwc/ntx64/cwstub.exe    <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
