# DDESPY Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=ddespy

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wddespy.exe <OWRELROOT>/binw/wddespy.exe
    <CCCMD> nt386/wddespy.exe  <OWRELROOT>/binnt/wddespy.exe
    <CCCMD> ntaxp/wddespy.exe  <OWRELROOT>/axpnt/wddespy.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
