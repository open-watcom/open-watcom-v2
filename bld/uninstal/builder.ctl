# UNINSTAL Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=uninstal

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> nt386/uninstal.exe <OWRELROOT>/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
