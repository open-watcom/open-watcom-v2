# WMake Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay .

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/wmk.exe    <OWRELROOT>/binw/wmaker.exe
    <CPCMD> dos386/wmk.exe    <OWRELROOT>/binw/wmake.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wmk.exe    <OWRELROOT>/binp/wmake.exe
    <CPCMD> os2386/wmk.sym    <OWRELROOT>/binp/wmake.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wmk.exe     <OWRELROOT>/binnt/wmake.exe
    <CPCMD> nt386/wmk.sym     <OWRELROOT>/binnt/wmake.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wmk.elf  <OWRELROOT>/binl/wmake
    <CPCMD> linux386/wmk.sym  <OWRELROOT>/binl/wmake.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wmk.exe     <OWRELROOT>/axpnt/wmake.exe

# I'm guessing for the QNX output, can't build it to check (Mat Nieuwenhoven)
  [ IFDEF (os_qnx)]
    <CPCMD> qnxi86/wmk.qnx          <OWRELROOT>/qnx/binq/wmake

[ BLOCK . . ]
#============
cdsay <PROJDIR>
