# wcl Builder Control file
# ========================

set PROJNAME=owcc

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
    <CPCMD> <OWOBJDIR>/bowcc.exe "<OWROOT>/build/<OWOBJDIR>/bowcc<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bspecs.owc "<OWROOT>/build/<OWOBJDIR>/bspecs.owc"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bowcc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bowcc<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bspecs.owc"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bspecs.owc"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/specs.owc     "<OWRELROOT>/binw/"
    <CCCMD> dosi86/owcc.exe      "<OWRELROOT>/binw/"
    <CCCMD> dosi86/owcc.sym      "<OWRELROOT>/binw/"
    <CCCMD> nt386/specs.owc      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/owcc.exe       "<OWRELROOT>/binnt/"
    <CCCMD> nt386/owcc.sym       "<OWRELROOT>/binnt/"
    <CCCMD> os2386/specs.owc     "<OWRELROOT>/binp/"
    <CCCMD> os2386/owcc.exe      "<OWRELROOT>/binp/"
    <CCCMD> os2386/owcc.sym      "<OWRELROOT>/binp/"
    <CCCMD> linux386/specs.owc   "<OWRELROOT>/binl/"
    <CCCMD> linux386/owcc.exe    "<OWRELROOT>/binl/owcc"
    <CCCMD> linux386/owcc.sym    "<OWRELROOT>/binl/"

    <CCCMD> bsdx64/specs.owc     "<OWRELROOT>/binb64/"
    <CCCMD> bsdx64/owcc.exe      "<OWRELROOT>/binb64/owcc"
    <CCCMD> ntx64/specs.owc      "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64/owcc.exe       "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/specs.owc   "<OWRELROOT>/binl64/"
    <CCCMD> linuxx64/owcc.exe    "<OWRELROOT>/binl64/owcc"
    <CCCMD> linuxx64/owcc.sym    "<OWRELROOT>/binl64/"
    <CCCMD> linuxarm/specs.owc   "<OWRELROOT>/arml/"
    <CCCMD> linuxarm/owcc.exe    "<OWRELROOT>/arml/owcc"
    <CCCMD> linuxa64/specs.owc   "<OWRELROOT>/arml64/"
    <CCCMD> linuxa64/owcc.exe    "<OWRELROOT>/arml64/owcc"
    <CCCMD> osxx64/specs.owc     "<OWRELROOT>/bino64/"
    <CCCMD> osxx64/owcc.exe      "<OWRELROOT>/bino64/owcc"
    <CCCMD> osxarm/specs.owc     "<OWRELROOT>/armo/"
    <CCCMD> osxarm/owcc.exe      "<OWRELROOT>/armo/owcc"
    <CCCMD> osxa64/specs.owc     "<OWRELROOT>/armo64/"
    <CCCMD> osxa64/owcc.exe      "<OWRELROOT>/armo64/owcc"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
