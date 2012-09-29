# TECHINFO Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=techinfo

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/techinfo.exe <OWRELROOT>/binw/
    <CCCMD> os2i86/techinfo.exe <OWRELROOT>/binp/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
