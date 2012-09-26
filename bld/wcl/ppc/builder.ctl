# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclppc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot rel ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwclppc.exe <OWBINDIR>/bwclppc<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwclppc<CMDEXT>
    rm -f <OWBINDIR>/bwclppc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dosi86/wclppc.exe     <OWRELROOT>/binw/wclppc.exe
    <CCCMD> <PROJDIR>/dosi86/wclppc.sym     <OWRELROOT>/binw/wclppc.sym
    <CCCMD> <PROJDIR>/nt386/wclppc.exe      <OWRELROOT>/binnt/wclppc.exe
    <CCCMD> <PROJDIR>/nt386/wclppc.sym      <OWRELROOT>/binnt/wclppc.sym
    <CCCMD> <PROJDIR>/os2386/wclppc.exe     <OWRELROOT>/binp/wclppc.exe
    <CCCMD> <PROJDIR>/os2386/wclppc.sym     <OWRELROOT>/binp/wclppc.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
