# wasaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wasaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwasaxp.exe <OWBINDIR>/bwasaxp<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasaxp<CMDEXT>
    rm -f <OWBINDIR>/bwasaxp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasaxp.exe     <OWRELROOT>/binw/
    <CCCMD> dos386/wasaxp.sym     <OWRELROOT>/binw/
    <CCCMD> os2386/wasaxp.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wasaxp.sym     <OWRELROOT>/binp/
    <CCCMD> nt386/wasaxp.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wasaxp.sym      <OWRELROOT>/binnt/
    <CCCMD> linux386/wasaxp.exe   <OWRELROOT>/binl/wasaxp
    <CCCMD> linux386/wasaxp.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wasaxp.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wasaxp.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wasaxp.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wasaxp.exe   <OWRELROOT>/binl64/wasaxp

[ BLOCK . . ]
#============
cdsay <PROJDIR>
