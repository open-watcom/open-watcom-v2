# WSTRIP Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wstrip

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wstripx.exe          <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwstrip<CMDEXT>
    rm -f <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dosi86/wstripx.exe    <OWRELROOT>/binw/wstrip.exe
    <CCCMD> <PROJDIR>/os2386/wstripx.exe    <OWRELROOT>/binp/wstrip.exe
    <CCCMD> <PROJDIR>/nt386/wstripx.exe     <OWRELROOT>/binnt/wstrip.exe
    <CCCMD> <PROJDIR>/linux386/wstripx.exe  <OWRELROOT>/binl/wstrip
    <CCCMD> <PROJDIR>/ntaxp/wstripx.exe     <OWRELROOT>/axpnt/wstrip.exe
    <CCCMD> <PROJDIR>/qnx386/wstripx.exe    <OWRELROOT>/qnx/binq/wstrip

[ BLOCK . . ]
#============
cdsay <PROJDIR>
