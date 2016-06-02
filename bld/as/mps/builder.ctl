# wasmps Builder Control file
# ===========================

set PROJNAME=wasmps

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
    <CPCMD> <OWOBJDIR>/bwasmps.exe <OWBINDIR>/bwasmps<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwasmps<CMDEXT>
    rm -f <OWBINDIR>/bwasmps<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wasmps.exe     <OWRELROOT>/binw/
    <CCCMD> dos386/wasmps.sym     <OWRELROOT>/binw/
    <CCCMD> os2386/wasmps.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wasmps.sym     <OWRELROOT>/binp/
    <CCCMD> nt386/wasmps.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wasmps.sym      <OWRELROOT>/binnt/
    <CCCMD> linux386/wasmps.exe   <OWRELROOT>/binl/wasmps
    <CCCMD> linux386/wasmps.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wasmps.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wasmps.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wasmps.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wasmps.exe   <OWRELROOT>/binl64/wasmps
    <CCCMD> linuxarm/wasmps.exe   <OWRELROOT>/arml/wasmps

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
