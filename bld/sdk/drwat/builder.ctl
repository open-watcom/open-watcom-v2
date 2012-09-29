# DRWATCOM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=drwatcom

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> win/wini86/drwatcom.exe  <OWRELROOT>/binw/drwatcom.exe
    <CCCMD> nt/nt386/drwatcom.exe    <OWRELROOT>/binnt/drwatcom.exe
    <CCCMD> nt/win95/drwatcom.exe    <OWRELROOT>/binnt/_drwin95.exe
    <CCCMD> nt/ntaxp/drwatcom.exe    <OWRELROOT>/axpnt/drwatcom.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
