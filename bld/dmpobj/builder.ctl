# dmpobj Builder Control file
# ===========================

set PROJNAME=dmpobj

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/dmpobj.exe "<OWROOT>/build/<OWOBJDIR>/bdmpobj<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bdmpobj<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bdmpobj<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/dmpobj.exe    "<OWRELROOT>/binw/"
    <CCCMD> os2386/dmpobj.exe    "<OWRELROOT>/binp/"
    <CCCMD> nt386/dmpobj.exe     "<OWRELROOT>/binnt/"
    <CCCMD> linux386/dmpobj.exe  "<OWRELROOT>/binl/dmpobj"
    <CCCMD> ntaxp/dmpobj.exe     "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/dmpobj.exe    "<OWRELROOT>/binb64/dmpobj"
    <CCCMD> ntx64/dmpobj.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/dmpobj.exe  "<OWRELROOT>/binl64/dmpobj"
    <CCCMD> linuxarm/dmpobj.exe  "<OWRELROOT>/arml/dmpobj"
    <CCCMD> linuxa64/dmpobj.exe  "<OWRELROOT>/arml64/dmpobj"
    <CCCMD> osxx64/dmpobj.exe    "<OWRELROOT>/bino64/dmpobj"
    <CCCMD> osxarm/dmpobj.exe    "<OWRELROOT>/armo/dmpobj"
    <CCCMD> osxa64/dmpobj.exe    "<OWRELROOT>/armo64/dmpobj"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
