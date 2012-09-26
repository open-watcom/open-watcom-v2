# TECHINFO Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/techinfo.exe <OWRELROOT>/binw/
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2i86/techinfo.exe <OWRELROOT>/binp/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
