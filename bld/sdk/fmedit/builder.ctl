# FMEDIT Builder Control file
# ===========================

set PROJNAME=fmedit

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/fmedit.dll <OWRELROOT>/binw/
    <CCCMD> nt386/fmedit.dll  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/fmedit.dll  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/fmedit.dll  <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
