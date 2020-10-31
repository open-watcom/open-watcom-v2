# dmpobj Builder Control file
# ===========================

set PROJNAME=dmpobj

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/dmpobj.exe <OWBINDIR>/<OWOBJDIR>/bdmpobj<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bdmpobj<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bdmpobj<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/dmpobj.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/dmpobj.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/dmpobj.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/dmpobj.exe  <OWRELROOT>/binl/dmpobj
    <CCCMD> ntaxp/dmpobj.exe     <OWRELROOT>/axpnt/

    <CCCMD> ntx64/dmpobj.exe     <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/dmpobj.exe  <OWRELROOT>/binl64/dmpobj
    <CCCMD> linuxarm/dmpobj.exe  <OWRELROOT>/arml/dmpobj
    <CCCMD> osxx64/dmpobj.exe    <OWRELROOT>/osx64/dmpobj

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
