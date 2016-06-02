# wpp386 Builder Control file
# ===========================

set PROJNAME=wpp386

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwpp386.exe     <OWBINDIR>/bwpp386<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwppd386<DYEXT> <OWBINDIR>/bwppd386<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwpp386<CMDEXT>
    rm -f <OWBINDIR>/bwpp386<CMDEXT>
    rm -f <OWBINDIR>/bwppd386<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wpp386.exe       <OWRELROOT>/binw/
    <CCCMD> dos386/wpp386.sym       <OWRELROOT>/binw/
    <CCCMD> dos386/wpp38601.int     <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wpp386.exe   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wpp386.sym   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wppd386.dll  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wppd386.sym  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wpp38601.int <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wpp386.exe    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wpp386.sym    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppd386.dll   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppd386.sym   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wpp38601.int  <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wpp386.exe    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wpp386.sym    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppd386.dll   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppd386.sym   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wpp38601.int  <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wpp386.exe       <OWRELROOT>/qnx/binq/wpp386
    <CCCMD> qnx386/wpp386.sym       <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wpp38601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wpp386.exe     <OWRELROOT>/binl/wpp386
    <CCCMD> linux386/wpp386.sym     <OWRELROOT>/binl/
    <CCCMD> linux386/wpp38601.int   <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wpp386.exe    <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wppd386.dll   <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wpp38601.int  <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wpp386.exe     <OWRELROOT>/binl64/wpp386
    <CCCMD> linuxx64/wpp38601.int   <OWRELROOT>/binl64/
    <CCCMD> linuxarm/wpp386.exe     <OWRELROOT>/arml/wpp386
    <CCCMD> linuxarm/wpp38601.int   <OWRELROOT>/arml/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
