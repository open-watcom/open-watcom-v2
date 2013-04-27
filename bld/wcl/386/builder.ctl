# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcl386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwcl386.exe <OWBINDIR>/bwcl386<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwcl386<CMDEXT>
    rm -f <OWBINDIR>/bwcl386<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wcl386.exe     <OWRELROOT>/binw/
    <CCCMD> dosi86/wcl386.sym     <OWRELROOT>/binw/
    <CCCMD> nt386/wcl386.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wcl386.sym      <OWRELROOT>/binnt/
    <CCCMD> os2386/wcl386.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wcl386.sym     <OWRELROOT>/binp/
    <CCCMD> linux386/wcl386.exe   <OWRELROOT>/binl/wcl386
    <CCCMD> linux386/wcl386.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wcl386.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcl386.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wcl386.exe      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
