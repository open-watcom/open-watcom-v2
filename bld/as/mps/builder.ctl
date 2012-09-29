# wasmps Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wasmps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK ( <1> <BINTOOL> ) rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#======================
    <CPCMD> <OWOBJDIR>/bwasmps.exe <OWBINDIR>/bwasmps<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasmps<CMDEXT>
    rm -f <OWBINDIR>/bwasmps<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasmps.exe     <OWRELROOT>/binw/wasmps.exe
    <CCCMD> dos386/wasmps.sym     <OWRELROOT>/binw/wasmps.sym
    <CCCMD> os2386/wasmps.exe     <OWRELROOT>/binp/wasmps.exe
    <CCCMD> os2386/wasmps.sym     <OWRELROOT>/binp/wasmps.sym
    <CCCMD> nt386/wasmps.exe      <OWRELROOT>/binnt/wasmps.exe
    <CCCMD> nt386/wasmps.sym      <OWRELROOT>/binnt/wasmps.sym
    <CCCMD> linux386/wasmps.exe   <OWRELROOT>/binl/wasmps
    <CCCMD> linux386/wasmps.sym   <OWRELROOT>/binl/wasmps.sym
    <CCCMD> ntaxp/wasmps.exe      <OWRELROOT>/axpnt/wasmps.exe
    <CCCMD> ntaxp/wasmps.sym      <OWRELROOT>/axpnt/wasmps.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
