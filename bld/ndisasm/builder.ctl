# WDISASM Builder Control file
# ============================

set PROJNAME=wdis

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
    <CPCMD> <OWOBJDIR>/wdis.exe <OWBINDIR>/bwdis<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwdis<CMDEXT>
    rm -f <OWBINDIR>/bwdis<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
#    <CCCMD> osi386/wdis.exe   <OWRELROOT>/binw/
#    <CCCMD> osi386/wdis.sym   <OWRELROOT>/binw/
    <CCCMD> dos386/wdis.exe   <OWRELROOT>/binw/
    <CCCMD> dos386/wdis.sym   <OWRELROOT>/binw/
    <CCCMD> os2386/wdis.exe   <OWRELROOT>/binp/
    <CCCMD> os2386/wdis.sym   <OWRELROOT>/binp/
    <CCCMD> nt386/wdis.exe    <OWRELROOT>/binnt/
    <CCCMD> nt386/wdis.sym    <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wdis.exe    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wdis.sym    <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wdis.exe   <OWRELROOT>/qnx/binq/wdis
    <CCCMD> qnx386/wdis.sym   <OWRELROOT>/qnx/sym/
    <CCCMD> linux386/wdis.exe <OWRELROOT>/binl/wdis
    <CCCMD> linux386/wdis.sym <OWRELROOT>/binl/

    <CCCMD> ntx64/wdis.exe    <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wdis.exe <OWRELROOT>/binl64/wdis
    <CCCMD> linuxarm/wdis.exe <OWRELROOT>/arml/wdis

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
