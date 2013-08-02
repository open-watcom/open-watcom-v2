# IDE configuration Builder Control file
# ======================================

set PROJDIR=<CWD>
set PROJNAME=ide cfg

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

    <CCCMD> linux386/*.cfg    <OWRELROOT>/binl/
    <CCCMD> linuxx64/*.cfg    <OWRELROOT>/binl64/
    <CCCMD> nt386/*.cfg       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/*.cfg       <OWRELROOT>/axpnt/
    <CCCMD> ntx64/*.cfg       <OWRELROOT>/binnt64/
    <CCCMD> os2386/*.cfg      <OWRELROOT>/binp/
    <CCCMD> wini86/*.cfg      <OWRELROOT>/binw/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
