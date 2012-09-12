# FMEDIT Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=fmedit

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/fmedit.dll <OWRELROOT>/binw/
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/fmedit.dll <OWRELROOT>/binnt/
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/fmedit.dll <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
