# wppaxp Builder Control file
# ===========================

set PROJNAME=wppppc

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
    <CPCMD> <OWOBJDIR>/bwppppc.exe     "<OWROOT>/build/<OWOBJDIR>/bwppppc<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwppdppc<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwppdppc<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwppppc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppppc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwppdppc<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wppppc.exe       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppppc.sym       "<OWRELROOT>/binw/"
    <CCCMD> dos386/wppppc01.int     "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/wppppc.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppppc.sym   "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wppdppc.dll  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppdppc.sym  "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wppppc01.int "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/wppppc.exe    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppppc.sym    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdppc.dll   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppdppc.sym   "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wppppc01.int  "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/wppppc.exe    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppppc.sym    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdppc.dll   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppdppc.sym   "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wppppc01.int  "<OWRELROOT>/axpnt/"
    <CCCMD> linux386/wppppc.exe     "<OWRELROOT>/binl/wppppc"
    <CCCMD> linux386/wppppc.sym     "<OWRELROOT>/binl/"
    <CCCMD> linux386/wppppc01.int   "<OWRELROOT>/binl/"

    <CCCMD> bsdx64/wppppc.exe       "<OWRELROOT>/binb64/wppppc"
    <CCCMD> bsdx64/wppppc01.int     "<OWRELROOT>/binb64/"
    <CCCMD> ntx64.dll/wppppc.exe    "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppdppc.dll   "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wppppc01.int  "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/wppppc.exe     "<OWRELROOT>/binl64/wppppc"
    <CCCMD> linuxx64/wppppc.sym     "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/wppppc01.int   "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/wppppc.exe     "<OWRELROOT>/arml/wppppc"
    <CCCMD> linuxarm/wppppc01.int   "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wppppc.exe     "<OWRELROOT>/arml64/wppppc"
    <CCCMD> linuxa64/wppppc01.int   "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/wppppc.exe       "<OWRELROOT>/bino64/wppppc"
    <CCCMD> osxx64/wppppc01.int     "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wppppc.exe       "<OWRELROOT>/armo/wppppc"
    <CCCMD> osxarm/wppppc01.int     "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wppppc.exe       "<OWRELROOT>/armo64/wppppc"
    <CCCMD> osxa64/wppppc01.int     "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
