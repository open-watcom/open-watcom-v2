# wlib Builder Control file
# =========================

set PROJNAME=wlib

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
    <CPCMD> <OWOBJDIR>/bwlib.exe     "<OWROOT>/build/<OWOBJDIR>/bwlib<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwlibd<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwlibd<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwlib<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwlib<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwlibd<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wlib.exe         "<OWRELROOT>/binw/"
    <CCCMD> dos386/wlib.sym         "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wlib.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wlib.sym     "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wlibd.dll    "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wlibd.sym    "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/wlib.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlib.sym      "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlibd.dll     "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlibd.sym     "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wlib.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlib.sym      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlibd.dll     "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlibd.sym     "<OWRELROOT>/axpnt/"
    <CCCMD> qnx386/wlib.exe         "<OWRELROOT>/qnx/binq/wlib"
    <CCCMD> qnx386/wlib.sym         "<OWRELROOT>/qnx/sym/"
    <CCCMD> linux386/wlib.exe       "<OWRELROOT>/binl/wlib"
    <CCCMD> linux386/wlib.sym       "<OWRELROOT>/binl/"
    <CCCMD> rdos386/wlib.exe        "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/wlib.sym        "<OWRELROOT>/rdos/"

    <CCCMD> bsdx64/wlib.exe         "<OWRELROOT>/binb64/wlib"
    <CCCMD> ntx64.dll/wlib.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wlibd.dll     "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wlib.exe       "<OWRELROOT>/binl64/wlib"
    <CCCMD> linuxx64/wlib.sym       "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wlib.exe       "<OWRELROOT>/arml/wlib"
    <CCCMD> linuxa64/wlib.exe       "<OWRELROOT>/arml64/wlib"
    <CCCMD> osxx64/wlib.exe         "<OWRELROOT>/bino64/wlib"
    <CCCMD> osxarm/wlib.exe         "<OWRELROOT>/armo/wlib"
    <CCCMD> osxa64/wlib.exe         "<OWRELROOT>/armo64/wlib"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
