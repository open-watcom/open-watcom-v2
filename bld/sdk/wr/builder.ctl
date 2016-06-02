# WR Builder Control file
# =======================

set PROJNAME=wr

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wr.dll        <OWRELROOT>/binw/
    <CCCMD> wini86/wr.sym        <OWRELROOT>/binw/
    <CCCMD> nt386/wr.dll         <OWRELROOT>/binnt/
    <CCCMD> nt386/wr.sym         <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wr.dll         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wr.sym         <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wr.dll         <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
