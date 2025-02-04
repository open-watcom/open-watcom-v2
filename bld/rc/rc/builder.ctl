# Resource Compiler Builder Control file
# ======================================

set PROJNAME=wrc

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
    <CPCMD> <OWOBJDIR>/bwrc.exe     "<OWROOT>/build/<OWOBJDIR>/bwrc<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwrcd<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwrcd<DYEXT>"
    <CPCMD> <OWOBJDIR>/kanji.uni    "<OWROOT>/build/<OWOBJDIR>/"
    <CPCMD> <OWOBJDIR>/to932.uni    "<OWROOT>/build/<OWOBJDIR>/"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwrc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwrc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwrcd<DYEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/kanji.uni"
    rm -f "<OWROOT>/build/<OWOBJDIR>/kanji.uni"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/to932.uni"
    rm -f "<OWROOT>/build/<OWOBJDIR>/to932.uni"

[ BLOCK <BLDRULE> rel cprel ]
#============================

    <CCCMD> dos386/wrc.exe          "<OWRELROOT>/binw/"
    <CCCMD> dos386/wrc.sym          "<OWRELROOT>/binw/"
    <CCCMD> dos386/*.uni            "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wrc.exe      "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wrc.sym      "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wrcd.dll     "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wrcd.sym     "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/*.uni        "<OWRELROOT>/binp/"
    <CCCMD> nt386.dll/wrc.exe       "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wrc.sym       "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wrcd.dll      "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wrcd.sym      "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/*.uni         "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wrc.exe       "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wrc.sym       "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wrcd.dll      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wrcd.sym      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/*.uni         "<OWRELROOT>/axpnt/"
    <CCCMD> linux386/wrc.exe        "<OWRELROOT>/binl/wrc"
    <CCCMD> linux386/wrc.sym        "<OWRELROOT>/binl/"
    <CCCMD> linux386/*.uni          "<OWRELROOT>/binl/"
    <CCCMD> rdos386/wrc.exe         "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/wrc.sym         "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/*.uni           "<OWRELROOT>/rdos/"

    <CCCMD> linuxx64/wrc.exe        "<OWRELROOT>/binl64/wrc"
    <CCCMD> linuxx64/wrc.sym        "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/*.uni          "<OWRELROOT>/binl64/"
    <CCCMD> ntx64.dll/wrc.exe       "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wrcd.dll      "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/*.uni         "<OWRELROOT>/binnt64/"

    <CCCMD> bsdx64/wrc.exe          "<OWRELROOT>/binb64/wrc"
    <CCCMD> bsdx64/*.uni            "<OWRELROOT>/binb64/"
    <CCCMD> linuxarm/wrc.exe        "<OWRELROOT>/arml/wrc"
    <CCCMD> linuxarm/wrc.sym        "<OWRELROOT>/arml/"
    <CCCMD> linuxarm/*.uni          "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wrc.exe        "<OWRELROOT>/arml64/wrc"
    <CCCMD> linuxa64/wrc.sym        "<OWRELROOT>/arml64/"
    <CCCMD> linuxa64/*.uni          "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/wrc.exe          "<OWRELROOT>/bino64/wrc"
    <CCCMD> osxx64/*.uni            "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wrc.exe          "<OWRELROOT>/armo/wrc"
    <CCCMD> osxarm/*.uni            "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wrc.exe          "<OWRELROOT>/armo64/wrc"
    <CCCMD> osxa64/*.uni            "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
