# asaxp Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <PROJDIR>/alpha/dos386/wasaxp.exe    <RELROOT>/binw/wasaxp.exe
    <CPCMD> <PROJDIR>/ppc/dos386/wasppc.exe      <RELROOT>/binw/wasppc.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/alpha/os2386/wasaxp.exe    <RELROOT>/binp/wasaxp.exe
    <CPCMD> <PROJDIR>/ppc/os2386/wasppc.exe      <RELROOT>/binp/wasppc.exe
    <CPCMD> <PROJDIR>/mips/os2386/wasmps.exe     <RELROOT>/binp/wasmps.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/alpha/nt386/wasaxp.exe     <RELROOT>/binnt/wasaxp.exe
    <CPCMD> <PROJDIR>/ppc/nt386/wasppc.exe       <RELROOT>/binnt/wasppc.exe
    <CPCMD> <PROJDIR>/mips/nt386/wasmps.exe      <RELROOT>/binnt/wasmps.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/alpha/ntaxp/wasaxp.exe     <RELROOT>/axpnt/wasaxp.exe
    <CPCMD> <PROJDIR>/ppc/ntaxp/wasppc.exe       <RELROOT>/axpnt/wasppc.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/alpha/linux386/wasaxp.exe  <RELROOT>/binl/wasaxp
    <CPCMD> <PROJDIR>/ppc/linux386/wasppc.exe    <RELROOT>/binl/wasppc
    <CPCMD> <PROJDIR>/mips/linux386/wasmps.exe   <RELROOT>/binl/wasmps

#    <CPCMD> <PROJDIR>/alpha/ntaxp/wasaxp.exe     <RELROOT>/axpnt/wasaxp.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
