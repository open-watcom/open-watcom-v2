# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wclmps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wclmps.exe <OWBINDIR>/bwclmps<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwclmps<CMDEXT>
    rm -f <OWBINDIR>/bwclmps<CMDEXT>

[ BLOCK <1> rel ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/nt386/wclmps.exe      <OWRELROOT>/binnt/wclmps.exe
    <CCCMD> <PROJDIR>/nt386/wclmps.sym      <OWRELROOT>/binnt/wclmps.sym
    <CCCMD> <PROJDIR>/os2386/wclmps.exe     <OWRELROOT>/binp/wclmps.exe
    <CCCMD> <PROJDIR>/os2386/wclmps.sym     <OWRELROOT>/binp/wclmps.sym
    <CCCMD> <PROJDIR>/linux386/wclmps.exe   <OWRELROOT>/binl/wclmps
    <CCCMD> <PROJDIR>/linux386/wclmps.sym   <OWRELROOT>/binl/wclmps.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
