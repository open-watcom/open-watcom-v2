# wasmps Builder Control file
# ===========================

set PROJNAME=wasmps

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
    <CPCMD> <OWOBJDIR>/bwasmps.exe "<OWROOT>/build/<OWOBJDIR>/bwasmps<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwasmps<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwasmps<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wasmps.exe     "<OWRELROOT>/binw/"
    <CCCMD> dos386/wasmps.sym     "<OWRELROOT>/binw/"
    <CCCMD> os2386/wasmps.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/wasmps.sym     "<OWRELROOT>/binp/"
    <CCCMD> nt386/wasmps.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/wasmps.sym      "<OWRELROOT>/binnt/"
    <CCCMD> linux386/wasmps.exe   "<OWRELROOT>/binl/wasmps"
    <CCCMD> linux386/wasmps.sym   "<OWRELROOT>/binl/"
    <CCCMD> ntaxp/wasmps.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/wasmps.sym      "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/wasmps.exe     "<OWRELROOT>/binb64/wasmps"
    <CCCMD> ntx64/wasmps.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wasmps.exe   "<OWRELROOT>/binl64/wasmps"
    <CCCMD> linuxx64/wasmps.sym   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wasmps.exe   "<OWRELROOT>/arml/wasmps"
    <CCCMD> linuxa64/wasmps.exe   "<OWRELROOT>/arml64/wasmps"
    <CCCMD> osxx64/wasmps.exe     "<OWRELROOT>/bino64/wasmps"
    <CCCMD> osxarm/wasmps.exe     "<OWRELROOT>/armo/wasmps"
    <CCCMD> osxa64/wasmps.exe     "<OWRELROOT>/armo64/wasmps"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
