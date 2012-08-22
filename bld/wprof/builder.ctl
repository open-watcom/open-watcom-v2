# WPROF Builder Control file
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

[ BLOCK <1> rel2 cprel2 ]
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

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
