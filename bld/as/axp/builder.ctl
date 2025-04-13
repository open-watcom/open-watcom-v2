# wasaxp Builder Control file
# ===========================

set PROJNAME=wasaxp

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
    <CPCMD> <OWOBJDIR>/bwasaxp.exe "<OWROOT>/build/<OWOBJDIR>/bwasaxp<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwasaxp<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwasaxp<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wasaxp.exe     "<OWRELROOT>/binw/"
    <CCCMD> dos386/wasaxp.sym     "<OWRELROOT>/binw/"
    <CCCMD> os2386/wasaxp.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/wasaxp.sym     "<OWRELROOT>/binp/"
    <CCCMD> nt386/wasaxp.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/wasaxp.sym      "<OWRELROOT>/binnt/"
    <CCCMD> linux386/wasaxp.exe   "<OWRELROOT>/binl/wasaxp"
    <CCCMD> linux386/wasaxp.sym   "<OWRELROOT>/binl/"
    <CCCMD> ntaxp/wasaxp.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/wasaxp.sym      "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/wasaxp.exe     "<OWRELROOT>/binb64/wasaxp"
    <CCCMD> ntx64/wasaxp.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wasaxp.exe   "<OWRELROOT>/binl64/wasaxp"
    <CCCMD> linuxx64/wasaxp.sym   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wasaxp.exe   "<OWRELROOT>/arml/wasaxp"
    <CCCMD> linuxa64/wasaxp.exe   "<OWRELROOT>/arml64/wasaxp"
    <CCCMD> osxx64/wasaxp.exe     "<OWRELROOT>/bino64/wasaxp"
    <CCCMD> osxarm/wasaxp.exe     "<OWRELROOT>/armo/wasaxp"
    <CCCMD> osxa64/wasaxp.exe     "<OWRELROOT>/armo64/wasaxp"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
