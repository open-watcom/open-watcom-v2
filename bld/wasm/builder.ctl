# WASM Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/wasm.exe     <OWRELROOT>/binw/wasm.exe
    <CPCMD> dos386/wasm.sym     <OWRELROOT>/binw/wasm.sym
    <CPCMD> dosi86/wasm.exe     <OWRELROOT>/binw/wasmr.exe
    <CPCMD> dosi86/wasm.sym     <OWRELROOT>/binw/wasmr.sym

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wasm.exe     <OWRELROOT>/binp/wasm.exe
    <CPCMD> os2386/wasm.sym     <OWRELROOT>/binp/wasm.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wasm.exe      <OWRELROOT>/binnt/wasm.exe
    <CPCMD> nt386/wasm.sym      <OWRELROOT>/binnt/wasm.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wasm.exe   <OWRELROOT>/binl/wasm
    <CPCMD> linux386/wasm.sym   <OWRELROOT>/binl/wasm.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wasm.exe      <OWRELROOT>/axpnt/wasm.exe
    <CPCMD> ntaxp/wasm.sym      <OWRELROOT>/axpnt/wasm.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wasm.exe     <OWRELROOT>/qnx/binq/wasm
    <CPCMD> qnx386/wasm.sym     <OWRELROOT>/qnx/sym/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
