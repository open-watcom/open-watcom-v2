# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcl386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wcl386.exe <OWBINDIR>/bwcl386<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwcl386<CMDEXT>
    rm -f <OWBINDIR>/bwcl386<CMDEXT>

[ BLOCK <1> rel ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wcl386.exe     <OWRELROOT>/binw/wcl386.exe
    <CCCMD> dosi86/wcl386.sym     <OWRELROOT>/binw/wcl386.sym
    <CCCMD> nt386/wcl386.exe      <OWRELROOT>/binnt/wcl386.exe
    <CCCMD> nt386/wcl386.sym      <OWRELROOT>/binnt/wcl386.sym
    <CCCMD> os2386/wcl386.exe     <OWRELROOT>/binp/wcl386.exe
    <CCCMD> os2386/wcl386.sym     <OWRELROOT>/binp/wcl386.sym
    <CCCMD> linux386/wcl386.exe   <OWRELROOT>/binl/wcl386
    <CCCMD> linux386/wcl386.sym   <OWRELROOT>/binl/wcl386.sym
    <CCCMD> ntaxp/wcl386.exe      <OWRELROOT>/axpnt/wcl386.exe
    <CCCMD> ntaxp/wcl386.sym      <OWRELROOT>/axpnt/wcl386.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
