# HEAPWALKER Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=whepwlk

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wheapwlk.exe <OWRELROOT>/binw/wheapwlk.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
