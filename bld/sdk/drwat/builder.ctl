# DRWATCOM Builder Control file
# ============================

set PROJNAME=drwatcom

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/drwatcom.exe  <OWRELROOT>/binw/
    <CCCMD> wini86/drwatcom.sym  <OWRELROOT>/binw/
    <CCCMD> nt386/drwatcom.exe   <OWRELROOT>/binnt/
    <CCCMD> nt386/drwatcom.sym   <OWRELROOT>/binnt/
    <CCCMD> win95/drwatcom.exe   <OWRELROOT>/binnt/_drwin95.exe
    <CCCMD> win95/drwatcom.sym   <OWRELROOT>/binnt/_drwin95.sym
    <CCCMD> ntaxp/drwatcom.exe   <OWRELROOT>/axpnt/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
