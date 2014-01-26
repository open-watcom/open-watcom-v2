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

    <CPCMD> watcom.ico              <OWRELROOT>/
    <CPCMD> <OWROOT>/license.txt    <OWRELROOT>/

    <CCCMD> nt386/unicode*          <OWRELROOT>/binnt/
    <CCCMD> ntx64/unicode*          <OWRELROOT>/binnt64/
    <CCCMD> os2386/unicode*         <OWRELROOT>/binp/
    <CCCMD> linux386/unicode*       <OWRELROOT>/binl/
    <CCCMD> linuxx64/unicode*       <OWRELROOT>/binl64/
    <CCCMD> wini86/unicode*         <OWRELROOT>/binw/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
