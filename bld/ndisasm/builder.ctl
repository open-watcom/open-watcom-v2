# WDISASM Builder Control file
# ============================

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
#    <CPCMD> osi386/wdis.exe   <OWRELROOT>/binw/
#    <CPCMD> osi386/wdis.sym   <OWRELROOT>/binw/
    <CPCMD> dos386/wdis.exe   <OWRELROOT>/binw/
    <CPCMD> dos386/wdis.sym   <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wdis.exe   <OWRELROOT>/binp/
    <CPCMD> os2386/wdis.sym   <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wdis.exe    <OWRELROOT>/binnt/
    <CPCMD> nt386/wdis.sym    <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wdis.exe    <OWRELROOT>/axpnt/
    <CPCMD> ntaxp/wdis.sym    <OWRELROOT>/axpnt/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wdis.exe   <OWRELROOT>/qnx/binq/wdis
    <CPCMD> qnx386/wdis.sym   <OWRELROOT>/qnx/sym/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wdis.exe <OWRELROOT>/binl/wdis
    <CPCMD> linux386/wdis.sym <OWRELROOT>/binl/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
