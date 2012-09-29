# IMAGE EDITOR Builder Control file
# =================================

set PROJDIR=<CWD>
set PROJNAME=wimgedit

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wimgedit.exe <OWRELROOT>/binw/
    <CCCMD> nt386/wimgedit.exe  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wimgedit.exe  <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
