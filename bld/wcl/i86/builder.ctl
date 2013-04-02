# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcl

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwcl.exe <OWBINDIR>/bwcl<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwcl<CMDEXT>
    rm -f <OWBINDIR>/bwcl<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wcl.exe        <OWRELROOT>/binw/wcl.exe
    <CCCMD> dosi86/wcl.sym        <OWRELROOT>/binw/wcl.sym
    <CCCMD> nt386/wcl.exe         <OWRELROOT>/binnt/wcl.exe
    <CCCMD> nt386/wcl.sym         <OWRELROOT>/binnt/wcl.sym
    <CCCMD> os2386/wcl.exe        <OWRELROOT>/binp/wcl.exe
    <CCCMD> os2386/wcl.sym        <OWRELROOT>/binp/wcl.sym
    <CCCMD> linux386/wcl.exe      <OWRELROOT>/binl/wcl
    <CCCMD> linux386/wcl.sym      <OWRELROOT>/binl/wcl.sym
    <CCCMD> ntaxp/wcl.exe         <OWRELROOT>/axpnt/wcl.exe
    <CCCMD> ntaxp/wcl.sym         <OWRELROOT>/axpnt/wcl.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
