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
    <CCCMD> wini86/wre.exe         <OWRELROOT>/binw/
    <CCCMD> wini86/wre.sym         <OWRELROOT>/binw/
    <CCCMD> nt386/wre.exe          <OWRELROOT>/binnt/
    <CCCMD> nt386/wre.sym          <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wre.exe          <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wre.sym          <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wre.exe          <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
