# WMake Builder Control file
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
    <CPCMD> dosi86/wmk.exe    <RELROOT>/binw/wmaker.exe
    <CPCMD> dos386/wmk.exe    <RELROOT>/binw/wmake.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wmk.exe    <RELROOT>/binp/wmake.exe
    <CPCMD> os2386/wmk.sym    <RELROOT>/binp/wmake.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wmk.exe     <RELROOT>/binnt/wmake.exe
    <CPCMD> nt386/wmk.sym     <RELROOT>/binnt/wmake.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wmk.elf  <RELROOT>/binl/wmake
    <CPCMD> linux386/wmk.sym  <RELROOT>/binl/wmake.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wmk.exe     <RELROOT>/axpnt/wmake.exe

# I'm guessing for the QNX output, can't build it to check (Mat Nieuwenhoven)
  [ IFDEF (os_qnx)]
    <CPCMD> qnxi86/wmk.qnx          <RELROOT>/qnx/binq/wmake

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
