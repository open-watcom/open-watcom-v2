# Resource tools Builder Control file
# ===================================

set PROJNAME=mkcdpg

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> nt386/mkcdpg.exe   <OWRELROOT>/binnt/
    <CCCMD> ntaxp/mkcdpg.exe   <OWRELROOT>/axpnt/

    <CCCMD> ntx64/mkcdpg.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
