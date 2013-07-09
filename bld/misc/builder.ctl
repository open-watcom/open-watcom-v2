# MISC Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=misc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    set OWDOCROOT=<OWROOT>/docs

    <CPCMD> unicode*                <OWRELROOT>/binw/
    <CPCMD> watcom.ico              <OWRELROOT>/
    <CPCMD> unicode*                <OWRELROOT>/binl/
    <CPCMD> <OWROOT>/license.txt    <OWRELROOT>/
    <CPCMD> <OWDOCROOT>/readme.txt  <OWRELROOT>/
    <CPCMD> <OWDOCROOT>/areadme.txt <OWRELROOT>/
    <CPCMD> <OWDOCROOT>/freadme.txt <OWRELROOT>/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
