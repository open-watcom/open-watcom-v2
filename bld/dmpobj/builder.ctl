# dmpobj Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=dmpobj

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/dmpobj.exe <OWBINDIR>/bdmpobj<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bdmpobj<CMDEXT>
    rm -f <OWBINDIR>/bdmpobj<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/dmpobj.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/dmpobj.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/dmpobj.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/dmpobj.exe  <OWRELROOT>/binl/dmpobj
    <CCCMD> ntaxp/dmpobj.exe     <OWRELROOT>/axpnt/

    <CCCMD> ntx64/dmpobj.exe     <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/dmpobj.exe  <OWRELROOT>/binl64/dmpobj
    <CCCMD> linuxarm/dmpobj.exe  <OWRELROOT>/arml/dmpobj

[ BLOCK . . ]
#============
cdsay <PROJDIR>
