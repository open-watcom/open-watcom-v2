# WSTRIP Builder Control file
# ===========================

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
    <CPCMD> dosi86/wstripx.exe    <OWRELROOT>/binw/wstrip.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wstripx.exe    <OWRELROOT>/binp/wstrip.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wstripx.exe     <OWRELROOT>/binnt/wstrip.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wstripx.exe  <OWRELROOT>/binl/wstrip

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wstripx.exe     <OWRELROOT>/axpnt/wstrip.exe

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wstripx.exe    <OWRELROOT>/qnx/binq/wstrip.

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
