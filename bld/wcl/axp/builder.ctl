# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwclaxp.exe <OWBINDIR>/bwclaxp<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwclaxp<CMDEXT>
    rm -f <OWBINDIR>/bwclaxp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wclaxp.exe     <OWRELROOT>/binw/wclaxp.exe
    <CCCMD> dosi86/wclaxp.sym     <OWRELROOT>/binw/wclaxp.sym
    <CCCMD> nt386/wclaxp.exe      <OWRELROOT>/binnt/wclaxp.exe
    <CCCMD> nt386/wclaxp.sym      <OWRELROOT>/binnt/wclaxp.sym
    <CCCMD> os2386/wclaxp.exe     <OWRELROOT>/binp/wclaxp.exe
    <CCCMD> os2386/wclaxp.sym     <OWRELROOT>/binp/wclaxp.sym
    <CCCMD> linux386/wclaxp.exe   <OWRELROOT>/binl/wclaxp
    <CCCMD> linux386/wclaxp.sym   <OWRELROOT>/binl/wclaxp.sym
    <CCCMD> ntaxp/wclaxp.exe      <OWRELROOT>/axpnt/wclaxp.exe
    <CCCMD> ntaxp/wclaxp.sym      <OWRELROOT>/axpnt/wclaxp.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
