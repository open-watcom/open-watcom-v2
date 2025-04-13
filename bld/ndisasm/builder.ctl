# WDISASM Builder Control file
# ============================

set PROJNAME=wdis

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
    <CPCMD> <OWOBJDIR>/wdis.exe "<OWROOT>/build/<OWOBJDIR>/bwdis<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwdis<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwdis<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wdis.exe     "<OWRELROOT>/binw/"
    <CCCMD> dos386/wdis.sym     "<OWRELROOT>/binw/"
    <CCCMD> os2386/wdis.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/wdis.sym     "<OWRELROOT>/binp/"
    <CCCMD> nt386/wdis.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/wdis.sym      "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp/wdis.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/wdis.sym      "<OWRELROOT>/axpnt/"
    <CCCMD> qnx386/wdis.exe     "<OWRELROOT>/qnx/binq/wdis"
    <CCCMD> qnx386/wdis.sym     "<OWRELROOT>/qnx/sym/"
    <CCCMD> linux386/wdis.exe   "<OWRELROOT>/binl/wdis"
    <CCCMD> linux386/wdis.sym   "<OWRELROOT>/binl/"
    <CCCMD> rdos386/wdis.exe    "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/wdis.sym    "<OWRELROOT>/rdos/"

    <CCCMD> bsdx64/wdis.exe     "<OWRELROOT>/binb64/wdis"
    <CCCMD> ntx64/wdis.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wdis.exe   "<OWRELROOT>/binl64/wdis"
    <CCCMD> linuxx64/wdis.sym   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wdis.exe   "<OWRELROOT>/arml/wdis"
    <CCCMD> linuxa64/wdis.exe   "<OWRELROOT>/arml64/wdis"
    <CCCMD> osxx64/wdis.exe     "<OWRELROOT>/bino64/wdis"
    <CCCMD> osxarm/wdis.exe     "<OWRELROOT>/armo/wdis"
    <CCCMD> osxa64/wdis.exe     "<OWRELROOT>/armo64/wdis"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
