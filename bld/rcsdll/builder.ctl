# RCSDLL Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=rcsdll

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/rcsdll.dll  <OWRELROOT>/binw/rcsdll.dll
    <CPCMD> bat/*.bat          <OWRELROOT>/binw/
    <CCCMD> os2386/rcsdll.dll  <OWRELROOT>/binp/dll/rcsdll.dll
    <CPCMD> cmd/*.cmd          <OWRELROOT>/binp/
    <CCCMD> nt386/rcsdll.dll   <OWRELROOT>/binnt/rcsdll.dll
    <CCCMD> ntaxp/rcsdll.dll   <OWRELROOT>/axpnt/rcsdll.dll

[ BLOCK . . ]
#============
cdsay <PROJDIR>
