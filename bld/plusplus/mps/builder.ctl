# wppaxp Builder Control file
# ===========================

set PROJNAME=wppmps

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
    <CPCMD> <OWOBJDIR>/bwppmps.exe     "<OWROOT>/build/<OWOBJDIR>/bwppmps<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwppdmps<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwppdmps<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwppmps<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppmps<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppdmps<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wppmps.exe       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppmps.sym       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppmps01.int     "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wppmps.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppmps.sym   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppdmps.dll  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppdmps.sym  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppmps01.int "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/wppmps.exe    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppmps.sym    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdmps.dll   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdmps.sym   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppmps01.int  "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wppmps.exe    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppmps.sym    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdmps.dll   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdmps.sym   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppmps01.int  "<OWRELROOT>/axpnt/"
    <CCCMD> linux386/wppmps.exe     "<OWRELROOT>/binl/wppmps"
    <CCCMD> linux386/wppmps.sym     "<OWRELROOT>/binl/"
    <CCCMD> linux386/wppmps01.int   "<OWRELROOT>/binl/"

    <CCCMD> bsdx64/wppmps.exe       "<OWRELROOT>/binb64/wppmps"
    <CCCMD> bsdx64/wppmps01.int     "<OWRELROOT>/binb64/"
    <CCCMD> ntx64.dll/wppmps.exe    "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppdmps.dll   "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppmps01.int  "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wppmps.exe     "<OWRELROOT>/binl64/wppmps"
    <CCCMD> linuxx64/wppmps.sym     "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/wppmps01.int   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wppmps.exe     "<OWRELROOT>/arml/wppmps"
    <CCCMD> linuxarm/wppmps01.int   "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wppmps.exe     "<OWRELROOT>/arml64/wppmps"
    <CCCMD> linuxa64/wppmps01.int   "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/wppmps.exe       "<OWRELROOT>/bino64/wppmps"
    <CCCMD> osxx64/wppmps01.int     "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wppmps.exe       "<OWRELROOT>/armo/wppmps"
    <CCCMD> osxarm/wppmps01.int     "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wppmps.exe       "<OWRELROOT>/armo64/wppmps"
    <CCCMD> osxa64/wppmps01.int     "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
