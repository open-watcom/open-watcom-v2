# ide2make Builder Control file
# =============================

set PROJNAME=ide2make

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
    <CPCMD> <OWOBJDIR>/ide2make.exe "<OWROOT>/build/<OWOBJDIR>/bide2mak<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bide2mak<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bide2mak<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/ide2make.exe   "<OWRELROOT>/binw/"
    <CCCMD> dos386/ide2make.sym   "<OWRELROOT>/binw/"
    <CCCMD> os2386/ide2make.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2386/ide2make.sym   "<OWRELROOT>/binp/"
    <CCCMD> nt386/ide2make.exe    "<OWRELROOT>/binnt/"
    <CCCMD> nt386/ide2make.sym    "<OWRELROOT>/binnt/"
    <CCCMD> linux386/ide2make.exe "<OWRELROOT>/binl/ide2make"
    <CCCMD> linux386/ide2make.sym "<OWRELROOT>/binl/"
    <CCCMD> ntaxp/ide2make.exe    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/ide2make.sym    "<OWRELROOT>/axpnt/"

    <CCCMD> ntx64/ide2make.exe    "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/ide2make.exe "<OWRELROOT>/binl64/ide2make"
    <CCCMD> linuxx64/ide2make.sym "<OWRELROOT>/binl64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
