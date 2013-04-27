# wasppc Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wasppc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwasppc.exe <OWBINDIR>/bwasppc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasppc<CMDEXT>
    rm -f <OWBINDIR>/bwasppc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasppc.exe     <OWRELROOT>/binw/
    <CCCMD> dos386/wasppc.sym     <OWRELROOT>/binw/
    <CCCMD> os2386/wasppc.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wasppc.sym     <OWRELROOT>/binp/
    <CCCMD> nt386/wasppc.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wasppc.sym      <OWRELROOT>/binnt/
    <CCCMD> linux386/wasppc.exe   <OWRELROOT>/binl/wasppc
    <CCCMD> linux386/wasppc.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wasppc.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wasppc.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wasppc.exe      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
