# wcl Builder Control file
# ========================

set PROJNAME=wclppc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwclppc.exe <OWBINDIR>/<OWOBJDIR>/bwclppc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwclppc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwclppc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wclppc.exe     <OWRELROOT>/binw/
    <CCCMD> dosi86/wclppc.sym     <OWRELROOT>/binw/
    <CCCMD> nt386/wclppc.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wclppc.sym      <OWRELROOT>/binnt/
    <CCCMD> os2386/wclppc.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wclppc.sym     <OWRELROOT>/binp/

    <CCCMD> ntx64/wclppc.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wclppc.exe   <OWRELROOT>/binl64/wclppc
    <CCCMD> linuxarm/wclppc.exe   <OWRELROOT>/arml/wclppc
    <CCCMD> osxx64/wclppc.exe     <OWRELROOT>/osx64/wclppc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
