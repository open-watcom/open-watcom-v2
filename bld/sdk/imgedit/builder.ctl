# IMAGE EDITOR Builder Control file
# =================================

set PROJDIR=<CWD>
set PROJNAME=wimgedit

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wimgedit.exe <OWRELROOT>/binw/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wimgedit.exe <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wimgedit.exe <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
