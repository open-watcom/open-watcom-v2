# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclmps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwclmps.exe <OWBINDIR>/bwclmps<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwclmps<CMDEXT>
    rm -f <OWBINDIR>/bwclmps<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> nt386/wclmps.exe      <OWRELROOT>/binnt/wclmps.exe
    <CCCMD> nt386/wclmps.sym      <OWRELROOT>/binnt/wclmps.sym
    <CCCMD> os2386/wclmps.exe     <OWRELROOT>/binp/wclmps.exe
    <CCCMD> os2386/wclmps.sym     <OWRELROOT>/binp/wclmps.sym
    <CCCMD> linux386/wclmps.exe   <OWRELROOT>/binl/wclmps
    <CCCMD> linux386/wclmps.sym   <OWRELROOT>/binl/wclmps.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
