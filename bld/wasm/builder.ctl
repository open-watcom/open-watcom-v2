# wasm Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=wasm

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK ( <1> <BINTOOL> ) rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#======================
    <CPCMD> <OWOBJDIR>/bwasm.exe <OWBINDIR>/bwasm<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasm<CMDEXT>
    rm -f <OWBINDIR>/bwasm<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasm.exe     <OWRELROOT>/binw/wasm.exe
    <CCCMD> dos386/wasm.sym     <OWRELROOT>/binw/wasm.sym
    <CCCMD> dosi86/wasm.exe     <OWRELROOT>/binw/wasmr.exe
    <CCCMD> dosi86/wasm.sym     <OWRELROOT>/binw/wasmr.sym
    <CCCMD> os2386/wasm.exe     <OWRELROOT>/binp/wasm.exe
    <CCCMD> os2386/wasm.sym     <OWRELROOT>/binp/wasm.sym
    <CCCMD> nt386/wasm.exe      <OWRELROOT>/binnt/wasm.exe
    <CCCMD> nt386/wasm.sym      <OWRELROOT>/binnt/wasm.sym
    <CCCMD> linux386/wasm.exe   <OWRELROOT>/binl/wasm
    <CCCMD> linux386/wasm.sym   <OWRELROOT>/binl/wasm.sym
    <CCCMD> ntaxp/wasm.exe      <OWRELROOT>/axpnt/wasm.exe
    <CCCMD> ntaxp/wasm.sym      <OWRELROOT>/axpnt/wasm.sym
    <CCCMD> qnx386/wasm.exe     <OWRELROOT>/qnx/binq/wasm
    <CCCMD> qnx386/wasm.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============

cdsay <PROJDIR>
