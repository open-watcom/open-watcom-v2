# MISC Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

    <CPCMD> unicode*                <OWRELROOT>/binw/
    <CPCMD> watcom.ico              <OWRELROOT>/
    <CPCMD> unicode*                <OWRELROOT>/binl/
    <CPCMD> <OWROOT>/license.txt    <OWRELROOT>/license.txt

[ BLOCK . . ]
#============
cdsay <PROJDIR>
