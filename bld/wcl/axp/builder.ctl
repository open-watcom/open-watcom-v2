# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot rel ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwclaxp.exe <OWBINDIR>/bwclaxp<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwclaxp<CMDEXT>
    rm -f <OWBINDIR>/bwclaxp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dosi86/wclaxp.exe     <OWRELROOT>/binw/wclaxp.exe
    <CCCMD> <PROJDIR>/dosi86/wclaxp.sym     <OWRELROOT>/binw/wclaxp.sym
    <CCCMD> <PROJDIR>/nt386/wclaxp.exe      <OWRELROOT>/binnt/wclaxp.exe
    <CCCMD> <PROJDIR>/nt386/wclaxp.sym      <OWRELROOT>/binnt/wclaxp.sym
    <CCCMD> <PROJDIR>/os2386/wclaxp.exe     <OWRELROOT>/binp/wclaxp.exe
    <CCCMD> <PROJDIR>/os2386/wclaxp.sym     <OWRELROOT>/binp/wclaxp.sym
    <CCCMD> <PROJDIR>/linux386/wclaxp.exe   <OWRELROOT>/binl/wclaxp
    <CCCMD> <PROJDIR>/linux386/wclaxp.sym   <OWRELROOT>/binl/wclaxp.sym
    <CCCMD> <PROJDIR>/ntaxp/wclaxp.exe      <OWRELROOT>/axpnt/wclaxp.exe
    <CCCMD> <PROJDIR>/ntaxp/wclaxp.sym      <OWRELROOT>/axpnt/wclaxp.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
