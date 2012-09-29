# Open Watcom Application Wizard control file
#============================================

set PROJDIR=<CWD>
set PROJNAME=appwiz

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
