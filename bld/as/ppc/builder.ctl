# wasppc Builder Control file
# ===========================

set PROJNAME=wasppc

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
    <CPCMD> <OWOBJDIR>/bwasppc.exe "<OWROOT>/build/<OWOBJDIR>/bwasppc<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwasppc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwasppc<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wasppc.exe     "<OWRELROOT>/binw/"
    <CCCMD> dos386/wasppc.sym     "<OWRELROOT>/binw/"
    <CCCMD> os2386/wasppc.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/wasppc.sym     "<OWRELROOT>/binp/"
    <CCCMD> nt386/wasppc.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/wasppc.sym      "<OWRELROOT>/binnt/"
    <CCCMD> linux386/wasppc.exe   "<OWRELROOT>/binl/wasppc"
    <CCCMD> linux386/wasppc.sym   "<OWRELROOT>/binl/"
    <CCCMD> ntaxp/wasppc.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/wasppc.sym      "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/wasppc.exe     "<OWRELROOT>/binb64/wasppc"
    <CCCMD> ntx64/wasppc.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wasppc.exe   "<OWRELROOT>/binl64/wasppc"
    <CCCMD> linuxx64/wasppc.sym   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wasppc.exe   "<OWRELROOT>/arml/wasppc"
    <CCCMD> linuxa64/wasppc.exe   "<OWRELROOT>/arml64/wasppc"
    <CCCMD> osxx64/wasppc.exe     "<OWRELROOT>/bino64/wasppc"
    <CCCMD> osxarm/wasppc.exe     "<OWRELROOT>/armo/wasppc"
    <CCCMD> osxa64/wasppc.exe     "<OWRELROOT>/armo64/wasppc"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
