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
    <CPCMD> dos386/wasm.exe     <RELROOT>/binw/wasm.exe
    <CPCMD> dos386/wasm.sym     <RELROOT>/binw/wasm.sym
    <CPCMD> dosi86/wasm.exe     <RELROOT>/binw/wasmr.exe
    <CPCMD> dosi86/wasm.sym     <RELROOT>/binw/wasmr.sym

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wasm.exe     <RELROOT>/binp/wasm.exe
    <CPCMD> os2386/wasm.sym     <RELROOT>/binp/wasm.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wasm.exe      <RELROOT>/binnt/wasm.exe
    <CPCMD> nt386/wasm.sym      <RELROOT>/binnt/wasm.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wasm.exe   <RELROOT>/binl/wasm
    <CPCMD> linux386/wasm.sym   <RELROOT>/binl/wasm.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wasm.exe      <RELROOT>/axpnt/wasm.exe
    <CPCMD> ntaxp/wasm.sym      <RELROOT>/axpnt/wasm.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wasm.exe     <RELROOT>/qnx/binq/wasm
    <CPCMD> qnx386/wasm.sym     <RELROOT>/qnx/sym/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
