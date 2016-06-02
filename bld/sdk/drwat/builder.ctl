# DRWATCOM Builder Control file
# ============================

set PROJNAME=drwatcom

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/drwatcom.exe  <OWRELROOT>/binw/drwatcom.exe
    <CCCMD> nt386/drwatcom.exe   <OWRELROOT>/binnt/drwatcom.exe
    <CCCMD> win95/drwatcom.exe   <OWRELROOT>/binnt/_drwin95.exe
    <CCCMD> ntaxp/drwatcom.exe   <OWRELROOT>/axpnt/drwatcom.exe

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
