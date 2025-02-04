# wppaxp Builder Control file
# ===========================

set PROJNAME=wppaxp

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
    <CPCMD> <OWOBJDIR>/bwppaxp.exe     "<OWROOT>/build/<OWOBJDIR>/bwppaxp<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwppdaxp<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwppdaxp<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwppaxp<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppaxp<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppdaxp<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wppaxp.exe       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppaxp.sym       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppaxp01.int     "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wppaxp.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppaxp.sym   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppdaxp.dll  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppdaxp.sym  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppaxp01.int "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/wppaxp.exe    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppaxp.sym    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdaxp.dll   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdaxp.sym   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppaxp01.int  "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wppaxp.exe    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppaxp.sym    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdaxp.dll   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdaxp.sym   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppaxp01.int  "<OWRELROOT>/axpnt/"
    <CCCMD> linux386/wppaxp.exe     "<OWRELROOT>/binl/wppaxp"
    <CCCMD> linux386/wppaxp.sym     "<OWRELROOT>/binl/"
    <CCCMD> linux386/wppaxp01.int   "<OWRELROOT>/binl/"

    <CCCMD> bsdx64/wppaxp.exe       "<OWRELROOT>/binb64/wppaxp"
    <CCCMD> bsdx64/wppaxp01.int     "<OWRELROOT>/binb64/"
    <CCCMD> ntx64.dll/wppaxp.exe    "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppdaxp.dll   "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppaxp01.int  "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wppaxp.exe     "<OWRELROOT>/binl64/wppaxp"
    <CCCMD> linuxx64/wppaxp.sym     "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/wppaxp01.int   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wppaxp.exe     "<OWRELROOT>/arml/wppaxp"
    <CCCMD> linuxarm/wppaxp01.int   "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wppaxp.exe     "<OWRELROOT>/arml64/wppaxp"
    <CCCMD> linuxa64/wppaxp01.int   "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/wppaxp.exe       "<OWRELROOT>/bino64/wppaxp"
    <CCCMD> osxx64/wppaxp01.int     "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wppaxp.exe       "<OWRELROOT>/armo/wppaxp"
    <CCCMD> osxarm/wppaxp01.int     "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wppaxp.exe       "<OWRELROOT>/armo64/wppaxp"
    <CCCMD> osxa64/wppaxp01.int     "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
