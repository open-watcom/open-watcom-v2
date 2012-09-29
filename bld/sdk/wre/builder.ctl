# WRE Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wre

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wre.exe         <OWRELROOT>/binw/wre.exe
    <CCCMD> wini86/wre.sym         <OWRELROOT>/binw/wre.sym
    <CCCMD> nt386/wre.exe          <OWRELROOT>/binnt/wre.exe
    <CCCMD> nt386/wre.sym          <OWRELROOT>/binnt/wre.sym
    <CCCMD> ntaxp/wre.exe          <OWRELROOT>/axpnt/wre.exe
    <CCCMD> ntaxp/wre.sym          <OWRELROOT>/axpnt/wre.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
