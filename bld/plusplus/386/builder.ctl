# wpp386 Builder Control file
# ===========================

set PROJNAME=wpp386

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
    <CPCMD> <OWOBJDIR>/bwpp386.exe     "<OWROOT>/build/<OWOBJDIR>/bwpp386<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwppd386<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwppd386<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwpp386<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwpp386<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppd386<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wpp386.exe       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wpp386.sym       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wpp38601.int     "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wpp386.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wpp386.sym   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppd386.dll  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppd386.sym  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wpp38601.int "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/wpp386.exe    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wpp386.sym    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppd386.dll   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppd386.sym   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wpp38601.int  "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wpp386.exe    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wpp386.sym    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppd386.dll   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppd386.sym   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wpp38601.int  "<OWRELROOT>/axpnt/"
    <CCCMD> qnx386/wpp386.exe       "<OWRELROOT>/qnx/binq/wpp386"
    <CCCMD> qnx386/wpp386.sym       "<OWRELROOT>/qnx/sym/"
    <CCCMD> qnx386/wpp38601.int     "<OWRELROOT>/qnx/binq/"
    <CCCMD> linux386/wpp386.exe     "<OWRELROOT>/binl/wpp386"
    <CCCMD> linux386/wpp386.sym     "<OWRELROOT>/binl/"
    <CCCMD> linux386/wpp38601.int   "<OWRELROOT>/binl/"
    <CCCMD> rdos386/wpp386.exe      "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/wpp386.sym      "<OWRELROOT>/rdos/"

    <CCCMD> bsdx64/wpp386.exe       "<OWRELROOT>/binb64/wpp386"
    <CCCMD> bsdx64/wpp38601.int     "<OWRELROOT>/binb64/"
    <CCCMD> ntx64.dll/wpp386.exe    "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppd386.dll   "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wpp38601.int  "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wpp386.exe     "<OWRELROOT>/binl64/wpp386"
    <CCCMD> linuxx64/wpp386.sym     "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/wpp38601.int   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wpp386.exe     "<OWRELROOT>/arml/wpp386"
    <CCCMD> linuxarm/wpp38601.int   "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wpp386.exe     "<OWRELROOT>/arml64/wpp386"
    <CCCMD> linuxa64/wpp38601.int   "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/wpp386.exe       "<OWRELROOT>/bino64/wpp386"
    <CCCMD> osxx64/wpp38601.int     "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wpp386.exe       "<OWRELROOT>/armo/wpp386"
    <CCCMD> osxarm/wpp38601.int     "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wpp386.exe       "<OWRELROOT>/armo64/wpp386"
    <CCCMD> osxa64/wpp38601.int     "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
