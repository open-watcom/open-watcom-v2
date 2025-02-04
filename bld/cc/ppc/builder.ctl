# wccppc Builder Control file
# ===========================

set PROJNAME=wccppc

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
    <CPCMD> <OWOBJDIR>/bwccppc.exe     "<OWROOT>/build/<OWOBJDIR>/bwccppc<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwccdppc<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwccdppc<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwccppc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwccppc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwccdppc<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/binw/"
    <CCCMD> dos386/<OWOBJDIR>/wccppc.sym        "<OWRELROOT>/binw/"
    <CCCMD> dos386/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/binw/"
    <CCCMD> os2386.dll/<OWOBJDIR>/wccppc.exe    "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/<OWOBJDIR>/wccppc.sym    "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/<OWOBJDIR>/wccdppc.dll   "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/<OWOBJDIR>/wccdppc.sym   "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/<OWOBJDIR>/wccppc01.int  "<OWRELROOT>/binp/dll/"
    <CCCMD> nt386.dll/<OWOBJDIR>/wccppc.exe     "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/<OWOBJDIR>/wccppc.sym     "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/<OWOBJDIR>/wccdppc.dll    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/<OWOBJDIR>/wccdppc.sym    "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/<OWOBJDIR>/wccppc01.int   "<OWRELROOT>/binnt/"
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccppc.exe     "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccppc.sym     "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccdppc.dll    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccdppc.sym    "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccppc01.int   "<OWRELROOT>/axpnt/"
    <CCCMD> qnx386/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/qnx/binq/wccppc"
    <CCCMD> qnx386/<OWOBJDIR>/wccppc.sym        "<OWRELROOT>/qnx/sym/"
    <CCCMD> qnx386/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/qnx/binq/"
    <CCCMD> linux386/<OWOBJDIR>/wccppc.exe      "<OWRELROOT>/binl/wccppc"
    <CCCMD> linux386/<OWOBJDIR>/wccppc.sym      "<OWRELROOT>/binl/"
    <CCCMD> linux386/<OWOBJDIR>/wccppc01.int    "<OWRELROOT>/binl/"

    <CCCMD> bsdx64/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/binb64/wccppc"
    <CCCMD> bsdx64/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/binb64/"
    <CCCMD> ntx64.dll/<OWOBJDIR>/wccppc.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/<OWOBJDIR>/wccdppc.dll    "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/<OWOBJDIR>/wccppc01.int   "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/<OWOBJDIR>/wccppc.exe      "<OWRELROOT>/binl64/wccppc"
    <CCCMD> linuxx64/wccppc.sym                 "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/<OWOBJDIR>/wccppc01.int    "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/<OWOBJDIR>/wccppc.exe      "<OWRELROOT>/arml/wccppc"
    <CCCMD> linuxarm/<OWOBJDIR>/wccppc01.int    "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/<OWOBJDIR>/wccppc.exe      "<OWRELROOT>/arml64/wccppc"
    <CCCMD> linuxa64/<OWOBJDIR>/wccppc01.int    "<OWRELROOT>/arml64/"
    <CCCMD> osxx64/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/bino64/wccppc"
    <CCCMD> osxx64/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/armo/wccppc"
    <CCCMD> osxarm/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/armo/"
    <CCCMD> osxa64/<OWOBJDIR>/wccppc.exe        "<OWRELROOT>/armo64/wccppc"
    <CCCMD> osxa64/<OWOBJDIR>/wccppc01.int      "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
