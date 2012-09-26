# WIC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/wic.exe <OWRELROOT>/binw/wic.exe
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wic.exe <OWRELROOT>/binp/wic.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wic.exe <OWRELROOT>/binnt/wic.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
