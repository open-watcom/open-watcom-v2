# wcl Builder Control file
# ========================

set PROJNAME=wclaxp

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
    <CPCMD> <OWOBJDIR>/bwclaxp.exe "<OWROOT>/build/<OWOBJDIR>/bwclaxp<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwclaxp<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwclaxp<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wclaxp.exe     "<OWRELROOT>/binw/"
    <CCCMD> dosi86/wclaxp.sym     "<OWRELROOT>/binw/"
    <CCCMD> nt386/wclaxp.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/wclaxp.sym      "<OWRELROOT>/binnt/"
    <CCCMD> os2386/wclaxp.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/wclaxp.sym     "<OWRELROOT>/binp/"
    <CCCMD> linux386/wclaxp.exe   "<OWRELROOT>/binl/wclaxp"
    <CCCMD> linux386/wclaxp.sym   "<OWRELROOT>/binl/"
    <CCCMD> ntaxp/wclaxp.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/wclaxp.sym      "<OWRELROOT>/axpnt/"

    <CCCMD> bsdx64/wclaxp.exe     "<OWRELROOT>/binb64/wclaxp"
    <CCCMD> ntx64/wclaxp.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wclaxp.exe   "<OWRELROOT>/binl64/wclaxp"
    <CCCMD> linuxx64/wclaxp.sym   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wclaxp.exe   "<OWRELROOT>/arml/wclaxp"
    <CCCMD> linuxa64/wclaxp.exe   "<OWRELROOT>/arml64/wclaxp"
    <CCCMD> osxx64/wclaxp.exe     "<OWRELROOT>/bino64/wclaxp"
    <CCCMD> osxarm/wclaxp.exe     "<OWRELROOT>/armo/wclaxp"
    <CCCMD> osxa64/wclaxp.exe     "<OWRELROOT>/armo64/wclaxp"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
