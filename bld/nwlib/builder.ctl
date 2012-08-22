# NWLIB Builder Control file
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
    <CPCMD> dos386/wlib.exe  <OWRELROOT>/binw/
    <CPCMD> dos386/wlib.sym  <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wlib.exe  <OWRELROOT>/binp/
    <CPCMD> os2386/wlibd.dll <OWRELROOT>/binp/dll/
    <CPCMD> os2386/wlibd.sym <OWRELROOT>/binp/dll/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wlib.exe   <OWRELROOT>/binnt/
    <CPCMD> nt386/wlibd.dll  <OWRELROOT>/binnt/
    <CPCMD> nt386/wlibd.sym  <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wlib.exe   <OWRELROOT>/axpnt/
    <CPCMD> ntaxp/wlibd.dll  <OWRELROOT>/axpnt/
    <CPCMD> ntaxp/wlibd.sym  <OWRELROOT>/axpnt/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wlib.exe  <OWRELROOT>/qnx/binq/wlib
    <CPCMD> qnx386/wlib.sym  <OWRELROOT>/qnx/sym/wlib.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wlib.exe  <OWRELROOT>/binl/wlib
    <CPCMD> linux386/wlib.sym  <OWRELROOT>/binl/wlib.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
