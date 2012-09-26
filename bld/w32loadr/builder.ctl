# w32ldr Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=w32ldr

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
   <CPCMD> dos386/cwsrun.exe   <OWRELROOT>/binw/w32run.exe
#   <CPCMD> dos386/x32run.exe   <OWRELROOT>/binw/x32run.exe
#   <CPCMD> dos386/x32run.exe   <OWRELROOT>/binw/x32run.exe
#   <CPCMD> dos386/d4grun.exe   <OWRELROOT>/binw/d4grun.exe
#   <CPCMD> dos386/tntrun.exe   <OWRELROOT>/binw/tntrun.exe

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> os2386/w32bind.exe  ../build/binp/w32bind.exe
#    <CPCMD> os2386/os2ldr.exe   ../build/binp/os2ldr.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/w32run.exe  <OWRELROOT>/binnt/w32run.exe
#    these are now real NT exe's - don't copy the stub
#    <CPCMD> nt/*.exe           <OWRELROOT>/binnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
