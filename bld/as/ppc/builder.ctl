# wasppc Builder Control file
# ===========================

set PROJNAME=wasppc

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
    <CPCMD> <OWOBJDIR>/bwasppc.exe <OWBINDIR>/<OWOBJDIR>/bwasppc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwasppc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwasppc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wasppc.exe     <OWRELROOT>/binw/
    <CCCMD> dos386/wasppc.sym     <OWRELROOT>/binw/
    <CCCMD> os2386/wasppc.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wasppc.sym     <OWRELROOT>/binp/
    <CCCMD> nt386/wasppc.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wasppc.sym      <OWRELROOT>/binnt/
    <CCCMD> linux386/wasppc.exe   <OWRELROOT>/binl/wasppc
    <CCCMD> linux386/wasppc.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wasppc.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wasppc.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wasppc.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wasppc.exe   <OWRELROOT>/binl64/wasppc
    <CCCMD> linuxarm/wasppc.exe   <OWRELROOT>/arml/wasppc
    <CCCMD> osxx64/wasppc.exe     <OWRELROOT>/osx64/wasppc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
