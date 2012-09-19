# ide2make Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=bide2mak

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/bintool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/ide2make.exe <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/ide2make.exe <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/ide2make.exe <OWRELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/ide2make.exe <OWRELROOT>/binl/ide2make

  [ IFDEF (cpu_axp) <2*> ] 
    <CPCMD> ntaxp/ide2make.exe <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>

