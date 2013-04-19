# DDESPY Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=ddespy

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wddespy.exe <OWRELROOT>/binw/
    <CCCMD> nt386/wddespy.exe  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wddespy.exe  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wddespy.exe  <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
