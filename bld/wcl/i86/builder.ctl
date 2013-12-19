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
    <CCCMD> dosi86/wcl.exe        <OWRELROOT>/binw/
    <CCCMD> dosi86/wcl.sym        <OWRELROOT>/binw/
    <CCCMD> nt386/wcl.exe         <OWRELROOT>/binnt/
    <CCCMD> nt386/wcl.sym         <OWRELROOT>/binnt/
    <CCCMD> os2386/wcl.exe        <OWRELROOT>/binp/
    <CCCMD> os2386/wcl.sym        <OWRELROOT>/binp/
    <CCCMD> linux386/wcl.exe      <OWRELROOT>/binl/wcl
    <CCCMD> linux386/wcl.sym      <OWRELROOT>/binl/
    <CCCMD> ntaxp/wcl.exe         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcl.sym         <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wcl.exe         <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wcl.exe      <OWRELROOT>/binl64/wcl

[ BLOCK . . ]
#============
cdsay <PROJDIR>
