# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclppc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwclppc.exe <OWBINDIR>/bwclppc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwclppc<CMDEXT>
    rm -f <OWBINDIR>/bwclppc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wclppc.exe     <OWRELROOT>/binw/
    <CCCMD> dosi86/wclppc.sym     <OWRELROOT>/binw/
    <CCCMD> nt386/wclppc.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wclppc.sym      <OWRELROOT>/binnt/
    <CCCMD> os2386/wclppc.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wclppc.sym     <OWRELROOT>/binp/

    <CCCMD> ntx64/wclppc.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wclppc.exe   <OWRELROOT>/binl64/wclppc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
