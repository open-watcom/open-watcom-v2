# WPROF Builder Control file
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
    <CPCMD> <DEVDIR>/wprof/dos386/wprof.exe     <RELROOT>/binw/wprof.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/wprof/wini86/wprof.exe     <RELROOT>/binw/wprofw.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/wprof/os2386pm/wprof.exe   <RELROOT>/binp/wprof.exe
    <CPCMD> <DEVDIR>/wprof/os2386/wprof.exe     <RELROOT>/binp/wprofc.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/wprof/nt386/wprof.exe      <RELROOT>/binnt/wprof.exe
    <CPCMD> <DEVDIR>/wprof/nt386c/wprof.exe     <RELROOT>/binnt/wprofc.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/wprof/ntaxp/wprof.exe      <RELROOT>/axpnt/wprof.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <DEVDIR>/wprof/linux386/wprof.exe   <RELROOT>/binl/wprof
    <CPCMD> <DEVDIR>/wprof/linux386/wprof.sym   <RELROOT>/binl/wprof.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <DEVDIR>/wprof/qnx386/wprof.qnx     <RELROOT>/qnx/binq/wprof.
    <CPCMD> <DEVDIR>/wprof/qnx386/wprof.sym     <RELROOT>/qnx/sym/wprof.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
