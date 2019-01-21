# ZOOM Builder Control file
# ============================

set PROJNAME=wzoom

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/wzoom.exe <OWRELROOT>/binw/
    <CCCMD> nt386/wzoom.exe  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wzoom.exe  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wzoom.exe  <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
