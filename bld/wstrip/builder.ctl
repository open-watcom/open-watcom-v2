# WSTRIP Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wstrip

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wstripx.exe          <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwstrip<CMDEXT>
    rm -f <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wstripx.exe    <OWRELROOT>/binw/wstrip.exe
    <CCCMD> os2386/wstripx.exe    <OWRELROOT>/binp/wstrip.exe
    <CCCMD> nt386/wstripx.exe     <OWRELROOT>/binnt/wstrip.exe
    <CCCMD> linux386/wstripx.exe  <OWRELROOT>/binl/wstrip
    <CCCMD> ntaxp/wstripx.exe     <OWRELROOT>/axpnt/wstrip.exe
    <CCCMD> qnx386/wstripx.exe    <OWRELROOT>/qnx/binq/wstrip

[ BLOCK . . ]
#============
cdsay <PROJDIR>
