# WDISASM Builder Control file
# ============================

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
#    <CPCMD> <PROJDIR>/osi386/wdis.exe   <RELROOT>/binw/
#    <CPCMD> <PROJDIR>/osi386/wdis.sym   <RELROOT>/binw/
    <CPCMD> <PROJDIR>/dos386/wdis.exe   <RELROOT>/binw/
    <CPCMD> <PROJDIR>/dos386/wdis.sym   <RELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/os2386/wdis.exe   <RELROOT>/binp/
    <CPCMD> <PROJDIR>/os2386/wdis.sym   <RELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/nt386/wdis.exe    <RELROOT>/binnt/
    <CPCMD> <PROJDIR>/nt386/wdis.sym    <RELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/ntaxp/wdis.exe    <RELROOT>/axpnt/
    <CPCMD> <PROJDIR>/ntaxp/wdis.sym    <RELROOT>/axpnt/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <PROJDIR>/qnx386/wdis.exe   <RELROOT>/qnx/binq/wdis
    <CPCMD> <PROJDIR>/qnx386/wdis.sym   <RELROOT>/qnx/sym/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/linux386/wdis.exe <RELROOT>/binl/wdis
    <CPCMD> <PROJDIR>/linux386/wdis.sym <RELROOT>/binl/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
