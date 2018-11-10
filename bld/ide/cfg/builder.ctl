# IDE configuration Builder Control file
# ======================================

set PROJNAME=idecfg

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================

    <CCCMD> linux386/*.cfg    <OWRELROOT>/binl/
    <CCCMD> linuxx64/*.cfg    <OWRELROOT>/binl64/
    <CCCMD> nt386/*.cfg       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/*.cfg       <OWRELROOT>/axpnt/
    <CCCMD> ntx64/*.cfg       <OWRELROOT>/binnt64/
    <CCCMD> os2386/*.cfg      <OWRELROOT>/binp/
    <CCCMD> wini86/*.cfg      <OWRELROOT>/binw/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
