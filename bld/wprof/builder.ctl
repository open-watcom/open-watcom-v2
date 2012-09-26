# WPROF Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wprof

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/wprof.exe     <OWRELROOT>/binw/wprof.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wprof.exe     <OWRELROOT>/binw/wprofw.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386pm/wprof.exe   <OWRELROOT>/binp/wprof.exe
    <CPCMD> os2386/wprof.exe     <OWRELROOT>/binp/wprofc.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wprof.exe      <OWRELROOT>/binnt/wprof.exe
    <CPCMD> nt386c/wprof.exe     <OWRELROOT>/binnt/wprofc.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wprof.exe      <OWRELROOT>/axpnt/wprof.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wprof.exe   <OWRELROOT>/binl/wprof
    <CPCMD> linux386/wprof.sym   <OWRELROOT>/binl/wprof.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wprof.qnx     <OWRELROOT>/qnx/binq/wprof.
    <CPCMD> qnx386/wprof.sym     <OWRELROOT>/qnx/sym/wprof.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
