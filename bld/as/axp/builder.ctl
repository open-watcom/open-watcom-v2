# wasaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wasaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <PROJDIR>/<OWOBJDIR>/wasaxp.exe   <OWBINDIR>/bwasaxp<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwasaxp<CMDEXT>
    rm -f <OWBINDIR>/bwasaxp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasaxp.exe     <OWRELROOT>/binw/wasaxp.exe
    <CCCMD> dos386/wasaxp.sym     <OWRELROOT>/binw/wasaxp.sym
    <CCCMD> os2386/wasaxp.exe     <OWRELROOT>/binp/wasaxp.exe
    <CCCMD> os2386/wasaxp.sym     <OWRELROOT>/binp/wasaxp.sym
    <CCCMD> nt386/wasaxp.exe      <OWRELROOT>/binnt/wasaxp.exe
    <CCCMD> nt386/wasaxp.sym      <OWRELROOT>/binnt/wasaxp.sym
    <CCCMD> linux386/wasaxp.exe   <OWRELROOT>/binl/wasaxp
    <CCCMD> linux386/wasaxp.sym   <OWRELROOT>/binl/wasaxp.sym
    <CCCMD> ntaxp/wasaxp.exe      <OWRELROOT>/axpnt/wasaxp.exe
    <CCCMD> ntaxp/wasaxp.sym      <OWRELROOT>/axpnt/wasaxp.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
