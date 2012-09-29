# WR Builder Control file
# =======================

set PROJDIR=<CWD>
set PROJNAME=wr

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wr.dll        <OWRELROOT>/binw/wr.dll
    <CCCMD> wini86/wr.sym        <OWRELROOT>/binw/wr.sym
    <CCCMD> nt386/wr.dll         <OWRELROOT>/binnt/wr.dll
    <CCCMD> nt386/wr.sym         <OWRELROOT>/binnt/wr.sym
    <CCCMD> ntaxp/wr.dll         <OWRELROOT>/axpnt/wr.dll
    <CCCMD> ntaxp/wr.sym         <OWRELROOT>/axpnt/wr.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
