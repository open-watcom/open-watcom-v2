# WDISASM Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wdis

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
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

[ BLOCK . . ]
#============

cdsay <PROJDIR>
