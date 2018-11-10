# Resource tools Builder Control file
# ===================================

set PROJNAME=rcexedmp

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> os2386/exedmp.exe  <OWRELROOT>/binp/
    <CCCMD> nt386/exedmp.exe   <OWRELROOT>/binnt/

    <CCCMD> ntx64/exedmp.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
