# w32ldr Builder Control file
# ===========================

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

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_dos "") <2*> ]
   <CPCMD> dos386/cwsrun.exe   <RELROOT>/binw/w32run.exe
#   <CPCMD> dos386/x32run.exe   <RELROOT>/binw/x32run.exe
#   <CPCMD> dos386/x32run.exe   <RELROOT>/binw/x32run.exe
#   <CPCMD> dos386/d4grun.exe   <RELROOT>/binw/d4grun.exe
#   <CPCMD> dos386/tntrun.exe   <RELROOT>/binw/tntrun.exe

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> os2386/w32bind.exe  ../build/binp/w32bind.exe
#    <CPCMD> os2386/os2ldr.exe   ../build/binp/os2ldr.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/w32run.exe  <RELROOT>/binnt/w32run.exe
#    these are now real NT exe's - don't copy the stub
#    <CPCMD> nt/*.exe           <RELROOT>/binnt/

[ BLOCK <1> clean ]
#=================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
