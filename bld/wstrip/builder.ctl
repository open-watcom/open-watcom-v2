# WSTRIP Builder Control file
# ===========================

set PROJNAME=wstrip

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wstripx.exe <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwstrip<CMDEXT>
    rm -f <OWBINDIR>/bwstrip<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/wstripx.exe    <OWRELROOT>/binw/wstrip.exe
    <CCCMD> os2386/wstripx.exe    <OWRELROOT>/binp/wstrip.exe
    <CCCMD> nt386/wstripx.exe     <OWRELROOT>/binnt/wstrip.exe
    <CCCMD> linux386/wstripx.exe  <OWRELROOT>/binl/wstrip
    <CCCMD> ntaxp/wstripx.exe     <OWRELROOT>/axpnt/wstrip.exe
    <CCCMD> qnx386/wstripx.exe    <OWRELROOT>/qnx/binq/wstrip

    <CCCMD> ntx64/wstripx.exe     <OWRELROOT>/binnt64/wstrip.exe
    <CCCMD> linuxx64/wstripx.exe  <OWRELROOT>/binl64/wstrip
    <CCCMD> linuxarm/wstripx.exe  <OWRELROOT>/arml/wstrip

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
