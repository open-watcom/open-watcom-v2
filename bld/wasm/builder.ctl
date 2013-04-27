# wasm Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=wasm

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwasm.exe <OWBINDIR>/bwasm<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasm<CMDEXT>
    rm -f <OWBINDIR>/bwasm<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasm.exe     <OWRELROOT>/binw/
    <CCCMD> dos386/wasm.sym     <OWRELROOT>/binw/
    <CCCMD> dosi86/wasm.exe     <OWRELROOT>/binw/wasmr.exe
    <CCCMD> dosi86/wasm.sym     <OWRELROOT>/binw/wasmr.sym
    <CCCMD> os2386/wasm.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wasm.sym     <OWRELROOT>/binp/
    <CCCMD> nt386/wasm.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wasm.sym      <OWRELROOT>/binnt/
    <CCCMD> linux386/wasm.exe   <OWRELROOT>/binl/wasm
    <CCCMD> linux386/wasm.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wasm.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wasm.sym      <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wasm.exe     <OWRELROOT>/qnx/binq/wasm
    <CCCMD> qnx386/wasm.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/wasm.exe      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============

cdsay <PROJDIR>
